#include "../include/console.h"
#include "../include/io.h"
#include "../include/drivers/pic.h"

void init_pic()
{
    printf("Initiating PIC...\n");
    u8 a1, a2;

    a1 = inportb(PIC_MASTER_DATA);
    a2 = inportb(PIC_SLAVE_DATA);

    outportb(PIC_MASTER_COMMAND, PIC_ICW1);
    outportb(PIC_SLAVE_COMMAND, PIC_ICW1);

    outportb(PIC_MASTER_DATA, 0x20);
    outportb(PIC_SLAVE_DATA, 0x28);

    outportb(PIC_MASTER_DATA, 4);
    outportb(PIC_SLAVE_DATA, 2);

    outportb(PIC_MASTER_DATA, PIC_ICW4_8086);
    outportb(PIC_SLAVE_DATA, PIC_ICW4_8086);

    outportb(PIC_MASTER_DATA, a1);
    outportb(PIC_SLAVE_DATA, a2);

    //! MASKING ALL INTERRUPTS
    pic_disable();
}

void pic_disable()
{
    outportb(PIC_MASTER_DATA, 0xFF);
    outportb(PIC_SLAVE_DATA, 0xFF);
}

void pic_eoi(u8 irq)
{
    if (irq >= 0x28)
        outportb(PIC_SLAVE_COMMAND, PIC_EOI);
    outportb(PIC_MASTER_COMMAND, PIC_EOI);
}

void mask(u8 irq)
{
    u16 port;
    u8 value;

    if (irq < 8)
    {
        port = PIC_MASTER_DATA;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }
    value = inportb(port) | (1 << irq);
    outportb(port, value);
}

void unmask(u8 irq)
{
    u16 port;
    u8 value;

    if (irq < 8)
    {
        port = PIC_MASTER_DATA;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }
    value = inportb(port) &~ (1 << irq);
    outportb(port, value);
}

u8 pic_read_irr(u8 pic)
{
    if (pic == PIC_MASTER)
    {
        outportb(PIC_MASTER_COMMAND, PIC_READ_IRR);
        return inportb(PIC_MASTER_COMMAND);
    }
    else if (pic == PIC_SLAVE)
    {
        outportb(PIC_SLAVE_COMMAND, PIC_READ_IRR);
        return inportb(PIC_SLAVE_COMMAND);
    }
    return 0;
}

u8 pic_read_isr(u8 pic)
{
    if (pic == PIC_MASTER)
    {
        outportb(PIC_MASTER_COMMAND, PIC_READ_ISR);
        return inportb(PIC_MASTER_COMMAND);
    }
    else if (pic == PIC_SLAVE)
    {
        outportb(PIC_SLAVE_COMMAND, PIC_READ_ISR);
        return inportb(PIC_SLAVE_COMMAND);
    }
    return 0;
}

void probe_pic()
{
    printf("Probing PIC status:\n");

    u8 master_imr = inportb(PIC_MASTER_DATA);
    u8 slave_imr = inportb(PIC_SLAVE_DATA);

    printf("Master IMR: 0x%02X\n", master_imr);
    printf("Slave IMR: 0x%02X\n", slave_imr);

    u8 master_irr = pic_read_irr(PIC_MASTER);
    u8 slave_irr = pic_read_irr(PIC_SLAVE);

    printf("Master IRR: 0x%02X\n", master_irr);
    printf("Slave IRR: 0x%02X\n", slave_irr);

    u8 master_isr = pic_read_isr(PIC_MASTER);
    u8 slave_isr = pic_read_isr(PIC_SLAVE);

    printf("Master ISR: 0x%02X\n", master_isr);
    printf("Slave ISR: 0x%02X\n", slave_isr);
}
