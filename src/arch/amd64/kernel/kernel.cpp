#include "./include/drivers/vga.hpp"

extern "C" void kernelMain()
{
    terminal_initialize();
    terminal_setcolor(COLOR_GREEN, COLOR_BLACK);
    terminal_writestring("welcome to fgOS!");
    while(1);
}
