#include <cc/lexer.h>
#include <core/alloc.h>
#include <core/nanoprintf.h>

struct mp_cc_lexer
{
    struct mp_source_repo* repo;
    struct mp_cc_lex_options options;
    struct mp_diagnostics* diags;
    size_t scan_index;
};

struct mp_cc_lexer* mp_cc_create_lexer(struct mp_source_repo* source, struct mp_diagnostics* diags, const struct mp_cc_lex_options* opts)
{
    if (NULL == source || NULL == diags || NULL == opts)
        return NULL;

    struct mp_cc_lexer* lexer = mp_try_alloc(sizeof(*lexer));
    if (NULL == lexer)
        return NULL;

    lexer->options = *opts;
    lexer->repo = source;
    lexer->scan_index = 0;
    lexer->diags = diags;

    return lexer;
}

void mp_cc_destroy_lexer(struct mp_cc_lexer** lexer)
{
    if (NULL == lexer || NULL == *lexer)
        return;

    mp_free(*lexer, sizeof(**lexer));
    *lexer = NULL;
}

static struct mp_cc_lex_token lex_number(struct mp_cc_lexer* lexer, int scan)
{
    if (!mp_isdigit(scan))
        return (struct mp_cc_lex_token){ .type = lex_token_type_eof };

    struct mp_cc_lex_token token = { .type = lex_token_type_integer_literal, .begin = lexer->scan_index - 1 };

    const int peek = mp_source_peek(lexer->repo, lexer->scan_index);
    const int peek2 = mp_source_peek(lexer->repo, lexer->scan_index + 1);
    if (('x' == peek || 'X' == peek || 'b' == peek || 'B' == peek) && mp_isdigit(peek2))
        lexer->scan_index++; //consume non-digit

    while (mp_isdigit(mp_source_peek(lexer->repo, lexer->scan_index)))
        lexer->scan_index++;

    token.length = (lexer->scan_index - 1) - token.begin;
    return token;
}

static struct mp_cc_lex_token lex_identifier(struct mp_cc_lexer* lexer, int scan)
{
    if (scan != '_' && !mp_isalpha(scan))
        return (struct mp_cc_lex_token){ .type = lex_token_type_eof };

    struct mp_cc_lex_token token = { .type = lex_token_type_identifier, .begin = lexer->scan_index - 1 };
    while (true)
    {
        scan = mp_source_peek(lexer->repo, lexer->scan_index);
        if (scan != '_' && !mp_isdigit(scan) && !mp_isalpha(scan))
            break;
        lexer->scan_index++;
    }
    
    token.length = lexer->scan_index - token.begin;
    return token;
}

struct mp_cc_lex_token mp_cc_lex_next(struct mp_cc_lexer* lexer)
{
    struct mp_cc_lex_token token = { .type = lex_token_type_eof };

    if (NULL == lexer)
        return token;

    while (mp_isspace(mp_source_peek(lexer->repo, lexer->scan_index)))
        lexer->scan_index++;

    token.begin = lexer->scan_index;
    token.length = 1;

