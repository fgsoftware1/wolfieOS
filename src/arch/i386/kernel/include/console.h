#ifndef CONSOLE_H
#define CONSOLE_H

#include "libc/include/types.h"
#include "./drivers/vga.h"

#define MAXIMUM_PAGES 16
#define MAX_BUFFER 128

#define SCROLL_UP 1
#define SCROLL_DOWN 2

void console_clear(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);
void console_init(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);
void console_scroll(int line_count);
void console_putchar(char ch);
void console_ungetchar();
void console_ungetchar_bound(u8 n);
void console_gotoxy(u16 x, u16 y);
void console_putstr(const char *str);

void getstr(char *buffer);
void getstr_bound(char *buffer, u8 bound);

static void print_num(unsigned int num, int base, int width, char pad_char);
void printf(const char *format, ...);

#endif
