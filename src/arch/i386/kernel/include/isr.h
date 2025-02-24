#ifndef ISR_H
#define ISR_H

#include "libc/include/types.h"

#define NO_INTERRUPT_HANDLERS 256

typedef struct
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*isr_t)(registers_t *);
typedef void (*exception_t)();

void isr_register_interrupt_handler(u8 num, isr_t handler);
void isr_register_exception_handler(u8 num, exception_t handler);
void isr_end_interrupt(int num);
void isr_irq_handler(registers_t *reg);

extern void exception_0();
extern void exception_1();
extern void exception_2();
extern void exception_3();
extern void exception_4();
extern void exception_5();
extern void exception_6();
extern void exception_7();
extern void exception_8();
extern void exception_9();
extern void exception_10();
extern void exception_11();
extern void exception_12();
extern void exception_13();
extern void exception_14();
extern void exception_15();
extern void exception_16();
extern void exception_17();
extern void exception_18();
extern void exception_19();
extern void exception_20();
extern void exception_21();
extern void exception_22();
extern void exception_23();
extern void exception_24();
extern void exception_25();
extern void exception_26();
extern void exception_27();
extern void exception_28();
extern void exception_29();
extern void exception_30();
extern void exception_31();
extern void exception_128();

extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();

#define IRQ_BASE            0x20
#define IRQ_TIMER           (IRQ_BASE + 0x00)
#define IRQ_KEYBOARD        (IRQ_BASE + 0x01)
#define IRQ_CASCADE         (IRQ_BASE + 0x02)
#define IRQ_SERIAL_PORT2    (IRQ_BASE + 0x03)
#define IRQ_SERIAL_PORT1    (IRQ_BASE + 0x04)
#define IRQ_AUDIO           (IRQ_BASE + 0x05)
#define IRQ_DISKETTE_DRIVE  (IRQ_BASE + 0x06)
#define IRQ_PARALLEL_PORT   (IRQ_BASE + 0x07)
#define IRQ_CMOS            (IRQ_BASE + 0x08)
#define IRQ_ACPI            (IRQ_BASE + 0x09)
#define IRQ_NIC1            (IRQ_BASE + 0x0A)
#define IRQ_NIC2            (IRQ_BASE + 0x0B)
#define IRQ_AUXILIARY       (IRQ_BASE + 0x0C)
#define IRQ_FPU             (IRQ_BASE + 0x0D)
#define IRQ_ATA1            (IRQ_BASE + 0x0E)
#define IRQ_ATA2            (IRQ_BASE + 0x0F)

#endif
