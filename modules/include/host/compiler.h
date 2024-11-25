#pragma once

#define MP_NORETURN_HINT __attribute__((noreturn))
#define MP_PRINTF_FUNC_HINT(str, arg0) __attribute__((format(printf, str, arg0)))
