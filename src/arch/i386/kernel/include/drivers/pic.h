#ifndef PIC_H
#define PIC_H

#include "../libc/include/types.h"

#define PIC_MASTER_COMMAND 0X20
#define PIC_MASTER_DATA 0X21
#define PIC_SLAVE_COMMAND 0XA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_READ_IRR 0x0A
#define PIC_READ_ISR 0x0B
#define PIC_MASTER 0
#define PIC_SLAVE 1
#define PIC_EOI 0x20
#define PIC_ICW1 0x11      /* interrupt control command word PIC for initialization */
#define PIC_ICW4_8086 0x01 /* 8086/88 (MCS-80/85) mode */

void init_pic();
void pic_disable();
void pic_eoi(u8 irq);
void mask(u8 irq);
void unmask(u8 irq);

u8 pic_read_irr(u8 pic);
u8 pic_read_isr(u8 pic);
void probe_pic();

#endif
