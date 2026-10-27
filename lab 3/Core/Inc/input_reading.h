#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

// 0: mode button (PA8), 1: increase button (PA9), 2: set button (PA10).
void button_init(void);
void button_reading(void);
// These queries return the debounced level, as in the lab example.
unsigned char is_button_pressed(unsigned char index);
unsigned char is_button_pressed_1s(unsigned char index);

#endif
