#ifndef CMOS_H
#define CMOS_H

#include "../libc/include/types.h"
#include "../isr.h"

#define CMOS_ADDR_REG 0x70
#define CMOS_DATA_REG 0x71

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS   0x04
#define RTC_DAY     0x07
#define RTC_MONTH   0x08
#define RTC_YEAR    0x09
#define RTC_CENTURY 0x32

void init_cmos();
u8 cmos_read(u8 reg);
void cmos_write(u8 reg, u8 val);

#endif //CMOS_H
