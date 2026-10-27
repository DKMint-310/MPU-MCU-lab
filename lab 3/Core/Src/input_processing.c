#include "main.h"
#include "input_reading.h"
#include "input_processing.h"
#include "software_timer.h"

enum ButtonState {
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_PRESSED_MORE_THAN_1_SECOND
};

enum ButtonState buttonState = BUTTON_RELEASED;
static enum ButtonState buttonState2 = BUTTON_RELEASED;
static enum ButtonState buttonState3 = BUTTON_RELEASED;
volatile int mode = 1;
volatile int red_duration = 5;
volatile int red_edit = 5;
volatile int yellow_duration = 2;
volatile int yellow_edit = 2;
volatile int green_duration = 3;
volatile int green_edit = 3;
static int repeat_mode = 0;

static void increase_duration(void)
{
    switch (mode) {
    case 2:
        red_edit++;
        if (red_edit > 99) red_edit = 1;
        break;
    case 3:
        yellow_edit++;
        if (yellow_edit > 99) yellow_edit = 1;
        break;
    case 4:
        green_edit++;
        if (green_edit > 99) green_edit = 1;
        break;
    }
}

void fsm_for_input_processing(void)
{
    int previous_mode = mode;
    switch (buttonState) {
    case BUTTON_RELEASED:
        if (is_button_pressed(0)) {
            buttonState = BUTTON_PRESSED;
            mode++;
            if (mode > 4) mode = 1;
            if (mode == 2) red_edit = red_duration;
            if (mode == 3) yellow_edit = yellow_duration;
            if (mode == 4) green_edit = green_duration;
        }
        break;

    case BUTTON_PRESSED:
        if (!is_button_pressed(0)) {
            buttonState = BUTTON_RELEASED;
        } else {
            if (is_button_pressed_1s(0)) {
                buttonState = BUTTON_PRESSED_MORE_THAN_1_SECOND;
            }
        }
        break;

    case BUTTON_PRESSED_MORE_THAN_1_SECOND:
        if (!is_button_pressed(0)) {
            buttonState = BUTTON_RELEASED;
        }
        break;
    }

    // Changing mode cancels repetition until the increase button is released.
    if (mode != previous_mode) {
        repeat_mode = 0;
        setTimer2(0);
    }

    switch (buttonState2) {
    case BUTTON_RELEASED:
        if (is_button_pressed(1)) {
            buttonState2 = BUTTON_PRESSED;
            repeat_mode = 0;
            setTimer2(0);
            if (mode == previous_mode && mode >= 2 && mode <= 4) {
                repeat_mode = mode;
                increase_duration();
            }
        }
        break;
    case BUTTON_PRESSED:
        if (!is_button_pressed(1)) {
            buttonState2 = BUTTON_RELEASED;
            repeat_mode = 0;
            setTimer2(0);
        } else if (repeat_mode == mode && is_button_pressed_1s(1)) {
            buttonState2 = BUTTON_PRESSED_MORE_THAN_1_SECOND;
            setTimer2(500);
        }
        break;
    case BUTTON_PRESSED_MORE_THAN_1_SECOND:
        if (!is_button_pressed(1)) {
            buttonState2 = BUTTON_RELEASED;
            repeat_mode = 0;
            setTimer2(0);
        } else if (repeat_mode == mode && timer2_flag == 1) {
            setTimer2(500);
            increase_duration();
        }
        break;
    }

    switch (buttonState3) {
    case BUTTON_RELEASED:
        if (is_button_pressed(2)) {
            buttonState3 = BUTTON_PRESSED;
            if (mode == previous_mode) {
                switch (mode) {
                case 2: red_duration = red_edit; break;
                case 3: yellow_duration = yellow_edit; break;
                case 4: green_duration = green_edit; break;
                }
            }
        }
        break;
    case BUTTON_PRESSED:
        if (!is_button_pressed(2)) buttonState3 = BUTTON_RELEASED;
        break;
    default:
        buttonState3 = BUTTON_RELEASED;
        break;
    }
}
