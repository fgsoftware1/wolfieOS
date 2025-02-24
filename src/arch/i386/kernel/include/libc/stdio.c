#include "include/stdio.h"

int vprintf(const char* __restrict format, va_list args)
{
    int ret = vfprintf(stdout, format, args);
    fflush(stdout);
    return ret;
}

// int printf(const char* __restrict format, ...)
// {
//     va_list args;
//     va_start(args, format);
//     int ret = vprintf(format, args);
//     va_end(args);
//     return ret;
// }

int putchar(int c)
{
    return putc(c, stdout);
}

int puts(const char* s)
{
    return fputs(s, stdout) + putchar('\n');
}

int snprintf(char* __restrict s, size_t n, const char* __restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(s, n, format, args);
    va_end(args);
    return ret;
}

int sprintf(char* __restrict s, const char* __restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsprintf(s, format, args);
    va_end(args);
    return ret;
}
