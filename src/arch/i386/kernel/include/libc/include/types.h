#ifndef TYPES_H
#define TYPES_H

#define NULL 0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef char wchar_t;
typedef unsigned int size_t;
typedef unsigned int uintptr_t;
typedef signed int ssize_t;
typedef char *va_list;

typedef u8 byte;
typedef u16 word;
typedef u32 dword;

typedef enum
{
    false,
    true
} bool;

typedef struct file
{
    char *ptr;
    int cnt;
    char *base;
    int flag;
    int fd;
    int bufsize;
    char *tmpfname;
} file_t;

#define va_start(ap, last) (ap = (va_list) & last + sizeof(last))
#define va_arg(ap, type) (*(type *)((ap += sizeof(type)) - sizeof(type)))
#define va_end(ap) (ap = (va_list)0)

static inline u32 refToU32(void **ptr) {
    return (u32)(uintptr_t)(*ptr);
}

#endif
