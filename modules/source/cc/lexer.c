#include <cc/lexer.h>
#include <core/alloc.h>
#include <core/nanoprintf.h>

const char* token_type_strs[] = 
{
    [lex_token_type_eof] = "<eof>",
    [lex_token_type_left_paren] = "left_paren",
    [lex_token_type_right_paren] = "right_paren",
    [lex_token_type_left_brace] = "left_brace",
    [lex_token_type_right_brace] = "right_brace",
    [lex_token_type_left_bracket] = "left_bracket",
    [lex_token_type_right_bracket] = "right_bracket",
    [lex_token_type_comma] = "comma",
    [lex_token_type_dot] = "dot",
    [lex_token_type_arrow] = "arrow",
    [lex_token_type_minus] = "minus",
    [lex_token_type_minus_minus] = "minus_minus",
    [lex_token_type_minus_equals] = "minus_equals",
    [lex_token_type_plus] = "plus",
    [lex_token_type_plus_plus] = "plus_plus",
    [lex_token_type_plus_equals] = "plus_equals",
    [lex_token_type_slash] = "slash",
    [lex_token_type_star] = "star",
    [lex_token_type_semicolon] = "semicolon",
    [lex_token_type_colon] = "colon",
    [lex_token_type_less] = "less",
    [lex_token_type_left_shift] = "left_shift",
    [lex_token_type_less_equal] = "less_equal",
    [lex_token_type_left_shift_equals] = "left_shift_equals",
    [lex_token_type_greater] = "greater",
    [lex_token_type_right_shift] = "right_shift",
    [lex_token_type_greater_equal] = "greater_equal",
    [lex_token_type_right_shift_equals] = "right_shift_equals",
    [lex_token_type_equals] = "equals",
    [lex_token_type_equals_equals] = "equals_equals",
    [lex_token_type_bang] = "bang",
    [lex_token_type_bang_equals] = "bang_equals",
    [lex_token_type_tilde] = "tilde",
    [lex_token_type_ampersand] = "ampersand",
    [lex_token_type_ampersand_ampersand] = "ampersand_ampersand",
    [lex_token_type_ampersand_equals] = "ampersand_equals",
    [lex_token_type_percent] = "percent",
    [lex_token_type_percent_equals] = "percent_equals",
    [lex_token_type_question_mark] = "question_mark",
    [lex_token_type_pipe] = "pipe",
    [lex_token_type_pipe_pipe] = "pipe_pipe",
    [lex_token_type_pipe_equals] = "pipe_equals",
    [lex_token_type_integer_literal] = "integer_literal",
    [lex_token_type_identifier] = "identifier",
    [lex_token_type_kw_struct] = "keyword_struct",
    [lex_token_type_kw_union] = "keyword_union",
    [lex_token_type_kw_return] = "keyword_return",
    [lex_token_type_kw_constexpr] = "keyword_constexpr",
    [lex_token_type_kw_auto] = "keyword_auto",
    [lex_token_type_kw_static] = "keyword_static",
    [lex_token_type_kw_extern] = "keyword_extern",
    [lex_token_type_kw_thread_local] = "keword_",
    [lex_token_type_kw_inline] = "keword_inline",
    [lex_token_type_kw_no_return] = "keword_no_return",
    [lex_token_type_kw_alignas] = "keword_alignas",
    [lex_token_type_kw_const] = "keword_const",
    [lex_token_type_kw_volatile] = "keword_volatile",
    [lex_token_type_kw_restrict] = "keword_restrict",
    [lex_token_type_kw_atomic] = "keword_atomic",
};

#define MAX_KEYWORD_LENGTH 16
const char* keyword_literals[] =
{
    [lex_token_type_kw_struct] = "struct",
    [lex_token_type_kw_union] = "union",
    [lex_token_type_kw_enum] = "enum",
    [lex_token_type_kw_return] = "return",
    [lex_token_type_kw_constexpr] = "constexpr",
    [lex_token_type_kw_auto] = "auto",
    [lex_token_type_kw_static] = "static",
    [lex_token_type_kw_extern] = "extern",
    [lex_token_type_kw_thread_local] = "_Thread_local",
    [lex_token_type_kw_inline] = "inline",
    [lex_token_type_kw_no_return] = "_Noreturn",
    [lex_token_type_kw_alignas] = "_Alignas",
    [lex_token_type_kw_const] = "const",
    [lex_token_type_kw_volatile] = "volatile",
    [lex_token_type_kw_restrict] = "restrict",
    [lex_token_type_kw_atomic] = "atomic",
};

MP_STATIC_ASSERT(sizeof(token_type_strs) / sizeof(const char*) == lex_token_type_count);

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

static struct mp_cc_lex_token lex_identifier_or_keyword(struct mp_cc_lexer* lexer, int scan)
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

    if (token.length >= MAX_KEYWORD_LENGTH)
        return token;

    char comp_buffer[MAX_KEYWORD_LENGTH];
    mp_source_read(lexer->repo, token.begin, comp_buffer, token.length);

    //check for keyword matches
    for (size_t i = 0; i < lex_token_type_count; i++)
    {
        if (NULL == keyword_literals[i])
            continue;

        const size_t kw_len = mp_strlen(keyword_literals[i]);
        if (kw_len != token.length)
            continue;
        if (mp_memcmp(comp_buffer, keyword_literals[i], kw_len) != 0)
            continue;

        token.type = i;
        break;
    }

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
        return lex_identifier_or_keyword(lexer, scan);
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
    mp_cc_lex_next(lexer);
    
    if (NULL != token)
        *token = next;
    return true;
}

struct mp_string mp_cc_lex_token_type_str(enum lex_token_type type)
{
    void* found = mp_memchr(token_type_strs[type], 0, -1);
    if (NULL == found)
        return (struct mp_string){ .text = NULL, .length = 0 };

    return (struct mp_string){ .text = token_type_strs[type], .length = (uintptr_t)found - (uintptr_t)token_type_strs[type] };
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

struct mp_source_repo* mp_cc_lex_get_source(struct mp_cc_lexer* lexer)
{
    if (NULL == lexer)
        return NULL;
    return lexer->repo;
}