    const int scan = mp_source_peek(lexer->repo, lexer->scan_index++);
    const int peek = mp_source_peek(lexer->repo, lexer->scan_index);
    switch (scan)
    {
    case MP_SOURCE_PEEK_EOF:
        return token;
    
    case '(':
        token.type = lex_token_type_left_paren;
        return token;
    case ')':
        token.type = lex_token_type_right_paren;
        return token;
    case '{':
        token.type = lex_token_type_left_brace;
        return token;
    case '}':
        token.type = lex_token_type_right_brace;
        return token;
    case '[':
        token.type = lex_token_type_left_bracket;
        return token;
    case ']':
        token.type = lex_token_type_right_bracket;
        return token;
    case ',':
        token.type = lex_token_type_comma;
        return token;
    case '.':
        token.type = lex_token_type_dot;
        return token;
    case '-':
        token.type = lex_token_type_minus;
        if ('-' == peek)
            token.type = lex_token_type_minus_minus;
        else if ('>' == peek)
            token.type = lex_token_type_arrow;
        else if ('=' == peek)
            token.type = lex_token_type_minus_equals;

        if (lex_token_type_minus != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '+':
        token.type = lex_token_type_plus;
        if ('+' == peek)
            token.type = lex_token_type_plus_plus;
        else if ('=' == peek)
            token.type = lex_token_type_plus_equals;

        if (lex_token_type_plus != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '/':
        token.type = lex_token_type_slash;
        return token;
    case '*':
        token.type = lex_token_type_star;
        return token;
    case ';':
        token.type = lex_token_type_semicolon;
        return token;
    case ':':
        token.type = lex_token_type_colon;
        return token;
    case '<':
        token.type = lex_token_type_less;
        if ('<' == peek)
            token.type = lex_token_type_left_shift;
        else if ('=' == peek)
            token.type = lex_token_type_less_equal;

        if (lex_token_type_less != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '>':
        token.type = lex_token_type_greater;
        if ('>' == peek)
            token.type = lex_token_type_right_shift;
        else if ('=' == peek)
            token.type = lex_token_type_greater_equal;

        if (lex_token_type_greater != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '=':
        token.type = lex_token_type_equals;
        if ('=' == peek)
            token.type = lex_token_type_equals_equals;

        if (lex_token_type_equals != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '!':
        token.type = lex_token_type_bang;
        if ('=' == peek)
            token.type = lex_token_type_bang_equals;

        if (lex_token_type_bang != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '~':
        token.type = lex_token_type_tilde;
        return token;
    case '&':
        token.type = lex_token_type_ampersand;
        if ('&' == peek)
            token.type = lex_token_type_ampersand_ampersand;
        else if ('=' == peek)
            token.type = lex_token_type_ampersand_equals;

        if (lex_token_type_ampersand != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '%':
        token.type = lex_token_type_percent;
        if ('=' == peek)
            token.type = lex_token_type_percent_equals;

        if (lex_token_type_percent != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;
    case '?':
        token.type = lex_token_type_question_mark;
        return token;
    case '|':
        token.type = lex_token_type_pipe;
        if ('|' == peek)
            token.type = lex_token_type_pipe;
        else if ('=' == peek)
            token.type = lex_token_type_pipe_equals;

        if (lex_token_type_pipe != token.type)
        {
            lexer->scan_index++;
            token.length++;
        }
        return token;

    default:
        if (mp_isdigit(scan))
            return lex_number(lexer, scan);
        return lex_identifier(lexer, scan);
    }
}

struct mp_cc_lex_token mp_cc_lex_peek(struct mp_cc_lexer* lexer, size_t offset)
{
    if (NULL == lexer)
        return (struct mp_cc_lex_token){ .type = lex_token_type_eof };

    //TODO: if we parse future tokens, we should store them so we dont have to re-parse them later
    const size_t backup_scan_head = lexer->scan_index;

    struct mp_cc_lex_token token = mp_cc_lex_next(lexer);
    for (size_t i = 0; i < offset && lex_token_type_eof != token.type; i++)
        token = mp_cc_lex_next(lexer);

    lexer->scan_index = backup_scan_head;
    return token;
}

bool mp_cc_lex_match(struct mp_cc_lexer* lexer, enum lex_token_type expected, struct mp_cc_lex_token* token)
{
    if (NULL == lexer)
        return false;

    struct mp_cc_lex_token next = mp_cc_lex_peek(lexer, 0);
    if (expected != next.type)
        return false;
    
    *token = next;
    return true;
}

struct mp_string mp_cc_lex_token_type_str(enum lex_token_type type)
{
    return (struct mp_string){ .text = "TODO:", .length = 5 };
}

size_t mp_cc_lex_print(struct mp_cc_lexer* lexer, struct mp_cc_lex_token token, struct mp_rw_string str, bool metadata)
{
    if (NULL == lexer)
        return 0;

    size_t format_len = 0;
    if (metadata)
    {
        format_len = npf_snprintf(str.text, str.length, "[%s %zu,%zu (%zu)]", 
            mp_cc_lex_token_type_str(token.type).text, token.begin, token.begin + token.length, 
            token.length);
    }

    if (str.length != 0)
        mp_source_read(lexer->repo, token.begin, str.text + format_len, mp_min(str.length - format_len, token.length));
    return format_len + token.length;
}
