#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include "types.h"
#include "stdarg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EOF (-1)

typedef struct _FILE FILE;

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define _F_RDONLY 1
#define _F_WRONLY 2
#define _F_RDWR 3

#define _F_SEEK_CUR 1
#define _F_SEEK_END 2
#define _F_SEEK_SET 3

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int vprintf(const char* __restrict format, va_list args);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int snprintf(char* __restrict, size_t, const char* __restrict, ...);
int sprintf(char* __restrict, const char* __restrict, ...);

#ifdef __cplusplus
}
#endif

#endif // LIBC_STDIO_H
