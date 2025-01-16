#ifndef VGA_H
#define VGA_H

#include "../types.h"

enum vga_color
{
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15
};

void vga_init(void);
void vga_putchar(char c);
void vga_write(const char *data, size_t size);
void vga_writestring(const char *data);
void vga_setcolor(u8 color);
void vga_clear(void);

#endif
