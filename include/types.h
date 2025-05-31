#ifndef TYPES_H
#define TYPES_H

#define NULL 0

typedef enum { false, true } bool;

typedef unsigned char u8;  // 8-bit unsigned integer
typedef unsigned short u16; // 16-bit unsigned integer
typedef unsigned int u32;  // 32-bit unsigned integer
typedef unsigned long u64; // 64-bit unsigned integer

typedef unsigned char byte;  // Alias for 8-bit unsigned integer
typedef unsigned short word; // Alias for 16-bit unsigned integer
typedef unsigned int dword;  // Alias for 32-bit unsigned integer
typedef unsigned long qword; // Alias for 64-bit unsigned integer

#if defined(__x86_64__) || defined(_M_X64)
    typedef u64 uintptr_t;
#elif defined(__i386__) || defined(_M_IX86)
    typedef u32 uintptr_t;
#else
    #error "Unsupported architecture"
#endif

#endif // !TYPES_H
