# x68k-keyboard-to-pc8801

This project provides a simple firmware that can be used to make an adapter to connect a Sharp X68000 keyboard on a NEC PC-8801 Type-A computer that supports Type-A keyboard (FH/MH models or later, excluding the PC-88VA series).

The firmware is made for the Atmel/Microchip ATTiny25 microcontrollers, and should be compatible with any AVR family MCU that have 2kB of flash memory and at least 3 available I/O pins.


## How are the keys mapped?

Luckily, since both the X68000 and the Type-A PC-8801 keyboards uses the JIS layout, the vast majority of keys are exactly where they should be.

However, the PC-8801 has some proprietary keys, which are mapped on the X68000 keyboard like this:

| PC-8801 key | X68000 key |
|-------------|------------|
| GRAPH | XF1 |
| PC | XF5 |
| F6 to F10 | Unsupported |

The __登録__ key (on the top-right corner of the X68000) has been selected to be the __PANIC__ button: upon release, the firmware will reset its FIFO and clear every row on the PC-8801, which can be handy if the latter has released-but-still-repeating-key problems.

PC-8801 Type-A keyboards added "extended" keys, which were only available with a modifier on the original PC-8801 keyboards. These keys are not supported for now. The most visible case are the F6 through F10 keys, which were available as Shift+F1 through Shift+F5. For now, you will have to use the key combinations.


## How does this firmware work?

Since the ATTiny25 does not have a hardware UART, all serial communications are performed by software.

The firmware reads the keyboard's keycodes on pin __PB0__, and store the keycodes in a 8 bytes FIFO buffer.

A loop then reads the keycodes from the FIFO, translates them to the PC-8801 format using the built-in keymap, and sends them to the PC-8801 using pin __PB2__.

In the event where the FIFO buffer gets full, the keyboard's __READY__ signal connected on pin __PB1__ will be de-asserted, preventing the keyboard to send new keycodes until the next translated keycode is sent to the computer.


## How does a X68000 keyboard work?

The X68000 keyboards have a simple protocol, consisting of single bytes transmitted through an UART at 2400bps in the 8N1 format. No handshaking is performed, except for the active high __READY__ signal that forbids the keyboard to send new keycodes. No MCU-to-keyboard communication is required for the adapter operation (PC-8801 keyboards does not have any LEDs), and the keyboard does not require any initialization.

Any hardware UART should be able to understand the keyboard frame format.


## How does a PC-8801 Type-A keyboard computer work?

The PC-8801 historically used a 13 pin DIN connector for its keyboards, using 4 bits to select the _keyboard row_, and 8 bits to read which keys were pressed on that _row_ (information known as _column bits_).
The computer cycled through each row, read the column bits, and knew which keys were pressed on the whole keyboard through polling.

PC-8801 with Type-A keyboards replaces this parallel communication system by a serial one: when a key is pressed or released, the 4 bits of the row and the 8 column bits are sent.

The frame format is non-standard (UART format with 12 data bits, an even parity bit and one stop bit), and the frames are sent on a non-standard speed (20800bps). This requires a software implementation, since AVR family MCUs doesn't support these parameters.
