#ifndef OUTPUT_DISPLAY_H
#define OUTPUT_DISPLAY_H

#include <stdint.h>

/* Turn off all digit selectors and all segments. */
void disable7SEG(void);
/* Write 0..9, or 10 for E, to the segment bus. */
void display7SEG(uint8_t number);

/* Mode 1: countdown for road 1 (en0/en1) and road 2 (en2/en3).
 * Modes 2..4: mode number and editable duration. */
void displayMode(void);
/* Call continuously from main; select one digit every 2 ms. */
void scan7SEG(void);

#endif
