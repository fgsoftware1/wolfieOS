#include "include/io.h"
#include "./include/libc/include/defines.h"
#include "./include/libc/include/types.h"

u8 inportb(u16 port) {
  u8 ret;
  asmv("inb %1, %0" : "=a"(ret) : "dN"(port));
  return ret;
}

void outportb(u16 port, u8 data) {
  asmv("outb %1, %0" ::"dN"(port), "a"(data));
}

u16 inportw(u16 port) {
  u16 rv;
  asmv("inw %1, %0" : "=a"(rv) : "dN"(port));
  return rv;
}

void outportw(u16 port, u16 data) {
  asmv("outw %1, %0" : : "dN"(port), "a"(data));
}

u32 inportl(u16 port) {
  u32 rv;
  asmv("inl %%dx, %%eax" : "=a"(rv) : "dN"(port));
  return rv;
}

void outportl(u16 port, u32 data) {
  asmv("outl %%eax, %%dx" : : "dN"(port), "a"(data));
}
