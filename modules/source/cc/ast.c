#include <cc/ast.h>
#include <core/alloc.h>

MP_STATIC_ASSERT(offsetof(struct mp_cc_ast_node_unary_expr, header) == 0);
MP_STATIC_ASSERT(offsetof(struct mp_cc_ast_node_binary_expr, header) == 0);
MP_STATIC_ASSERT(offsetof(struct mp_cc_ast_node_assign_expr, header) == 0);
MP_STATIC_ASSERT(offsetof(struct mp_cc_ast_node_call_expr, header) == 0);

struct ast_node_metadata
{
    const char* name;
    size_t node_size;
    size_t (*fptr_children)(struct mp_cc_ast_node*, struct mp_cc_ast_node**);
};

static size_t fptr_children_unary_expr(struct mp_cc_ast_node* node, struct mp_cc_ast_node** out)
{}

static size_t fptr_children_binary_expr(struct mp_cc_ast_node* node, struct mp_cc_ast_node** out)
{}

static size_t fptr_children_assign_expr(struct mp_cc_ast_node* node, struct mp_cc_ast_node** out)
{}

static size_t fptr_children_call_expr(struct mp_cc_ast_node* node, struct mp_cc_ast_node** out)
{}

static struct ast_node_metadata node_meta[] =
{
    [ast_node_type_integer_literal] = 
    { 
        .name = "integer_literal", 
        .fptr_children = NULL 
    },
    [ast_node_type_unary_expr] = 
    { 
        .name = "unary_expr", 
        .node_size = sizeof(struct mp_cc_ast_node_unary_expr),
        .fptr_children = fptr_children_unary_expr,
    },
    [ast_node_type_binary_expr] =
    {
        .name = "binary_expr",
        .node_size = sizeof(struct mp_cc_ast_node_binary_expr),
        .fptr_children = fptr_children_binary_expr,
    },
    [ast_node_type_assign_expr] =
    {
        .name = "assign_expr",
        .node_size = sizeof(struct mp_cc_ast_node_assign_expr),
        .fptr_children = fptr_children_assign_expr,
    },
    [ast_node_type_call_expr] =
    {
        .name = "call_expr",
        .node_size = sizeof(struct mp_cc_ast_node_call_expr),
        .fptr_children = fptr_children_call_expr,
    },
};

MP_STATIC_ASSERT(sizeof(node_meta) / sizeof(struct ast_node_metadata) == ast_node_type_count);

struct mp_cc_ast* mp_cc_create_ast()
{
    struct mp_cc_ast* ast = mp_try_alloc(sizeof(*ast));
    if (NULL == ast)
        return NULL;

    ast->root = NULL;

    return ast;
}

void mp_cc_destroy_ast(struct mp_cc_ast** ast)
{
    if (NULL == ast || NULL == *ast)
        return;

    mp_free(*ast, sizeof(**ast));
    *ast = NULL;
}

struct mp_cc_ast_node* mp_cc_create_ast_node(enum ast_node_type type)
{
    const size_t alloc_size = mp_max(sizeof(struct mp_cc_ast_node), node_meta[type].node_size);
    struct mp_cc_ast_node* node = mp_try_alloc(alloc_size);
    if (NULL == node)
        return NULL;

    mp_memset(node, 0, node_meta[type].node_size);
    node->type = type;
    node->children = node_meta[type].fptr_children;

    return node;
}

void mp_cc_destroy_ast_node(struct mp_cc_ast_node** node)
{
    if (NULL == node || NULL == *node)
        return;

    //TODO: free child nodes
    const size_t free_size = mp_max(sizeof(struct mp_cc_ast_node), node_meta[(**node).type].node_size);
    mp_free(*node, free_size);
    *node = NULL;
}

struct mp_cc_ast_node* mp_cc_create_integer_literal(struct mp_cc_lex_token value)
{
    struct mp_cc_ast_node_integer_literal* expr = (struct mp_cc_ast_node_integer_literal*)mp_cc_create_ast_node(ast_node_type_integer_literal);
    if (NULL == expr)
        return NULL;

    expr->value = value;
    return (struct mp_cc_ast_node*)expr;
}

struct mp_cc_ast_node* mp_cc_create_unary_expr(struct mp_cc_lex_token op_token, struct mp_cc_ast_node* operand)
{
    if (NULL == operand)
        return NULL;

    struct mp_cc_ast_node_unary_expr* expr = (struct mp_cc_ast_node_unary_expr*)mp_cc_create_ast_node(ast_node_type_unary_expr);
    if (NULL == expr)
        return NULL;

    expr->op = op_token;
    expr->operand = operand;

    return (struct mp_cc_ast_node*)expr;
}

struct mp_cc_ast_node* mp_cc_create_binary_expr(struct mp_cc_ast_node* lhs, struct mp_cc_lex_token op_token, struct mp_cc_ast_node* rhs)
{
    if (NULL == lhs || NULL == rhs)
        return NULL;

    struct mp_cc_ast_node_binary_expr* expr = (struct mp_cc_ast_node_binary_expr*)mp_cc_create_ast_node(ast_node_type_binary_expr);
    if (NULL == expr)
        return NULL;

    expr->lhs = lhs;
    expr->op = op_token;
    expr->rhs = rhs;

    return (struct mp_cc_ast_node*)expr;
}
