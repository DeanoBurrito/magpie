#include <core/diagnostics.h>
#include <cc/ingest.h>
#include <cc/preprocessor.h>
#include <cc/lexer.h>

static void process_source_file(struct mp_string filename)
{
    LOG_INFO("processing source file: %.*s", (int)filename.length, filename.text);

    struct mp_diagnostics* diags = mp_create_diagnostics();
    if (NULL == diags)
    {
        LOG_ERROR("failed to create diagnostics container");
        return;
    }

    struct mp_source_repo* repo = mp_create_source_repo(filename);
    if (NULL == repo)
    {
        LOG_ERROR("failed to create source repo for: %.*s", (int)filename.length, filename.text);
        return;
    }
    LOG_TRACE("created source repo for: %.*s", (int)filename.length, filename.text);

    //TODO: preprocess file
    struct mp_cc_lexer* lexer = mp_cc_create_lexer(repo, diags);
    if (NULL == lexer)
    {
        LOG_ERROR("failed to create lexer for: %.*s", (int)filename.length, filename.text);
        return;
    }
    LOG_TRACE("created lexer for: %.*s", (int)filename.length, filename.text);

    while (true)
    {
        struct mp_cc_lex_token token = mp_cc_lex_next(lexer);
        char token_buff[128];
        const size_t token_len = mp_cc_lex_print(lexer, token, (struct mp_rw_string){ .length = sizeof(token_buff), .text = token_buff }, true);
        LOG_TRACE("token: %.*s", (int)token_len, token_buff);
        mp_source_read(repo, token.begin, token_buff, token.length);

        LOG_TRACE("token: %u, \"%.*s\"", token.type, (int)token.length, token_buff);

        if (lex_token_type_eof == token.type)
            break;
    }

    mp_cc_destroy_lexer(&lexer);
    mp_destroy_source_repo(&repo);
    mp_destroy_diagnostics(&diags);
}

int main()
{
    const char filename[] = "../test.c";
    struct mp_string str = { .text = filename, .length = sizeof(filename) - 1 };
    process_source_file(str);

    return 0;
}
