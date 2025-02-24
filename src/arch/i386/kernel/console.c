#include "include/libc/include/types.h"
#include "include/libc/include/string.h"
#include "include/console.h"
#include "include/drivers/keyboard.h"
#include "include/drivers/vga.h"

static u16 *g_vga_buffer;
static u32 g_vga_index;
static u8 cursor_pos_x = 0, cursor_pos_y = 0;
static u16 g_temp_pages[MAXIMUM_PAGES][VGA_TOTAL_ITEMS];

u8 g_fore_color = COLOR_WHITE, g_back_color = COLOR_BLACK;
u32 g_current_temp_page = 0;

void console_clear(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color)
{
    u32 i;

    for (i = 0; i < VGA_TOTAL_ITEMS; i++)
    {
        g_vga_buffer[i] = vga_item_entry(NULL, fore_color, back_color);
    }
    g_vga_index = 0;
    cursor_pos_x = 0;
    cursor_pos_y = 0;
    vga_set_cursor_pos(cursor_pos_x, cursor_pos_y);
}

void console_init(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color)
{
    printf("Initializing terminal...\n");
    g_vga_buffer = (u16 *)VGA_ADDRESS;
    g_fore_color = fore_color;
    g_back_color = back_color;
    cursor_pos_x = 0;
    cursor_pos_y = 0;
    console_clear(fore_color, back_color);
}

void console_scroll(int type)
{
    u32 i;
    if (type == SCROLL_UP)
    {

        if (g_current_temp_page > 0)
            g_current_temp_page--;
        g_current_temp_page %= MAXIMUM_PAGES;
        for (i = 0; i < VGA_TOTAL_ITEMS; i++)
        {
            g_vga_buffer[i] = g_temp_pages[g_current_temp_page][i];
        }
    }
    else
    {

        g_current_temp_page++;
        g_current_temp_page %= MAXIMUM_PAGES;
        for (i = 0; i < VGA_TOTAL_ITEMS; i++)
        {
            g_vga_buffer[i] = g_temp_pages[g_current_temp_page][i];
        }
    }
}

static void console_newline()
{
    u32 i;

    if (cursor_pos_y >= VGA_HEIGHT)
    {
        for (i = 0; i < VGA_TOTAL_ITEMS; i++)
            g_temp_pages[g_current_temp_page][i] = g_vga_buffer[i];
        g_current_temp_page++;
        cursor_pos_x = 0;
        cursor_pos_y = 0;
        console_clear(g_fore_color, g_back_color);
    }
    else
    {
        for (i = 0; i < VGA_TOTAL_ITEMS; i++)
            g_temp_pages[g_current_temp_page][i] = g_vga_buffer[i];

        g_vga_index += VGA_WIDTH - (g_vga_index % VGA_WIDTH);
        cursor_pos_x = 0;
        ++cursor_pos_y;
        vga_set_cursor_pos(cursor_pos_x, cursor_pos_y);
    }
}

void console_putchar(char ch)
{
    if (ch == ' ')
    {
        g_vga_buffer[g_vga_index++] = vga_item_entry(' ', g_fore_color, g_back_color);
        vga_set_cursor_pos(cursor_pos_x++, cursor_pos_y);
    }
    if (ch == '\t')
    {
        for (int i = 0; i < 4; i++)
        {
            g_vga_buffer[g_vga_index++] = vga_item_entry(' ', g_fore_color, g_back_color);
            vga_set_cursor_pos(cursor_pos_x++, cursor_pos_y);
        }
    }
    else if (ch == '\n')
    {
        console_newline();
    }
    else
    {
        if (ch > 0)
        {
            g_vga_buffer[g_vga_index++] = vga_item_entry(ch, g_fore_color, g_back_color);
            vga_set_cursor_pos(++cursor_pos_x, cursor_pos_y);
        }
    }
}

void console_ungetchar()
{
    if (g_vga_index > 0)
    {
        g_vga_buffer[g_vga_index--] = vga_item_entry(0, g_fore_color, g_back_color);
        if (cursor_pos_x > 0)
        {
            vga_set_cursor_pos(cursor_pos_x--, cursor_pos_y);
        }
        else
        {
            cursor_pos_x = VGA_WIDTH;
            if (cursor_pos_y > 0)
                vga_set_cursor_pos(cursor_pos_x--, --cursor_pos_y);
            else
                cursor_pos_y = 0;
        }
    }

    g_vga_buffer[g_vga_index] = vga_item_entry(0, g_fore_color, g_back_color);
}

