const gdt = @import("../arch/i386/gdt.zig");
const idt = @import("../arch/i386/idt.zig");

pub fn hlt() void {
    asm volatile (
        \\ hlt
    );
}

pub fn inb(port: u16) u8 {
    return asm volatile (
        \\ inb %[port], %[result]
        : [result] "={al}" (-> u8),
        : [port] "N{dx}" (port),
    );
}

pub fn outb(port: u16, data: u8) void {
    asm volatile (
        \\ outb %[data], %[port]
        :
        : [port] "{dx}" (port),
          [data] "{al}" (data),
    );
}

pub fn lgdt(gdt_ptr: *const gdt.GDTPtr) void {
    // Load the GDT into the CPU
    asm volatile (
        \\ lgdt (%%eax)
        :
        : [gdt_ptr] "{eax}" (gdt_ptr),
        : "memory"
    );
}

pub fn ltr(offset: u16) void {
    asm volatile (
        \\ ltr %%ax
        :
        : [offset] "{ax}" (offset),
        : "memory"
    );
}

pub fn lidt(idt_ptr: *const idt.IDTPtr) void {
    asm volatile (
        \\ lidt (%%eax)
        :
        : [idt_ptr] "{eax}" (idt_ptr),
        : "memory"
    );
}
