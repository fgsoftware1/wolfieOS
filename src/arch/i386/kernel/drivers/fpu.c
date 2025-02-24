#include "../include/drivers/fpu.h"
#include "../include/libc/include/defines.h"
#include "../include/libc/include/string.h"

void fpu_set_control_word(const u16 cw) {
    asmv("fldcw %0" ::"m"(cw));
}

void fpu_enable(registers_t *r) {
    u32 cr4;
    asmv("mov %%cr4, %0" :"=r"(cr4));
    // set 9th bit to 1 in cr4
    cr4 |= 0x200;
    asmv("mov %0, %%cr4" ::"r"(cr4));
    fpu_set_control_word(0x37F); // fpu init
    fpu_set_control_word(0x37E); // invalid operand exceptions
    fpu_set_control_word(0x37A); // divide by zero
}

void init_fpu(){
    printf("Initiating FPU...\n");
    isr_register_interrupt_handler(IRQ_FPU, fpu_enable);
}
