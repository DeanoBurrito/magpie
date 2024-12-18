#pragma once

#include <core/source.h>
#include <core/diagnostics.h>

enum lex_token_type
{
    lex_token_type_eof,

    lex_token_type_left_paren,
    lex_token_type_right_paren,
    lex_token_type_left_brace,
    lex_token_type_right_brace,
    lex_token_type_left_bracket,
    lex_token_type_right_bracket,
    lex_token_type_comma,
    lex_token_type_dot,
    lex_token_type_arrow,
    lex_token_type_minus,
    lex_token_type_minus_minus,
    lex_token_type_minus_equals,
    lex_token_type_plus,
    lex_token_type_plus_plus,
    lex_token_type_plus_equals,
    lex_token_type_slash,
    lex_token_type_star,
    lex_token_type_semicolon,
    lex_token_type_colon,
    lex_token_type_less,
    lex_token_type_left_shift,
    lex_token_type_less_equal,
    lex_token_type_left_shift_equals,
    lex_token_type_greater,
    lex_token_type_right_shift,
    lex_token_type_greater_equal,
    lex_token_type_right_shift_equals,
    lex_token_type_equals,
    lex_token_type_equals_equals,
    lex_token_type_bang,
    lex_token_type_bang_equals,
    lex_token_type_tilde,
    lex_token_type_ampersand,
    lex_token_type_ampersand_ampersand,
    lex_token_type_ampersand_equals,
    lex_token_type_percent,
    lex_token_type_percent_equals,
    lex_token_type_question_mark,
    lex_token_type_pipe,
    lex_token_type_pipe_pipe,
    lex_token_type_pipe_equals,

    lex_token_type_integer_literal,
    lex_token_type_identifier,

    lex_token_type_kw_struct,
    lex_token_type_kw_union,
    lex_token_type_kw_enum,
    lex_token_type_kw_return,
    lex_token_type_kw_constexpr,
    lex_token_type_kw_auto,
    lex_token_type_kw_static,
    lex_token_type_kw_extern,
    lex_token_type_kw_thread_local,
    lex_token_type_kw_inline,
    lex_token_type_kw_no_return,
    lex_token_type_kw_alignas,
    lex_token_type_kw_const,
    lex_token_type_kw_volatile,
    lex_token_type_kw_restrict,
    lex_token_type_kw_atomic,

    lex_token_type_count
};

struct mp_cc_lex_token
{
    enum lex_token_type type;
    size_t begin;
    size_t length;
};

struct mp_cc_lex_options
{
};

#define MP_CC_DEFAULT_LEXER_OPTIONS \
    (struct mp_cc_lex_options) { }

struct mp_cc_lexer;

struct mp_cc_lexer* mp_cc_create_lexer(struct mp_source_repo* source, struct mp_diagnostics* diags, const struct mp_cc_lex_options* opts);
void mp_cc_destroy_lexer(struct mp_cc_lexer** lexer);

struct mp_cc_lex_token mp_cc_lex_next(struct mp_cc_lexer* lexer);
struct mp_cc_lex_token mp_cc_lex_peek(struct mp_cc_lexer* lexer, size_t offset);
bool mp_cc_lex_match(struct mp_cc_lexer* lexer, enum lex_token_type expected, struct mp_cc_lex_token* token);
struct mp_string mp_cc_lex_token_type_str(enum lex_token_type type);
size_t mp_cc_lex_print(struct mp_cc_lexer* lexer, struct mp_cc_lex_token token, struct mp_rw_string str, bool metadata);
struct mp_source_repo* mp_cc_lex_get_source(struct mp_cc_lexer* lexer);
