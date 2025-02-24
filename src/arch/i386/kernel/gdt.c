#include "include/gdt.h"
#include "include/console.h"
#include "include/libc/include/defines.h"
#include "include/libc/include/string.h"
#include "include/libc/include/types.h"

GDT gdt[GDT_ENTRIES];
GDT_PTR gp;
tss_entry tss;
u32 page_directory[1024] __attribute__((aligned(4096)));
u32 first_page_table[1024] __attribute__((aligned(4096)));

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
  // Identity-map the first 4MB of memory.
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

void usermode() {
  asm volatile(
      "cli\n" // Disable interrupts.
      // Set up segment registers for user mode.
      "mov $0x23, %%ax\n" // User data segment (selector index 4, RPL=3: 0x23).
      "mov %%ax, %%ds\n"
      "mov %%ax, %%es\n"
      "mov %%ax, %%fs\n"
      "mov %%ax, %%gs\n"
      "mov %%esp, %%eax\n" // Save current ESP.
      "pushl $0x23\n"      // Push user SS.
      "pushl %%eax\n"      // Push user ESP.
      "pushfl\n"           // Push EFLAGS.
      "pushl $0x1B\n"      // Push user CS (selector index 3, RPL=3: 0x1B).
      "pushl $1f\n"        // Push entry point.
      "iret\n"             // Return to user mode.
      "1:\n"
      "nop\n"
      :
      :
      : "eax");
}

void init_gdt() {
  printf("initiating GDT...\n");
  gp.limit = (sizeof(gdt) * GDT_ENTRIES) - 1;
  gp.base = (u32)&gdt;

  set_gdt_entry(0, 0, 0, 0, 0);
  set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
  set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
  set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
  set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

  memset(&tss, 0, sizeof(tss));
  tss.ss0 = 0x10;
  tss.esp0 = (u32)stack_top;
  tss.iomap_base = sizeof(tss);
  set_gdt_entry(5, (u32)&tss, sizeof(tss), 0x89, 0x00);

  load_gdt((u32)&gp);
  asm volatile("ltr %0" : : "r"((u16)0x28));
  setup_paging();
}
