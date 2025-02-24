#ifndef GDT_H
#define GDT_H

#include "libc/include/string.h"
#include "libc/include/types.h"

#define GDT_ENTRIES 6
#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4

typedef struct {
  u16 limit_low;
  u16 base_low;
  u8 base_middle;
  u8 access;
  u8 granularity;
  u8 base_high;
} __attribute__((packed)) GDT;

typedef struct {
  u16 limit;
  u32 base;
} __attribute__((packed)) GDT_PTR;

typedef struct {
  u32 prev_tss; // Previous TSS (unused here).
  u32 esp0;     // Kernel stack pointer.
  u32 ss0;      // Kernel stack segment.
  u32 esp1;
  u32 ss1;
  u32 esp2;
  u32 ss2;
  u32 cr3;
  u32 eip;
  u32 eflags;
  u32 eax;
  u32 ecx;
  u32 edx;
  u32 ebx;
  u32 esp;
  u32 ebp;
  u32 esi;
  u32 edi;
  u32 es;
  u32 cs;
  u32 ss;
  u32 ds;
  u32 fs;
  u32 gs;
  u32 ldt_segment_selector;
  u16 trap;
  u16 iomap_base;
} __attribute__((packed)) tss_entry;

extern u8 stack_top[];
extern void load_gdt(u32 gdt_ptr);

void set_gdt_entry(int index, u32 base, u32 limit, u8 access, u8 gran);
void setup_paging();
void usermode();
void init_gdt();

#endif
