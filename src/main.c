#define F_CPU 8000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "x68k-keyboard-to-pc8801.h"

#define RECIEVER_PRESCALER 64
#define RECIEVER_COUNTER_TOP (F_CPU / (2400L * RECIEVER_PRESCALER))
#define RECIEVER_COUNTER_HALF_TOP RECIEVER_COUNTER_TOP / 2

#define TRANSMITTER_PRESCALER 8
#define TRANSMITTER_COUNTER_TOP (F_CPU / (20800L * TRANSMITTER_PRESCALER))

volatile unsigned char transmitter_state, reciever_state;

volatile unsigned char reciever_buffer;
volatile unsigned char reciever_bits_recieved;

volatile unsigned char transmitter_shifted_bits = 0;
volatile unsigned char transmitter_parity_bit_set;
volatile unsigned short int transmitter_buffer = 0x0000;

unsigned char key_matrix[PC88_SUPPORTED_KEYBOARD_ROWS];

void enable_input_change_interrupt() {
	GIFR = (1 << PCIF);
	GIMSK |= (1 << PCIE);
}

void disable_input_change_interrupt() {
	GIMSK &= ~(1 << PCIE);
}

void enable_input_timer_interrupt() {
	// Enable Compare Match interrupt and set the counter so that
	// the interrupt will be triggered after a half period, right
	// into the first data bit duration
	GTCCR |= (1 << TSM) | (1 << PSR0);
	TCNT0 = (255 - RECIEVER_COUNTER_HALF_TOP);
	TIFR = (1 << OCF0A);
	TIMSK |= (1 << OCIE0A);
	GTCCR &= ~(1 << TSM);
}

void disable_input_timer_interrupt() {
	TIMSK &= ~(1 << OCIE0A);
}

void set_keyboard_ready() {
	PORTB |= (1 << X68k_READY_PIN);
}

void clear_keyboard_ready() {
	PORTB &= ~(1 << X68k_READY_PIN);
}

void enable_transmitter_timer_interrupt() {
	// Configure Timer 1 to run in CTC mode.
	GTCCR |= (1 << PSR1);
	TCNT1 = 0;
	TIFR = (1 << OCF1A);
	TIMSK |= (1 << OCIE1A);
	TCCR1 = (1 << CTC1) | (1 << CS12);
}

void disable_transmitter_timer_interrupt() {
	// Stop Timer 1 operation, and then remove the CTC
	// interrupt from the timer interrupt mask.
	TCCR1 &= ~(1 << CS12);
	TIMSK &= ~(1 << OCIE1A);
}

void send_keycode_to_pc88(unsigned short int keycode) {
	unsigned char set_bits = 0;

	// Count how much set bits are inside the keycode
	for (unsigned char i = 0; i < 12; i++) {
		if (keycode & (1 << i)) {
			set_bits++;
		}
	}

	// The PC-8801 expects an even parity. If we have an odd set bit count,
	// then set the parity bit to 1.
	transmitter_parity_bit_set = set_bits & 0x01;
	transmitter_shifted_bits = 0;
	transmitter_buffer = keycode;

	// We are ready to send the keycode to the computer.
	// Set the PC-8801 output pin to the Space level, and enable the timer.
	transmitter_state = STATE_ACTIVE;
	enable_transmitter_timer_interrupt();
	PORTB &= ~(1 << PC88_OUTPUT_PIN);
}

/*
 * Reset the adapter key matrix to its default state, and
 * also reset the PC88 keys registers.
 */
void reset_keys_matrix() {
	unsigned char current_row;

	// Set up initial values for the pressed keys map
	memset(key_matrix, 0xFF, PC88_SUPPORTED_KEYBOARD_ROWS);

	for (current_row = 0; current_row < PC88_SUPPORTED_KEYBOARD_ROWS; current_row++) {
		send_keycode_to_pc88((unsigned short int)(0xFF << 4) | current_row);

		// Busy wait until the current keycode is sent.
		while(transmitter_state != STATE_STANDBY) { }
	}
}

/*
 * Pin Change interrupt handler
 * When triggered, the start bit of the keyboard UART output
 * has been recieved by the microcontroller.
 */
ISR(PCINT0_vect) {
	if ((PINB & (1 << X68k_INPUT_PIN)) == 0) {
		disable_input_change_interrupt();
		
		// Prepare the reciever variables
		reciever_state = STATE_ACTIVE;
		reciever_buffer = 0xFF;
		reciever_bits_recieved = 0;

		enable_input_timer_interrupt();
	}
}

