#include "../include/drivers/pit.h"
#include "../include/drivers/pic.h"
#include "../include/console.h"
#include "../include/io.h"
#include "../include/isr.h"

volatile u32 pit_ticks = 0;

void pit_handler(registers_t *r) { pit_ticks ++; }

void init_pit() {
  printf("Initiating PIT...\n");
  isr_register_interrupt_handler(IRQ_TIMER, pit_handler);
  unmask(IRQ_TIMER - IRQ_BASE);
}

void start_pit_timer(u32 frequency) {
    u32 divisor = PIT_FREQUENCY / frequency;
    outportb(PIT_COMMAND_REGISTER, 0x36); // Set the operating mode to square wave generator
    outportb(PIT_CH0_DATA_PORT, divisor & 0xFF); // Set low byte of divisor
    outportb(PIT_CH0_DATA_PORT, (divisor >> 8) & 0xFF); // Set high byte of divisor
}

void stop_pit_timer() { outportb(PIT_COMMAND_PORT, 0x30); }

u8 read_pit(u8 channel) {
  u8 port;
  switch (channel) {
  case 0:
    port = PIT_CH0_DATA_PORT;
    break;
  case 1:
    port = PIT_CH1_DATA_PORT;
    break;
  case 2:
    port = PIT_CH2_DATA_PORT;
    break;
  default:
    // Handle invalid channel
    break;
  }

  // Read from the specified channel
  u8 value = inportb(port);
  return value;
}

void write_pit(u8 channel, u8 value) {
  u8 port;
  switch (channel) {
  case 0:
    port = PIT_CH0_DATA_PORT;
    break;
  case 1:
    port = PIT_CH1_DATA_PORT;
    break;
  case 2:
    port = PIT_CH2_DATA_PORT;
    break;
  default:
    // Handle invalid channel
    break;
  }

  // Write to the specified channel
  outportb(port, value);
}

void sleep(u32 milliseconds) {
    u32 ticks = (milliseconds * PIT_FREQUENCY) / 1000;

    // Save the current PIT channel 0 count
    u8 initialCounterLow = inportb(PIT_CH0_DATA_PORT);
    u8 initialCounterHigh = inportb(PIT_CH0_DATA_PORT);

    pit_ticks = 0;
    start_pit_timer(1000); // Set PIT to 1ms intervals

    while (pit_ticks < ticks) {
        // Wait for PIT interrupt
    }

    // Restore the initial PIT channel 0 count
    outportb(PIT_COMMAND_REGISTER, 0x36); // Set the operating mode to square wave generator
    outportb(PIT_CH0_DATA_PORT, initialCounterLow); // Restore low byte of initial counter
    outportb(PIT_CH0_DATA_PORT, initialCounterHigh); // Restore high byte of initial counter
}
