const io = @import("utils/io.zig");
const vga = @import("console.zig");
const multiboot = @import("multiboot.zig");
const gdt = @import("arch/i386/gdt.zig");

pub export var stack_bytes: [0x4000]u8 align(16) linksection(".bss") = undefined;

export fn _start() callconv(.Naked) noreturn {
    // =================================
    //?IN:
    //*  ESP - stack
    //?OUT:
    //*  EBX - multiboot2 info struct
    //*  EAX - bootloader magic number
    // =================================
    asm volatile (
        \\ mov %[stack_top], %%esp
        \\ pushl %%ebx
        \\ pushl %%eax
        \\ call %[kmain:P]
        \\ hlt
        :
        : [stack_top] "i" (@as([*]align(16) u8, @ptrCast(&stack_bytes)) + @sizeOf(@TypeOf(stack_bytes))),
          [kmain] "X" (&kmain),
        : "memory"
    );
}

pub fn kmain(magic: u32, mb_info: u32) callconv(.C) void {
    vga.initialize();

    // Check the bootloader magic number
    if (!multiboot.validateBootloaderMagic(magic)) {
        vga.puts("Error: Invalid bootloader magic number! Expected 0x36D76289, got 0x");
        // Convert magic number to hex string and print it
        const hex_chars = "0123456789ABCDEF";
        var i: u5 = 8;
        while (i > 0) : (i -= 1) {
            vga.putChar(hex_chars[(magic >> ((i - 1) * 4)) & 0xF]);
        }
        vga.puts("\n");
        io.hlt();
    }

    // Check the multiboot info structure alignment
    if (!multiboot.validateInfoAlignment(mb_info)) {
        vga.puts("Error: Multiboot info structure misaligned!\n");
        io.hlt();
    }

    vga.puts("Multiboot2 initialization successful!\n");

    gdt.load();

    io.hlt();
}
