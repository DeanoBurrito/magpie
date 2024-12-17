#include <core/diagnostics.h>
#include <cc/ingest.h>
#include <cc/preprocessor.h>
#include <cc/lexer.h>
#include <cc/parser.h>

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
    struct mp_cc_lex_options lexer_options = MP_CC_DEFAULT_LEXER_OPTIONS;
    struct mp_cc_lexer* lexer = mp_cc_create_lexer(repo, diags, &lexer_options);
    if (NULL == lexer)
    {
        LOG_ERROR("failed to create lexer for: %.*s", (int)filename.length, filename.text);
        return;
    }
    LOG_TRACE("created lexer for: %.*s", (int)filename.length, filename.text);

    if (!LIST_EMPTY(&diags->errors))
    {
        struct mp_diagnostic* error = NULL;
        LIST_FOREACH(error, &diags->errors, list)
        {
            LOG_ERROR("lexer error: %.*s", (int)error->text.length, error->text.text);
        }
        return;
    }

    struct mp_cc_parser_options parser_options = MP_CC_DEFAULT_PARSER_OPTIONS;
    struct mp_cc_parser* parser = mp_cc_create_parser(lexer, diags, &parser_options);
    if (NULL == parser)
    {
        LOG_ERROR("failed to create parser for: %.*s", (int)filename.length, filename.text);
        return;
    }
    LOG_TRACE("created parser for %.*s", (int)filename.length, filename.text);

    if (!LIST_EMPTY(&diags->errors))
    {
        struct mp_diagnostic* error = NULL;
        LIST_FOREACH(error, &diags->errors, list)
        {
            LOG_ERROR("parser error: %.*s", (int)error->text.length, error->text.text);
        }
        return;
    }

    mp_cc_destroy_parser(&parser);
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
