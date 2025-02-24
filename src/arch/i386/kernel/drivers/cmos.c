#include "../include/drivers/cmos.h"
#include "../include/drivers/pic.h"
#include "../include/console.h"
#include "../include/isr.h"
#include "../include/io.h"

void rtc_handler(registers_t *r)
{
    u8 seconds = cmos_read(RTC_SECONDS);
    u8 minutes = cmos_read(RTC_MINUTES);
    u8 hours = cmos_read(RTC_HOURS);
    u8 day = cmos_read(RTC_DAY);
    u8 month = cmos_read(RTC_MONTH);
    u8 year = cmos_read(RTC_YEAR);

    pic_eoi(IRQ_CMOS);
}

void init_cmos()
{
    printf("Initiating CMOS...\n");
    isr_register_interrupt_handler(IRQ_CMOS, rtc_handler);
    cmos_self_test();
    unmask(IRQ_CMOS - IRQ_BASE);
    cmos_write(0x0A, cmos_read(0x0A) | 0x40);
}

void cmos_self_test()
{
    printf("Performing CMOS self-test...\n");

    u8 test_address = 0x10;
    u8 test_value = 0xAA;
    u8 original_value = cmos_read(test_address);

    cmos_write(test_address, test_value);

    u8 read_value = cmos_read(test_address);

    if (read_value == test_value)
    {
        printf("CMOS self-test passed.\n");
    }
    else
    {
        printf("CMOS self-test failed.\n");
    }

    cmos_write(test_address, original_value);
}

u8 cmos_read(u8 reg)
{
    outportb(CMOS_ADDR_REG, reg);
    return inportb(CMOS_DATA_REG);
}

void cmos_write(u8 reg, u8 val)
{
    outportb(CMOS_ADDR_REG, reg);
    outportb(CMOS_DATA_REG, val);
}
