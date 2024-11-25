#pragma once

#include <core/queue.h>
#include <core/source.h>
#include <host/compiler.h>

struct mp_diagnostic
{
    LIST_ENTRY(mp_diagnostic) list;
    struct mp_string text;
    struct mp_source_pos source_pos;
};

struct mp_diagnostics
{
    LIST_HEAD(mp_diag_list, mp_diagnostic) errors;
    LIST_HEAD(, mp_diagnostic) warnings;
    LIST_HEAD(, mp_diagnostic) info;
};

struct mp_diagnostics* mp_create_diagnostics();
void mp_destroy_diagnostics(struct mp_diagnostics** diags);

void mp_write_diagnostic(struct mp_diag_list* list, struct mp_source_pos pos, const char* format, ...);

#define EMIT_ERROR(diags, pos, msg, ...) mp_write_diagnostic(&(diags)->errors, pos, msg, ##__VA_ARGS__)
#define EMIT_WARNING(diags, pos, msg, ...) mp_write_diagnostic(&diags)->warnings, pos, msg, ##__VA_ARGS__)
#define EMIT_INFO(diags, pos, msg, ...) mp_write_diagnostic(&(diags)->info, pos, msg, ##__VA_ARGS__)

enum mp_log_level
{
    mp_log_level_fatal,
    mp_log_level_error,
    mp_log_level_warning,
    mp_log_level_info,
    mp_log_level_trace,
};

MP_PRINTF_FUNC_HINT(2, 3)
void mp_write_log(enum mp_log_level level, const char* format, ...);

#define LOG_HEADER "(%s:%u): "
#define LOG_FATAL(why, ...) mp_write_log(mp_log_level_fatal, LOG_HEADER why, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) mp_write_log(mp_log_level_error, LOG_HEADER msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) mp_write_log(mp_log_level_warning, LOG_HEADER msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) mp_write_log(mp_log_level_info, LOG_HEADER msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_TRACE(msg, ...) mp_write_log(mp_logLevel_trace, LOG_HEADER msg, __FILE__, __LINE__, ##__VA_ARGS__)
