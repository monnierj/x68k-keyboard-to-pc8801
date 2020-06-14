#ifndef __X68K_KBD_TO_PC8801_H
#define __X68K_KBD_TO_PC8801_H

#define PC8801_KEY(ROW,COLUMN) (((COLUMN & 0X0F) << 4) | (ROW & 0X0F))
#define UNMAPPED_KEY PC8801_KEY(0x0F, 0x0F)
#define PANIC_KEY PC8801_KEY(0x0F, 0x0E)

extern const unsigned char keymap[];

#endif