ISR(TIMER0_COMPA_vect) {
	unsigned char current_bit = PINB << 7;

	if (reciever_bits_recieved++ == 8) {
		// Check whether we have a stop bit
		if (current_bit && !fifo_is_full()) {
			// We have a supposedly valid byte recieved byte, we can safely
			// disable Timer 0 and push the recieved byte to its buffer.
			// We also checked that the FIFO was not full.
			fifo_write(reciever_buffer);
		}

		reciever_state = STATE_STANDBY;
		enable_input_change_interrupt();
		disable_input_timer_interrupt();
	} else {
		/*
		 * Sample the current bit: the UART convention shifts them
		 * LSB to MSB, so we shift right the reciever buffer and then
		 * toggle the MSB bit.
		 */
		reciever_buffer = (reciever_buffer >> 1) | current_bit;
	}
}

ISR(TIMER1_COMPA_vect) {
	switch (transmitter_shifted_bits) {
		case 12:
			// Parity bit case.
			if (transmitter_parity_bit_set) {
				PORTB |= (1 << PC88_OUTPUT_PIN);
			} else {
				PORTB &= ~(1 << PC88_OUTPUT_PIN);
			}

			break;

		case 13:
			// Stop bit case.
			PORTB |= (1 << PC88_OUTPUT_PIN);
			disable_transmitter_timer_interrupt();
			transmitter_state = STATE_STANDBY;
			break;

		default:
			// Regular bit case
			if (transmitter_buffer & (1 << transmitter_shifted_bits)) {
				PORTB |= (1 << PC88_OUTPUT_PIN);
			} else {
				PORTB &= ~(1 << PC88_OUTPUT_PIN);
			}
	}

	transmitter_shifted_bits++;
}

void main() {
	unsigned char input_keycode, packed_keycode, current_row, current_col, fifo_full_cleared;

	// Disable the ADC and the USI, as we don't need it
	PRR = (1 << PRADC) | (1 << PRUSI);

	/*
	 * Set port B input and output directions:
	 * - Port B.0 is X68k keyboard RxD pin (input)
	 * - Port B.1 is X68k keyboard RDY signal (output)
	 * - Port B.2 is PC88 output pin (output)
	 */
	DDRB = (1 << X68k_READY_PIN) | (1 << PC88_OUTPUT_PIN);

	// Enable the internal pull-up for the input pin and also
	// set the PC88 Output pin to the Mark level
	PORTB |= (1 << X68k_INPUT_PIN) | (1 << PC88_OUTPUT_PIN);

	// Clear the Ready signal
	clear_keyboard_ready();

	/*
	 * Configure Timer 0 for reciever operation
	 */
	
	// Set the top value for the counter
	OCR0A = RECIEVER_COUNTER_TOP;

	// Reset the counter value
	TCNT0 = 0;

	// Disable Compare Output, use CTC mode
	TCCR0A = (1 << WGM01);

	// Use the /64 prescaler
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// Only enable Pin Change interrupt for B.0
	PCMSK = (1 << X68k_INPUT_PIN);

	/*
	 * Configure Timer 1 for transmitter operation
	 */

	// Set the top value for the counter
	OCR1A = TRANSMITTER_COUNTER_TOP;
	OCR1C = TRANSMITTER_COUNTER_TOP;

	sei();

	// Clean the PC88 and local key matrix.
	reset_keys_matrix();

	// Reset the X68k keyboard input FIFO buffer
	fifo_reset();

	// Set up initial values for the reciever & sender machines
	transmitter_state = STATE_STANDBY;
	reciever_state = STATE_STANDBY;

	set_keyboard_ready();

	enable_input_change_interrupt();

	fifo_full_cleared = 1;

	while(1) {
		if ((!fifo_is_empty()) && transmitter_state == STATE_STANDBY) {
			input_keycode = fifo_read();
			packed_keycode = pgm_read_byte(&keymap[input_keycode & 0x7F]);

			if (packed_keycode == PANIC_KEY && input_keycode & 0x80) {
				// The Panic key (which is the 登録 key) has been released.
				// Declare all the rows as clean.
				fifo_reset();
				reset_keys_matrix();
			} else {
				current_row = packed_keycode & 0x0F;
				current_col = packed_keycode >> 4;

				if (input_keycode & 0x80) {
					// This is a "break" code, the key was released
					key_matrix[current_row] |= (1 << current_col);
				} else {
					// This is a "make" code, the key was pressed
					key_matrix[current_row] &= ~(1 << current_col);
				}

				send_keycode_to_pc88(
					((unsigned short int)key_matrix[current_row] << 4) | current_row);
			}
		}

		if (fifo_is_full()) {
			clear_keyboard_ready();
			disable_input_change_interrupt();
			fifo_full_cleared = 0;
		} else if (!fifo_full_cleared) {
			// Track that we recovered from a full FIFO condition to avoid messing with
			// the interrupt registers on each loop cycle
			enable_input_change_interrupt();
			set_keyboard_ready();
			fifo_full_cleared = 1;
		}
	}
}
