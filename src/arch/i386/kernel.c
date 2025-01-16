#include "drivers/vga.h"

void kmain(void)
{
    vga_init();
    vga_setcolor(VGA_LIGHT_GREY);
    vga_putchar('H');
    vga_setcolor(VGA_LIGHT_BLUE);
    vga_putchar('e');
    vga_setcolor(VGA_LIGHT_GREEN);
    vga_putchar('l');
    vga_setcolor(VGA_LIGHT_RED);
    vga_putchar('l');
    vga_setcolor(VGA_LIGHT_MAGENTA);
    vga_putchar('o');

    while (0)
    {
        __asm__ volatile ("hlt");
    }
}
