#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

extern volatile int mode;
extern volatile int red_duration; // Saved red duration in seconds.
extern volatile int red_edit;     // Value being edited in mode 2.
extern volatile int yellow_duration;
extern volatile int yellow_edit;
extern volatile int green_duration;
extern volatile int green_edit;
void fsm_for_input_processing(void);

#endif
