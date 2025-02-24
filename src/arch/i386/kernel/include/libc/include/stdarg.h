#ifndef LIBC_STDARG_H
#define LIBC_STDARG_H

#if defined(__x86_64__) || defined(__aarch64__)
typedef unsigned long va_list;
#else
typedef char* va_list;
#endif

#endif // LIBC_STDARG_H
