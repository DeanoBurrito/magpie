#pragma once

#include <cc/lexer.h>
#include <core/queue.h>

struct mp_cc_ast_node;

struct mp_cc_type_spec
{
};

struct mp_cc_declr
{
    LIST_ENTRY(mp_cc_declr) list;
    struct mp_cc_lex_token identifier;
    size_t array_size;
    struct mp_cc_ast_node* init_expr;
};

enum decln_flags
{
    decln_flag_none = 0,
    decln_flag_typedef = 1 << 0,
    decln_flag_constexpr = 1 << 1,
    decln_flag_auto = 1 << 2,
    decln_flag_register = 1 << 3,
    decln_flag_static = 1 << 4,
    decln_flag_extern = 1 << 5,
    decln_flag_thread_local = 1 << 6,
    decln_flag_const = 1 << 7,
    decln_flag_volatile = 1 << 8,
    decln_flag_restrict = 1 << 9,
    decln_flag_atomic = 1 << 10,
    decln_flag_inline = 1 << 11,
    decln_flag_no_return = 1 << 12,
};

struct mp_cc_decln
{
    enum decln_flags flags;
    struct mp_cc_ast_node* alignas_expr;
    struct mp_cc_type_spec type_spec;

    LIST_HEAD(, mp_cc_declr) decls;
};

enum ast_node_type
{
    ast_node_type_integer_literal,
    ast_node_type_unary_expr,
    ast_node_type_binary_expr,
    ast_node_type_assign_expr,
    ast_node_type_call_expr,

    //ast_node_type_expr_statement,
    //ast_node_type_block_statement,
    //ast_node_type_if_statement,
    //ast_node_type_loop_statement,
    //ast_node_type_return_statement,

    ast_node_type_count
};

struct mp_cc_ast_node
{
    enum ast_node_type type;

    size_t (*children)(struct mp_cc_ast_node* node, struct mp_cc_ast_node** out);
};

struct mp_cc_ast_node_integer_literal
{
    struct mp_cc_ast_node header;
    struct mp_cc_lex_token value;
};

struct mp_cc_ast_node_unary_expr
{
    struct mp_cc_ast_node header;
    struct mp_cc_lex_token op;
    struct mp_cc_ast_node* operand;
};

struct mp_cc_ast_node_binary_expr
{
    struct mp_cc_ast_node header;
    struct mp_cc_lex_token op;
    struct mp_cc_ast_node* lhs;
    struct mp_cc_ast_node* rhs;
};

struct mp_cc_ast_node_assign_expr
{
    struct mp_cc_ast_node header;
    struct mp_cc_lex_token identifier;
    struct mp_cc_ast_node from_expr;
};

struct mp_cc_ast_node_call_expr
{
    struct mp_cc_ast_node header;
    struct mp_cc_lex_token identifier;
};

struct mp_cc_ast_node_expr_statement
{
    struct mp_cc_ast_node header;
    struct mp_cc_ast_node* expression;
};

struct mp_cc_ast
{
    struct mp_cc_ast_node* root;
};

struct mp_cc_ast* mp_cc_create_ast();
void mp_cc_destroy_ast(struct mp_cc_ast** ast);

struct mp_cc_ast_node* mp_cc_create_ast_node(enum ast_node_type type);
void mp_cc_destroy_ast_node(struct mp_cc_ast_node** node);
struct mp_cc_ast_node* mp_cc_create_integer_literal(struct mp_cc_lex_token value);
struct mp_cc_ast_node* mp_cc_create_unary_expr(struct mp_cc_lex_token op_token, struct mp_cc_ast_node* operand);
struct mp_cc_ast_node* mp_cc_create_binary_expr(struct mp_cc_ast_node* lhs, struct mp_cc_lex_token op_token, struct mp_cc_ast_node* rhs);
