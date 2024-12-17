#pragma once

#define MP_NORETURN_HINT __attribute__((noreturn))
#define MP_PRINTF_FUNC_HINT(str, arg0) __attribute__((format(printf, str, arg0)))
#define MP_STATIC_ASSERT(cond) _Static_assert(cond, #cond)
