#ifndef __X68K_KBD_TO_PC8801_H
#define __X68K_KBD_TO_PC8801_H

#define PC8801_KEY(ROW,COLUMN) (((COLUMN & 0X0F) << 4) | (ROW & 0X0F))
#define UNMAPPED_KEY PC8801_KEY(0x0F, 0x0F)
#define PANIC_KEY PC8801_KEY(0x0F, 0x0E)

#define RECIEVER_FIFO_BUFFER_SIZE	8

// Port B pin definitions
#define X68k_INPUT_PIN	0
#define X68k_READY_PIN	1
#define PC88_OUTPUT_PIN	2

// Amount of keyboard rows supported by the PC-8801 keyboard controller.
// The sixteenth row theoretically exists, but seems unused.
#define PC88_SUPPORTED_KEYBOARD_ROWS	15

// State machine state list
#define STATE_STANDBY	0
#define STATE_ACTIVE	1


extern const unsigned char keymap[];

void fifo_reset();
void fifo_write(unsigned char keycode);
unsigned char fifo_read();
int fifo_is_full();
int fifo_is_empty();

#endif
