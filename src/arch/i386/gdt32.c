#include "gdt.h"
#include "io.h"

GDTEntry gdt[GDT_ENTRIES];
u32 page_directory[1024] __attribute__((aligned(4096)));
u32 first_page_table[1024] __attribute__((aligned(4096)));

void gdt_init()
{
    GDTPointer gdt_pointer;
    gdt_pointer.limit = (sizeof(gdt) * GDT_ENTRIES) - 1;
    gdt_pointer.base = (u32)&gdt;

    set_gdt_entry(0, 0, 0, 0, 0);
    set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    load_gdt((u32)&gdt_pointer);
    setup_paging();
}

void set_gdt_entry(int num, u32 base, u32 limit, u8 access, u8 gran) {
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

  gdt[num].limit_low = (limit & 0xFFFF);
  gdt[num].granularity = ((limit >> 16) & 0x0F);
  gdt[num].granularity |= (gran & 0xF0);
  gdt[num].access = access;
}

void setup_paging() {
  int i;

  for (i = 0; i < 1024; i++) {
    first_page_table[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_RW | PAGE_USER;
  }

  page_directory[0] =
      ((u32)first_page_table) | PAGE_PRESENT | PAGE_RW | PAGE_USER;

  for (i = 1; i < 1024; i++) {
    page_directory[i] = 0;
  }

  asm volatile("mov %0, %%cr3" : : "r"(page_directory));

  u32 cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000;
  asm volatile("mov %0, %%cr0" : : "r"(cr0));
}
