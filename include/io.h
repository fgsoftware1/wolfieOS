#ifndef IO_H
#define IO_H

#include "types.h"

u8 inb(u16 port);
u16 inw(u16 port);
u32 inl(u16 port);
#if defined(__x86_64__)
u64 inq(u16 port);
#endif //  defined(__x86_64__)

void outb(u16 port, u8 value);
void outw(u16 port, u16 value);
void outl(u16 port, u32 value);
#if defined(__x86_64__)
void outq(u16 port, u64 value);
#endif //  defined(__x86_64__)

#endif // !IO_H
