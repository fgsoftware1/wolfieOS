#ifndef VGA_H
#define VGA_H

#include "../libc/include/types.h"

#define VGA_ADDRESS        0xB8000
#define VGA_TOTAL_ITEMS    2200
#define VGA_WIDTH     80
#define VGA_HEIGHT    24

typedef enum {
    COLOR_BLACK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_GREY,
    COLOR_DARK_GREY,
    COLOR_BRIGHT_BLUE,
    COLOR_BRIGHT_GREEN,
    COLOR_BRIGHT_CYAN,
    COLOR_BRIGHT_RED,
    COLOR_BRIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE,
} VGA_COLOR_TYPE;

u16 vga_item_entry(u8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);

void vga_set_cursor_pos(u8 x, u8 y);

void vga_disable_cursor();

#endif
