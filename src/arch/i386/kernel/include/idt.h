#ifndef IDT_H
#define IDT_H

#include "libc/include/types.h"
#include "libc/include/string.h"

#define NO_IDT_DESCRIPTORS     256

extern void isr_syscall();

typedef u32 (*syscall_t)(u32, u32, u32, u32);

typedef struct {
    u16 base_low;
    u16 segment_selector;
    u8 zero;
    u8 type;
    u16 base_high;
} __attribute__((packed)) IDT;

typedef struct {
    u16 limit;
    u32 base_address;
} __attribute__((packed)) IDT_PTR;

extern void load_idt(u32 idt_ptr);

void idt_set_entry(int index, u32 base, u16 seg_sel, u8 flags);

void init_idt();
void syscall_handler(u32 esp);
u32 syscall(u32 num, u32 arg1, u32 arg2, u32 arg3);

#endif
