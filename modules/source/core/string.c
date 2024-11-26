#include <core/string.h>

bool mp_isspace(char c)
{
    return (c >= 0x9 && c <= 0xD) || (c == 0x20);
}

bool mp_isprintable(char c)
{
    return c >= 32 && c <= 127;
}

bool mp_isdigit(char c)
{
    return c >= '0' && c <= '9';
}

bool mp_isalpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
