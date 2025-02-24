#ifndef FPU_H
#define FPU_H

#include "../libc/include/types.h"
#include "../isr.h"

void init_fpu();
void fpu_enable(registers_t *r);
void fpu_set_control_word(const u16 cw);

#endif //FPU_H

