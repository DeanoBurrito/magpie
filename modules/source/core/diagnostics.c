#include <core/diagnostics.h>
#include <core/alloc.h>
#include <core/nanoprintf.h>
#include <host/api.h>
#include <stdarg.h>

static struct mp_diagnostic* mp_create_diagnostic(const char* text_buff, size_t text_len, struct mp_source_pos pos)
{
    struct mp_diagnostic* diag = mp_try_alloc(sizeof(struct mp_diagnostic));
    if (NULL == diag)
        return NULL;

    diag->source_pos = pos;
    diag->text.text = text_buff;
    diag->text.length = text_len;

    return diag;
}

static void mp_destroy_diagnostic(struct mp_diagnostic** diagnostic)
{
    if (NULL == diagnostic || NULL == *diagnostic)
        return;

    mp_free((**diagnostic).text.text, (**diagnostic).text.length + 1);

    mp_free(*diagnostic, sizeof(**diagnostic));
    *diagnostic = NULL;
}

struct mp_diagnostics* mp_create_diagnostics()
{
    struct mp_diagnostics* diags = mp_try_alloc(sizeof(struct mp_diagnostics));
    if (NULL == diags)
        return NULL;

    mp_memset(diags, 0, sizeof(struct mp_diagnostics));
    return diags;
}

void mp_destroy_diagnostics(struct mp_diagnostics** diags)
{
    if (NULL == diags || NULL == *diags)
        return;

    while (!LIST_EMPTY(&(**diags).errors))
    {
        struct mp_diagnostic* head = LIST_FIRST(&(**diags).errors);
        LIST_REMOVE_HEAD(&(**diags).errors, list);
        mp_destroy_diagnostic(&head);
    }

    while (!LIST_EMPTY(&(**diags).warnings))
    {
        struct mp_diagnostic* head = LIST_FIRST(&(**diags).warnings);
        LIST_REMOVE_HEAD(&(**diags).warnings, list);
        mp_destroy_diagnostic(&head);
    }

    while (!LIST_EMPTY(&(**diags).info))
    {
        struct mp_diagnostic* head = LIST_FIRST(&(**diags).info);
        LIST_REMOVE_HEAD(&(**diags).info, list);
        mp_destroy_diagnostic(&head);
    }

    mp_free(*diags, sizeof(**diags));
    *diags = NULL;
}

void mp_write_diagnostic(struct mp_diag_list* list, struct mp_source_pos pos, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    const size_t buff_len = npf_vsnprintf(NULL, 0, format, args);
    va_end(args);

    char* buff = mp_try_alloc(buff_len + 1);
    if (NULL == buff)
        return;
    
    va_start(args, format);
    npf_vsnprintf(buff, buff_len + 1, format, args);
    va_end(args);

    struct mp_diagnostic* diag = mp_create_diagnostic(buff, buff_len, pos);
    if (NULL == diag)
    {
        mp_free(buff, buff_len + 1);
        return;
    }

    LIST_INSERT_HEAD(list, diag, list);
}

#define MP_STACK_LOG_LENGTH 1024

void mp_write_log(enum mp_log_level level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    const size_t buff_len = npf_vsnprintf(NULL, 0, format, args);
    va_end(args);

    char stack_buffer[MP_STACK_LOG_LENGTH];
    char* buff = stack_buffer;
    if (buff_len + 1 > MP_STACK_LOG_LENGTH)
        buff = mp_try_alloc(buff_len + 1);

    if (NULL == buff)
    {
        const char reason[] = "mp_write_log() failed to allocate for log buffer, when buffer did not fit on stack.";
        mp_host_panic(reason, sizeof(reason));
    }

    va_start(args, format);
    npf_vsnprintf(buff, buff_len + 1, format, args);
    va_end(args);

    if (mp_log_level_fatal == level)
        mp_host_panic(buff, buff_len);
    mp_host_log(buff, buff_len);

    if (buff != stack_buffer)
        mp_free(buff, buff_len + 1);
}
