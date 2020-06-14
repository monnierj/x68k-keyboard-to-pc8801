#include <avr/pgmspace.h>
#include "x68k-keyboard-to-pc8801.h"

const unsigned char keymap[] PROGMEM = {
	// X68k keyboard, keys 0x00 to 0x0F
	UNMAPPED_KEY,
	PC8801_KEY(0x09, 0x07), // Escape
	PC8801_KEY(0x06, 0x01), // 1, !
	PC8801_KEY(0x06, 0x02),	// 2, "
	PC8801_KEY(0x06, 0x03), // 3, #
	PC8801_KEY(0x06, 0x04), // 4, $
	PC8801_KEY(0x06, 0x05), // 5, %
	PC8801_KEY(0x06, 0x06), // 6, &
	PC8801_KEY(0x06, 0x07), // 7, '
	PC8801_KEY(0x07, 0x00), // 8, (
	PC8801_KEY(0x07, 0x01), // 9, )
	PC8801_KEY(0x06, 0x00), // 0
	PC8801_KEY(0x05, 0x07), // Minus sign, equal
	PC8801_KEY(0x05, 0x06), // Caret
	PC8801_KEY(0x05, 0x04), // Yen symbol, vertical pipe
	PC8801_KEY(0x08, 0x03), // INSDEL, used as BS. Needs extended keycodes.
	
	// X68k keyboard, keys 0x10 to 0x1F
	PC8801_KEY(0x0A, 0x00), // TAB
	PC8801_KEY(0x04, 0x01), // Q
	PC8801_KEY(0x04, 0x07), // W
	PC8801_KEY(0x02, 0x05), // E
	PC8801_KEY(0x04, 0x02), // R
	PC8801_KEY(0x04, 0x04), // T
	PC8801_KEY(0x05, 0x01), // Y
	PC8801_KEY(0x04, 0x05), // U
	PC8801_KEY(0x03, 0x01), // I
	PC8801_KEY(0x03, 0x07), // O
	PC8801_KEY(0x04, 0x00), // P
	PC8801_KEY(0x02, 0x00), // @
	PC8801_KEY(0x05, 0x03), // [
	PC8801_KEY(0x01, 0x07), // Return
	PC8801_KEY(0x02, 0x01), // A
	PC8801_KEY(0x04, 0x03), // S
	
	// X68k keyboard, keys 0x20 to 0x2F
	PC8801_KEY(0x02, 0x04), // D
	PC8801_KEY(0x02, 0x06), // F
	PC8801_KEY(0x02, 0x07), // G
	PC8801_KEY(0x03, 0x00), // H
	PC8801_KEY(0x03, 0x02), // J
	PC8801_KEY(0x03, 0x03), // K
	PC8801_KEY(0x03, 0x04), // L
	PC8801_KEY(0x07, 0x03), // ;
	PC8801_KEY(0x07, 0x02), // :
	PC8801_KEY(0x05, 0x05), // ]
	PC8801_KEY(0x05, 0x02), // Z
	PC8801_KEY(0x05, 0x00), // X
	PC8801_KEY(0x02, 0x03), // C
	PC8801_KEY(0x04, 0x06), // V
	PC8801_KEY(0x02, 0x02), // B
	PC8801_KEY(0x03, 0x06), // N
	
	// X68k keyboard, keys 0x30 to 0x3F
	PC8801_KEY(0x03, 0x05), // M
	PC8801_KEY(0x07, 0x04), // ,
	PC8801_KEY(0x07, 0x05), // .
	PC8801_KEY(0x07, 0x06), // /
	PC8801_KEY(0x0A, 0x05), // minus sign, ru hiragana
	PC8801_KEY(0x09, 0x06), // space bar
	PC8801_KEY(0x08, 0x00), // Home. TODO: handle clear properly
	PC8801_KEY(0x0C, 0x07), // Del
	PC8801_KEY(0x0B, 0x00), // Roll Up
	PC8801_KEY(0x0B, 0x01), // Roll Down,
	UNMAPPED_KEY, // Undo
	PC8801_KEY(0x0A, 0x02), // Left
	PC8801_KEY(0x08, 0x01), // Up
	PC8801_KEY(0x08, 0x02), // Right
	PC8801_KEY(0x0A, 0x01), // Down
	PC8801_KEY(0x08, 0x00), // Clear. Mapped as Home, to be fixed.
	
	// X68k keyboard, keys 0x40 to 0x4F
	PC8801_KEY(0x0A, 0x06), // KP /
	PC8801_KEY(0x01, 0x02), // KP *
	PC8801_KEY(0x0A, 0x05), // KP -
	PC8801_KEY(0x00, 0x07), // KP 7
	PC8801_KEY(0x01, 0x00), // KP 8
	PC8801_KEY(0x01, 0x01), // KP 9
	PC8801_KEY(0x01, 0x03), // KP +
	PC8801_KEY(0x00, 0x04), // KP 4
	PC8801_KEY(0x00, 0x05), // KP 5
	PC8801_KEY(0x00, 0x06), // KP 6
	PC8801_KEY(0x01, 0x04), // KP =
	PC8801_KEY(0x00, 0x01), // KP 1
	PC8801_KEY(0x00, 0x02), // KP 2
	PC8801_KEY(0x00, 0x03), // KP 3
	PC8801_KEY(0x01, 0x07), // KP Return, using generic code
	PC8801_KEY(0x00, 0x00), // KP 0
	
	// X68k keyboard, keys 0x50 to 0x5F
	PC8801_KEY(0x01, 0x05), // KP ,
	PC8801_KEY(0x01, 0x06), // KP .
	UNMAPPED_KEY,
	PANIC_KEY,
	PC8801_KEY(0x0A, 0x03), // HELP
	PC8801_KEY(0x08, 0x04), // XF1 key, mapped as Graph key
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	PC8801_KEY(0x0D, 0x02), // XF5 key, mapped as PC key
	PC8801_KEY(0x08, 0x05), // Kana
	UNMAPPED_KEY, // Romaji?
	UNMAPPED_KEY, // Code input
	PC8801_KEY(0x0A, 0x07), // Caps, not caps lock
	UNMAPPED_KEY, // INS key. Needs regular+extended code support
	UNMAPPED_KEY, // Hiragana
	
	// X68k keyboard, keys 0x60 to 0x6F
	PC8801_KEY(0x0D, 0x03), // Full-width mode
	PC8801_KEY(0x09, 0x00), // Break, mapped as Stop
	PC8801_KEY(0x0A, 0x04), // Copy
	PC8801_KEY(0x09, 0x01), // F1
	PC8801_KEY(0x09, 0x02), // F2
	PC8801_KEY(0x09, 0x03), // F3
	PC8801_KEY(0x09, 0x04), // F4
	PC8801_KEY(0x09, 0x05), // F5
	UNMAPPED_KEY, // F6,
	UNMAPPED_KEY, // F7,
	UNMAPPED_KEY, // F8, 
	UNMAPPED_KEY, // F9,
	UNMAPPED_KEY, // and F10 need regular+extended code support
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	
	// X68k keyboard, keys 0x70 to 0x7F
	PC8801_KEY(0x08, 0x06), // Shift. X68k doesn't differentiate left and right
	PC8801_KEY(0x08, 0x07), // Control
	UNMAPPED_KEY, // Opt. 1
	UNMAPPED_KEY, // Opt. 2
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY,
	UNMAPPED_KEY
};