void console_ungetchar_bound(u8 n)
{
    if (((g_vga_index % VGA_WIDTH) > n) && (n > 0))
    {
        g_vga_buffer[g_vga_index--] = vga_item_entry(0, g_fore_color, g_back_color);
        if (cursor_pos_x >= n)
        {
            vga_set_cursor_pos(cursor_pos_x--, cursor_pos_y);
        }
        else
        {
            cursor_pos_x = VGA_WIDTH;
            if (cursor_pos_y > 0)
                vga_set_cursor_pos(cursor_pos_x--, --cursor_pos_y);
            else
                cursor_pos_y = 0;
        }
    }

    g_vga_buffer[g_vga_index] = vga_item_entry(0, g_fore_color, g_back_color);
}

void console_gotoxy(u16 x, u16 y)
{
    g_vga_index = (80 * y) + x;
    cursor_pos_x = x;
    cursor_pos_y = y;
    vga_set_cursor_pos(cursor_pos_x, cursor_pos_y);
}

void console_putstr(const char *str)
{
    u32 index = 0;
    while (str[index])
    {
        if (str[index] == '\n')
            console_newline();
        else
            console_putchar(str[index]);
        index++;
    }
}

static void print_num(unsigned int num, int base, int width, char pad_char) {
    char buf[MAX_BUFFER];
    char *str = &buf[sizeof(buf) - 1];
    *str = '\0';

    do {
        *--str = "0123456789abcdef"[num % base];
        num /= base;
    } while (num > 0);

    int len = &buf[sizeof(buf) - 1] - str;
    while (len < width) {
        console_putchar(pad_char);
        width--;
    }

    while (*str) {
        console_putchar(*str++);
    }
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format != '%') {
            console_putchar(*format++);
            continue;
        }

        format++;

        int width = 0;
        char pad_char = ' ';

        if (*format == '0') {
            pad_char = '0';
            format++;
        }

        while (*format >= '0' && *format <= '9') {
            width = width * 10 + (*format - '0');
            format++;
        }

        switch (*format) {
            case 'd':
            case 'i': {
                int num = va_arg(args, int);
                if (num < 0) {
                    console_putchar('-');
                    num = -num;
                }
                print_num((unsigned int)num, 10, width, pad_char);
                break;
            }
            case 'u':
                print_num(va_arg(args, unsigned int), 10, width, pad_char);
                break;
            case 'x':
            case 'X':
                print_num(va_arg(args, unsigned int), 16, width, pad_char);
                break;
            case 'p':
                console_putchar('0');
                console_putchar('x');
                print_num((unsigned int)va_arg(args, void*), 16, width, pad_char);
                break;
            case 's': {
                const char *str = va_arg(args, const char*);
                if (!str) str = "(null)";
                int len = 0;
                while (str[len]) len++;
                while (width > len) {
                    console_putchar(pad_char);
                    width--;
                }
                while (*str) {
                    console_putchar(*str++);
                }
                break;
            }
            case 'c':
                console_putchar((char)va_arg(args, int));
                break;
            case '%':
                console_putchar('%');
                break;
            default:
                console_putchar('%');
                console_putchar(*format);
                break;
        }
        format++;
    }

    va_end(args);
}

void getstr(char *buffer)
{
    if (!buffer)
        return;
    while (1)
    {
        char ch = kb_getchar();
        if (ch == '\n')
        {
            printf("\n");
            return;
        }
        else
        {
            *buffer++ = ch;
            printf("%c", ch);
        }
    }
}

void getstr_bound(char *buffer, u8 bound)
{
    if (!buffer)
        return;
    while (1)
    {
        char ch = kb_getchar();
        if (ch == '\n')
        {
            printf("\n");
            return;
        }
        else if (ch == '\b')
        {
            console_ungetchar_bound(bound);
            buffer--;
            *buffer = '\0';
        }
        else
        {
            *buffer++ = ch;
            printf("%c", ch);
        }
    }
}
