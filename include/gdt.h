#ifndef GDT_H
#define GDT_H

#include "types.h"

#define GDT_ENTRIES 6
#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4

extern void load_gdt(u32 gdt_pointer);

typedef struct
{
    u16 limit_low;  // Lower 16 bits of the limit
    u16 base_low;   // Lower 16 bits of the base address
    u8 base_middle; // Next 8 bits of the base address
    u8 access;      // Access flags
    u8 granularity; // Granularity and upper 4 bits of the limit
    u8 base_high;   // Upper 8 bits of the base address
} __attribute__((packed)) GDTEntry;

typedef struct
{
    u16 limit;  // Size of the GDT in bytes
    u32 base; // Base address of the GDT
} __attribute__((packed)) GDTPointer;

void gdt_init();
void set_gdt_entry(int num, u32 base, u32 limit, u8 access, u8 gran);
void setup_paging();

#endif // !GDT_H
