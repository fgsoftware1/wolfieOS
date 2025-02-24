#include "./include/libc/include/types.h"
#include "include/idt.h"
#include "include/isr.h"
#include "include/drivers/pic.h"
#include "./include/libc/include/defines.h"

IDT g_idt[NO_IDT_DESCRIPTORS];
IDT_PTR g_idt_ptr;

void idt_set_entry(int index, u32 base, u16 seg_sel, u8 flags) {
    IDT *this = &g_idt[index];

    this->base_low = base & 0xFFFF;
    this->segment_selector = seg_sel;
    this->zero = 0;
    this->type = flags | 0x60;
    this->base_high = (base >> 16) & 0xFFFF;
}

void init_idt() {
    printf("initiating IDT...\n");
    g_idt_ptr.base_address = (u32)g_idt;
    g_idt_ptr.limit = sizeof(g_idt) - 1;

    idt_set_entry(0, (u32)exception_0, 0x08, 0x8E);
    idt_set_entry(1, (u32)exception_1, 0x08, 0x8E);
    idt_set_entry(2, (u32)exception_2, 0x08, 0x8E);
    idt_set_entry(3, (u32)exception_3, 0x08, 0x8E);
    idt_set_entry(4, (u32)exception_4, 0x08, 0x8E);
    idt_set_entry(5, (u32)exception_5, 0x08, 0x8E);
    idt_set_entry(6, (u32)exception_6, 0x08, 0x8E);
    idt_set_entry(7, (u32)exception_7, 0x08, 0x8E);
    idt_set_entry(8, (u32)exception_8, 0x08, 0x8E);
    idt_set_entry(9, (u32)exception_9, 0x08, 0x8E);
    idt_set_entry(10, (u32)exception_10, 0x08, 0x8E);
    idt_set_entry(11, (u32)exception_11, 0x08, 0x8E);
    idt_set_entry(12, (u32)exception_12, 0x08, 0x8E);
    idt_set_entry(13, (u32)exception_13, 0x08, 0x8E);
    idt_set_entry(14, (u32)exception_14, 0x08, 0x8E);
    idt_set_entry(15, (u32)exception_15, 0x08, 0x8E);
    idt_set_entry(16, (u32)exception_16, 0x08, 0x8E);
    idt_set_entry(17, (u32)exception_17, 0x08, 0x8E);
    idt_set_entry(18, (u32)exception_18, 0x08, 0x8E);
    idt_set_entry(19, (u32)exception_19, 0x08, 0x8E);
    idt_set_entry(20, (u32)exception_20, 0x08, 0x8E);
    idt_set_entry(21, (u32)exception_21, 0x08, 0x8E);
    idt_set_entry(22, (u32)exception_22, 0x08, 0x8E);
    idt_set_entry(23, (u32)exception_23, 0x08, 0x8E);
    idt_set_entry(24, (u32)exception_24, 0x08, 0x8E);
    idt_set_entry(25, (u32)exception_25, 0x08, 0x8E);
    idt_set_entry(26, (u32)exception_26, 0x08, 0x8E);
    idt_set_entry(27, (u32)exception_27, 0x08, 0x8E);
    idt_set_entry(28, (u32)exception_28, 0x08, 0x8E);
    idt_set_entry(29, (u32)exception_29, 0x08, 0x8E);
    idt_set_entry(30, (u32)exception_30, 0x08, 0x8E);
    idt_set_entry(31, (u32)exception_31, 0x08, 0x8E);
    idt_set_entry(32, (u32)irq_0, 0x08, 0x8E);
    idt_set_entry(33, (u32)irq_1, 0x08, 0x8E);
    idt_set_entry(34, (u32)irq_2, 0x08, 0x8E);
    idt_set_entry(35, (u32)irq_3, 0x08, 0x8E);
    idt_set_entry(36, (u32)irq_4, 0x08, 0x8E);
    idt_set_entry(37, (u32)irq_5, 0x08, 0x8E);
    idt_set_entry(38, (u32)irq_6, 0x08, 0x8E);
    idt_set_entry(39, (u32)irq_7, 0x08, 0x8E);
    idt_set_entry(40, (u32)irq_8, 0x08, 0x8E);
    idt_set_entry(41, (u32)irq_9, 0x08, 0x8E);
    idt_set_entry(42, (u32)irq_10, 0x08, 0x8E);
    idt_set_entry(43, (u32)irq_11, 0x08, 0x8E);
    idt_set_entry(44, (u32)irq_12, 0x08, 0x8E);
    idt_set_entry(45, (u32)irq_13, 0x08, 0x8E);
    idt_set_entry(46, (u32)irq_14, 0x08, 0x8E);
    idt_set_entry(47, (u32)irq_15, 0x08, 0x8E);
    idt_set_entry(128, (u32)isr_syscall, 0x08, 0x8E);

    load_idt((u32)&g_idt_ptr);
    asmv("sti");
}

u32 syscall_write(u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    char *str = (char*)arg1;
    printf(str);
    return 0;
}

syscall_t syscalls[2] = {
    syscall_write,
};

void syscall_handler(u32 esp) {
    u32 *stack = (u32*)esp;
    u32 syscall_number = stack[0]; // EAX contains syscall number
    u32 arg1 = stack[1]; // EBX
    u32 arg2 = stack[2]; // ECX
    u32 arg3 = stack[3]; // EDX
    u32 arg4 = stack[4]; // ESI

    if (syscall_number < 2) {
        stack[0] = syscalls[syscall_number](arg1, arg2, arg3, arg4);
    }
}

u32 syscall(u32 num, u32 arg1, u32 arg2, u32 arg3) {
    u32 ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
    );
    return ret;
}
