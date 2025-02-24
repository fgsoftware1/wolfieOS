#include "../include/drivers/vga.h"
#include "../include/io.h"
#include "../include/libc/include/types.h"

u16 vga_item_entry(u8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color)
{
    u16 ax = 0;
    u8 ah = 0, al = 0;

    ah = back_color;
    ah <<= 4;
    ah |= fore_color;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

void vga_set_cursor_pos(u8 x, u8 y)
{

    u16 cursorLocation = y * VGA_WIDTH + x;
    outportb(0x3D4, 14);
    outportb(0x3D5, cursorLocation >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, cursorLocation);
}

void vga_disable_cursor()
{
    outportb(0x3D4, 10);
    outportb(0x3D5, 32);
}
