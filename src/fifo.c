#include "x68k-keyboard-to-pc8801.h"

// Keyboard input FIFO buffer variables
volatile unsigned char reciever_fifo_buffer[RECIEVER_FIFO_BUFFER_SIZE];
volatile unsigned char reciever_fifo_read_index, reciever_fifo_write_index, reciever_fifo_item_count;

/*
 * Reset the keyboard input FIFO buffer state variables.
 */
void fifo_reset() {
	reciever_fifo_read_index = 0;
	reciever_fifo_write_index = 0;
	reciever_fifo_item_count = 0;
}

/*
 * Write a keycode into the input FIFO buffer.
 */
void fifo_write(unsigned char keycode) {
	if (reciever_fifo_write_index == RECIEVER_FIFO_BUFFER_SIZE) {
		reciever_fifo_write_index = 0;
	}

	reciever_fifo_buffer[reciever_fifo_write_index++] = keycode;
	reciever_fifo_item_count++;
}

/*
 * Read a keycode from the input FIFO buffer.
 */
unsigned char fifo_read() {
	if (reciever_fifo_read_index == RECIEVER_FIFO_BUFFER_SIZE) {
		reciever_fifo_read_index = 0;
	}

	reciever_fifo_item_count--;
	return reciever_fifo_buffer[reciever_fifo_read_index++];
}

int fifo_is_full() {
	return reciever_fifo_item_count == RECIEVER_FIFO_BUFFER_SIZE;
}

int fifo_is_empty() {
	return reciever_fifo_item_count == 0;
}
