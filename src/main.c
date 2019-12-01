#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define X68k_INPUT_PIN	0
#define X68k_READY_PIN	1
#define PC88_OUTPUT_PIN	2

#define RECIEVER_STANDBY 0
#define RECIEVER_ACTIVE 1
#define RECIEVER_DONE 2

#define RECIEVER_PRESCALER 8
#define RECIEVER_COUNTER_TOP (F_CPU / (2400 * RECIEVER_PRESCALER)) + 1
#define RECIEVER_COUNTER_HALF_TOP RECIEVER_COUNTER_TOP / 2

volatile unsigned char reciever_status;
volatile unsigned char reciever_buffer;
volatile unsigned char reciever_bits_recieved;

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
	TCNT0 = 0;
	TIFR = (1 << OCF0A);
	TIMSK |= (1 << OCIE0A);
	OCR0A = RECIEVER_COUNTER_HALF_TOP;
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

/*
 * Pin Change interrupt handler
 * When triggered, the start bit of the keyboard UART output
 * has been recieved by the microcontroller.
 */
ISR(PCINT0_vect) {
	if ((PINB & (1 << X68k_INPUT_PIN)) == 0) {
		disable_input_change_interrupt();
		
		// Prepare the reciever variables
		reciever_status = RECIEVER_ACTIVE;
		reciever_buffer = 0xFF;
		reciever_bits_recieved = 0;

		enable_input_timer_interrupt();
	}
}

ISR(TIMER0_COMPA_vect) {
	unsigned char current_bit = PINB << 7;
	OCR0A = RECIEVER_COUNTER_TOP;

	if (reciever_bits_recieved++ == 8) {
		// Check whether we have a stop bit
		if (current_bit) {
			// We have a supposedly valid byte recieved byte, we can safely
			// disable Timer 0 and push the recieved byte to its buffer
			reciever_status = RECIEVER_DONE;
		} else {
			// We have a logic 1 on the input, this is not a stop bit.
			// We just discard the frame in this case.
			reciever_status = RECIEVER_STANDBY;
		}

		disable_input_timer_interrupt();
		// TODO: remove me
		enable_input_change_interrupt();
	} else {
		/*
		 * Sample the current bit: the UART convention shifts them
		 * LSB to MSB, so we shift right the reciever buffer and then
		 * toggle the MSB bit.
		 */
		reciever_buffer = (reciever_buffer >> 1) | current_bit;
	}
}

void main() {
	// Disable ADC, as we don't need it
	PRR = (1 << PRADC) | (1 << PRUSI);

	/*
	 * Set port B input and output directions:
	 * - Port B.0 is X68k keyboard RxD pin (input)
	 * - Port B.1 is X68k keyboard RDY signal (output)
	 * - Port B.2 is PC88 output pin (output)
	 */
	DDRB = (1 << X68k_READY_PIN) | (1 << PC88_OUTPUT_PIN) | (1 << 4) | (1 << 3);

	// Enable the internal pull-up for the input pin
	//PORTB |= (1 << X68k_INPUT_PIN);

	// Clear the Ready signal
	clear_keyboard_ready();

	// Set PC88 output pin to Mark level.
	//PORTB |= (1 << PC88_OUTPUT_PIN);

	// Set up initial values for the reciever & sender machines
	reciever_status = RECIEVER_STANDBY;

	/*
	 * Configure Timer 0 for reciever operation
	 */
	
	// Set the top value for the counter
	OCR0A = RECIEVER_COUNTER_TOP;

	// Reset the counter value
	TCNT0 = 0;

	// Disable Compare Output, use CTC mode
	TCCR0A = (1 << WGM01);

	// Use the /8 prescaler
	TCCR0B |= (1 << CS01);

	// Only enable Pin Change interrupt for B.0
	PCMSK = (1 << X68k_INPUT_PIN);

	_delay_ms(500);
	set_keyboard_ready();

	sei();
	enable_input_change_interrupt();

	while(1) {
		if (reciever_status == RECIEVER_DONE) {
			reciever_status = RECIEVER_STANDBY;

			if (reciever_buffer == 0x11) {
				PORTB |= (1 << PC88_OUTPUT_PIN);
			} else {
				PORTB &= ~(1 << PC88_OUTPUT_PIN);
			}

			if (reciever_buffer == 0x91) {
				PORTB |= (1 << 4);
			}
		}
	}
}
