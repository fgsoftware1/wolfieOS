#include "kernel.h"
#include "gdt.h"

void kmain32(u32 magic, u32 multiboot_info)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        return;
    }

    if (multiboot_info & 7)
    {
        return;
    }

    gdt_init();

    while (true)
    {
        asm volatile("hlt");
    }
}
