#include "include/stdio.h"
#include "include/stdlib.h"

__attribute__((__noreturn__)) inline void abort(void)
{
#if defined(__is_libk)

    printf("kernel: panic: abort()\n");
#else

    printf("abort()\n");
#endif
    while (1)
    {
    }
    __builtin_unreachable();
}
