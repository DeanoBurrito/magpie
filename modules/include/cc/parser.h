#pragma once

#include <cc/lexer.h>
#include <cc/ast.h>

struct mp_cc_parser_options
{
    bool log_declarations;
};

#define MP_CC_DEFAULT_PARSER_OPTIONS \
    (struct mp_cc_parser_options) { \
        .log_declarations = true \
    }

struct mp_cc_parser;

struct mp_cc_parser* mp_cc_create_parser(struct mp_cc_lexer* input, struct mp_diagnostics* diags, const struct mp_cc_parser_options* opts);
void mp_cc_destroy_parser(struct mp_cc_parser** parser);
