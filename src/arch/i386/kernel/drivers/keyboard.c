#include "../include/drivers/keyboard.h"
#include "../include/drivers/pic.h"
#include "../include/console.h"
#include "../include/console.h"
#include "../include/idt.h"
#include "../include/io.h"
#include "../include/isr.h"
#include "../include/libc/include/types.h"
#include "../include/libc/include/string.h"

static bool g_caps_lock = false;
static bool g_shift_pressed = false;
char g_ch = 0, g_scan_code = 0;

char g_scan_code_chars[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0};

static int get_scancode()
{
    int i, scancode = 0;

    for (i = 1000; i > 0; i++)
    {

        if ((inportb(KEYBOARD_STATUS_PORT) & 1) == 0)
            continue;

        scancode = inportb(KEYBOARD_DATA_PORT);
        break;
    }
    if (i > 0)
        return scancode;
    return 0;
}

char alternate_chars(char ch)
{
    switch (ch)
    {
    case '`':
        return '~';
    case '1':
        return '!';
    case '2':
        return '@';
    case '3':
        return '#';
    case '4':
        return '$';
    case '5':
        return '%';
    case '6':
        return '^';
    case '7':
        return '&';
    case '8':
        return '*';
    case '9':
        return '(';
    case '0':
        return ')';
    case '-':
        return '_';
    case '=':
        return '+';
    case '[':
        return '{';
    case ']':
        return '}';
    case '\\':
        return '|';
    case ';':
        return ':';
    case '\'':
        return '\"';
    case ',':
        return '<';
    case '.':
        return '>';
    case '/':
        return '?';
    default:
        return ch;
    }
}

void keyboard_handler(registers_t *r)
{
    int scancode;

    g_ch = 0;
    scancode = get_scancode();
    g_scan_code = scancode;
    if (scancode & 0x80)
    {
    }
    else
    {

        switch (scancode)
        {
        case SCAN_CODE_KEY_CAPS_LOCK:
            if (g_caps_lock == false)
                g_caps_lock = true;
            else
                g_caps_lock = false;
            break;

        case SCAN_CODE_KEY_ENTER:
            g_ch = '\n';
            break;

        case SCAN_CODE_KEY_TAB:
            g_ch = '\t';
            break;

        case SCAN_CODE_KEY_LEFT_SHIFT:
            g_shift_pressed = true;
            break;

        default:
            g_ch = g_scan_code_chars[scancode];

            if (g_caps_lock)
            {

                if (g_shift_pressed)
                {

                    g_ch = alternate_chars(g_ch);
                }
                else
                    g_ch = upper(g_ch);
            }
            else
            {
                if (g_shift_pressed)
                {
                    if (isalpha(g_ch))
                        g_ch = upper(g_ch);
                    else

                        g_ch = alternate_chars(g_ch);
                }
                else
                    g_ch = g_scan_code_chars[scancode];
                g_shift_pressed = false;
            }
            break;
        }
    }

    pic_eoi(IRQ_KEYBOARD);
}

static void wait_for_keyboard_controller(void){
    while (inportb(KEYBOARD_STATUS_PORT) & 0x02)
        ;
}

static void keyboard_send_command(u8 command){
    wait_for_keyboard_controller();
    outportb(KEYBOARD_COMMAND_PORT, command);
}

static void keyboard_send_data(u8 data){
    wait_for_keyboard_controller();
    outportb(KEYBOARD_DATA_PORT, data);
}

void init_keyboard()
{
    printf("Initiating keyboard...\n");
    flush_keyboard_buffer();
    isr_register_interrupt_handler(IRQ_KEYBOARD, keyboard_handler);
    keyboard_self_test();
    unmask(IRQ_KEYBOARD - IRQ_BASE);
}

void flush_keyboard_buffer(void){
    while (inportb(KEYBOARD_STATUS_PORT) & 0x01)
    {
        inportb(KEYBOARD_DATA_PORT);
    }
}

void keyboard_self_test(){
    printf("Performing keyboard self-test...\n");

    // disable keyboard
    keyboard_send_command(0xAD);

    // Flush the keyboard buffer
    flush_keyboard_buffer();

    // Perform keyboard controller self-test
    keyboard_send_command(0xAA);
    if (inportb(KEYBOARD_DATA_PORT) != 0x55)
    {
        printf("Keyboard controller self-test failed\n");
        return;
    }

    // Perform keyboard interface test
    keyboard_send_command(0xAB);
    if (inportb(KEYBOARD_DATA_PORT) != 0x00)
    {
        printf("Keyboard interface test failed\n");
        return;
    }

    // Enable keyboard
    keyboard_send_command(0xAE);

    // Reset keyboard
    keyboard_send_data(0xFF);
    if (inportb(KEYBOARD_DATA_PORT) != 0xFA)
    {
        printf("Keyboard reset failed\n");
        return;
    }

    printf("keyboard self-test passed.\n");
}

char kb_getchar()
{
    char c;

    while (g_ch <= 0)
        ;
    c = g_ch;
    g_ch = 0;
    g_scan_code = 0;
    return c;
}

char kb_get_scancode()
{
    char code;

    while (g_scan_code <= 0)
        ;
    code = g_scan_code;
    g_ch = 0;
    g_scan_code = 0;
    return code;
}
