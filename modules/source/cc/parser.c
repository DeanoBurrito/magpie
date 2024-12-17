#include <cc/parser.h>
#include <core/alloc.h>

#define MP_NO_PRECEDENCE -1
#define GET_SOURCE_POS(parser, index) (mp_source_pos_from_index(mp_cc_lex_get_source((parser)->lexer), index))
#define EMIT_ERROR_MISMATCH_TOKEN(msg, ...) EMIT_ERROR(parser->diags, GET_SOURCE_POS(parser, mp_cc_lex_peek(parser->lexer, 0).begin), (msg), ##__VA_ARGS__)

struct mp_cc_parser
{
    struct mp_cc_lexer* lexer;
    struct mp_cc_parser_options options;
    struct mp_diagnostics* diags;
};

static struct mp_cc_ast_node* parse_expr(struct mp_cc_parser* parser);
static struct mp_cc_ast_node* parse_paren_expr(struct mp_cc_parser* parser);

static bool try_parse_type_spec(struct mp_cc_parser* parser, struct mp_cc_type_spec* spec)
{
    struct mp_cc_lex_token first = mp_cc_lex_next(parser->lexer);
    return first.type == lex_token_type_identifier; //TODO: proper type spec parsing
}

static void parse_decln_parts(struct mp_cc_parser* parser, struct mp_cc_decln* decln)
{
    bool has_type_spec = false;
    while (true)
    {
        bool consume_next = true;
        struct mp_cc_lex_token peek = mp_cc_lex_peek(parser->lexer, 0);

        switch (peek.type)
        {
            case lex_token_type_kw_constexpr:
                decln->flags |= decln_flag_constexpr;
                break;
            case lex_token_type_kw_auto:
                decln->flags |= decln_flag_register;
                break;
            case lex_token_type_kw_static:
                decln->flags |= decln_flag_static;
                break;
            case lex_token_type_kw_extern:
                decln->flags |= decln_flag_extern;
                break;
            case lex_token_type_kw_thread_local:
                decln->flags |= decln_flag_thread_local;
                break;
            case lex_token_type_kw_const:
                decln->flags |= decln_flag_const;
                break;
            case lex_token_type_kw_volatile:
                decln->flags |= decln_flag_volatile;
                break;
            case lex_token_type_kw_restrict:
                decln->flags |= decln_flag_restrict;
                break;
            case lex_token_type_kw_inline:
                decln->flags |= decln_flag_inline;
                break;
            case lex_token_type_kw_no_return:
                decln->flags |= decln_flag_no_return;
                break;
            case lex_token_type_kw_alignas:
                mp_cc_lex_next(parser->lexer);
                consume_next = false;
                decln->alignas_expr = parse_paren_expr(parser);
                break;
            default:
                if (has_type_spec)
                    return;
                has_type_spec = try_parse_type_spec(parser, &decln->type_spec);
                if (!has_type_spec)
                {} //TODO: emit error, unexpected token, expected type spec
                consume_next = false;
                break;
        }

        if (consume_next)
            mp_cc_lex_next(parser->lexer);
    }
}

static bool parse_declr(struct mp_cc_parser* parser, struct mp_cc_declr* declr)
{
    if (!mp_cc_lex_match(parser->lexer, lex_token_type_identifier, &declr->identifier))
    {
        struct mp_string str = mp_cc_lex_token_type_str(mp_cc_lex_peek(parser->lexer, 0).type);
        EMIT_ERROR_MISMATCH_TOKEN("expected declarator identifier, got %.*s", 
            (int)str.length, str.text);
        return false;
    }

    if (mp_cc_lex_match(parser->lexer, lex_token_type_equals, NULL))
        declr->init_expr = parse_expr(parser);

    return true;
}

//https://en.cppreference.com/w/c/language/declarations
static struct mp_cc_decln* parse_declaration(struct mp_cc_parser* parser)
{
    struct mp_cc_decln* decln = mp_try_alloc(sizeof(*decln));
    if (NULL == decln)
        return NULL;
    mp_memset(decln, 0, sizeof(*decln));

    parse_decln_parts(parser, decln);
    if (parser->options.log_declarations)
        LOG_TRACE("new declaration: ");

    do
    {
        struct mp_cc_declr* declr = mp_try_alloc(sizeof(*declr));
        if (NULL == declr)
            return NULL;
        mp_memset(declr, 0, sizeof(*declr));

        if (!parse_declr(parser, declr))
            return NULL;

        LIST_INSERT_HEAD(&decln->decls, declr, list);
        if (parser->options.log_declarations)
            LOG_TRACE("new declarator: ");
    } while (mp_cc_lex_match(parser->lexer, lex_token_type_comma, NULL));

    if (!mp_cc_lex_match(parser->lexer, lex_token_type_semicolon, NULL))
    {
        EMIT_ERROR_MISMATCH_TOKEN("expected semicolon at end of declaration");
        return NULL;
    }
    return decln;
}

static struct mp_cc_ast_node* parse_paren_expr(struct mp_cc_parser* parser)
{
    if (!mp_cc_lex_match(parser->lexer, lex_token_type_left_paren, NULL))
    {
        EMIT_ERROR_MISMATCH_TOKEN("expected opening parenthesis");
        return NULL;
    }

    struct mp_cc_ast_node* inner = parse_expr(parser);
    if (!mp_cc_lex_match(parser->lexer, lex_token_type_right_paren, NULL))
    {
        EMIT_ERROR_MISMATCH_TOKEN("expected closing parenthesis");
        return NULL;
    }

    return inner;
}

static struct mp_cc_ast_node* parse_integer_literal(struct mp_cc_parser* parser)
{
    struct mp_cc_lex_token value;
    if (!mp_cc_lex_match(parser->lexer, lex_token_type_integer_literal, &value))
    {
        EMIT_ERROR_MISMATCH_TOKEN("expected integer literal token.");
        return NULL;
    }

    return mp_cc_create_integer_literal(value);
}

static struct mp_cc_ast_node* parse_expr(struct mp_cc_parser* parser)
{
    struct mp_cc_lex_token peek = mp_cc_lex_peek(parser->lexer, 0);
    switch (peek.type)
    {
    case lex_token_type_left_paren:
        return parse_paren_expr(parser);
    case lex_token_type_integer_literal:
        return parse_integer_literal(parser);
    default:
        {
            struct mp_string type_name = mp_cc_lex_token_type_str(peek.type);
            EMIT_ERROR(parser->diags, GET_SOURCE_POS(parser, mp_cc_lex_peek(parser->lexer, 0).begin), 
                "unexpected token type for primary expression: %.*s", (int)type_name.length, type_name.text);
            return NULL;
        }
    };
}

struct mp_cc_parser* mp_cc_create_parser(struct mp_cc_lexer* input, struct mp_diagnostics* diags, const struct mp_cc_parser_options* opts)
{
    if (NULL == input || NULL == diags || NULL == opts)
        return NULL;

    struct mp_cc_parser* parser = mp_try_alloc(sizeof(*parser));
    if (NULL == parser)
        return NULL;

    parser->lexer = input;
    parser->options = *opts;
    parser->diags = diags;

    parse_declaration(parser);

    return parser;
}

void mp_cc_destroy_parser(struct mp_cc_parser** parser)
{
    if (NULL == parser || NULL == *parser)
        return;

    mp_free(*parser, sizeof(**parser));
    *parser = NULL;
}
