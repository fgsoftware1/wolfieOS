#ifndef PIT_H
#define PIT_H

#define PIT_CH0_DATA_PORT 0x40
#define PIT_CH1_DATA_PORT 0x41
#define PIT_CH2_DATA_PORT 0x42
#define PIT_COMMAND_PORT 0x43
#define PIT_COMMAND_REGISTER 0x36
#define PIT_FREQUENCY 1193182

#include "../libc/include/types.h"

void init_pit();
void start_pit_timer(u32 frequency);
void stop_pit_timer();
void sleep(u32 milliseconds);
u8 read_pit(u8 channel);
void write_pit(u8 channel, u8 value);

#endif
