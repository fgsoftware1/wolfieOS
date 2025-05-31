#include "io.h"

u8 inb(u16 port)
{
	u8 value;
	asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

u16 inw(u16 port)
{
	u16 value;
	asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

u32 inl(u16 port)
{
	u32 value;
	asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

#if defined(__x86_64__)
u64 inq(u16 port)
{
	u64 value;
	asm volatile("inq %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}
#endif //  defined(__x86_64__)

void outb(u16 port, u8 value)
{
	asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void outw(u16 port, u16 value)
{
	asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

void outl(u16 port, u32 value)
{
	asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

#if defined(__x86_64__)
void outq(u16 port, u64 value)
{
	asm volatile("outq %0, %1" : : "a"(value), "Nd"(port));
}
#endif //  defined(__x86_64__)
