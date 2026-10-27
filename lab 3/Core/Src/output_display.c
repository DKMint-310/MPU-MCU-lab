#include "output_display.h"
#include "main.h"
#include "input_processing.h"
#include "software_timer.h"

static uint8_t led_buffer[4] = {0, 1, 0, 0};

static const uint8_t segment_code[11] = {
  0xC0, 0xF9, 0xA4, 0xB0, 0x99,
  0x92, 0x82, 0xF8, 0x80, 0x90, 0x86
};

void disable7SEG(void)
{
  HAL_GPIO_WritePin(GPIOB, en0_Pin | en1_Pin | en2_Pin | en3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, leda_Pin | ledb_Pin | ledc_Pin | ledd_Pin |
                          lede_Pin | ledf_Pin | ledg_Pin, GPIO_PIN_SET);
}

void display7SEG(uint8_t number)
{
  if (number > 10) return;
  for (int i = 0; i < 7; i++)
  {
    HAL_GPIO_WritePin(GPIOB, (uint16_t)(GPIO_PIN_0 << i),
        (segment_code[number] & (1U << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

void scan7SEG(void)
{
  static uint32_t last_scan = 0;
  static int index = 0;
  uint32_t now = HAL_GetTick();

  if ((uint32_t)(now - last_scan) < 2) return;
  last_scan = now;

  disable7SEG();
  display7SEG(led_buffer[index]);
  switch (index) {
  case 0: HAL_GPIO_WritePin(en0_GPIO_Port, en0_Pin, GPIO_PIN_RESET); break;
  case 1: HAL_GPIO_WritePin(en1_GPIO_Port, en1_Pin, GPIO_PIN_RESET); break;
  case 2: HAL_GPIO_WritePin(en2_GPIO_Port, en2_Pin, GPIO_PIN_RESET); break;
  case 3: HAL_GPIO_WritePin(en3_GPIO_Port, en3_Pin, GPIO_PIN_RESET); break;
  }
  index++;
  if (index >= 4) index = 0;
}

enum TrafficState { GREEN_RED, YELLOW_RED, RED_GREEN, RED_YELLOW };
static enum TrafficState traffic_state = GREEN_RED;
static int countdown1, countdown2;
static int normal_red, normal_yellow, normal_green;
static int timing_error = 0;
static uint32_t last_second = 0;

static void allTrafficOff(void)
{
  HAL_GPIO_WritePin(GPIOA, RED_Op_Pin | YELLOW_Op_Pin | GREEN_Op_Pin |
                          RED_Pin | YELLOW_Pin | GREEN_Pin, GPIO_PIN_SET);
}

static void showTrafficState(void)
{
  allTrafficOff();
  switch (traffic_state) {
  case GREEN_RED:
    HAL_GPIO_WritePin(GPIOA, GREEN_Pin | RED_Op_Pin, GPIO_PIN_RESET);
    break;
  case YELLOW_RED:
    HAL_GPIO_WritePin(GPIOA, YELLOW_Pin | RED_Op_Pin, GPIO_PIN_RESET);
    break;
  case RED_GREEN:
    HAL_GPIO_WritePin(GPIOA, RED_Pin | GREEN_Op_Pin, GPIO_PIN_RESET);
    break;
  case RED_YELLOW:
    HAL_GPIO_WritePin(GPIOA, RED_Pin | YELLOW_Op_Pin, GPIO_PIN_RESET);
    break;
  }
}

static void startNormal(void)
{
  normal_red = red_duration;
  normal_yellow = yellow_duration;
  normal_green = green_duration;
  timing_error = (normal_red < 1 || normal_red > 99 ||
                  normal_yellow < 1 || normal_yellow > 99 ||
                  normal_green < 1 || normal_green > 99 ||
                  normal_red != normal_green + normal_yellow);
  last_second = HAL_GetTick();
  if (timing_error) {
    allTrafficOff();
    HAL_GPIO_WritePin(GPIOA, RED_Pin | RED_Op_Pin, GPIO_PIN_RESET);
    return;
  }
  traffic_state = GREEN_RED;
  countdown1 = normal_green;
  countdown2 = normal_red;
  showTrafficState();
}

static void runNormal(void)
{
  if (timing_error) {
    for (int i = 0; i < 4; i++) led_buffer[i] = 10; // EE EE
    return;
  }

  uint32_t now = HAL_GetTick();
  enum TrafficState old_state = traffic_state;
  while ((uint32_t)(now - last_second) >= 1000) {
    last_second += 1000;
    countdown1--;
    countdown2--;
    switch (traffic_state) {
    case GREEN_RED:
      if (countdown1 == 0) {
        traffic_state = YELLOW_RED;
        countdown1 = normal_yellow;
      }
      break;
    case YELLOW_RED:
      if (countdown1 == 0) {
        traffic_state = RED_GREEN;
        countdown1 = normal_red;
        countdown2 = normal_green;
      }
      break;
    case RED_GREEN:
      if (countdown2 == 0) {
        traffic_state = RED_YELLOW;
        countdown2 = normal_yellow;
      }
      break;
    case RED_YELLOW:
      if (countdown2 == 0) {
        traffic_state = GREEN_RED;
        countdown1 = normal_green;
        countdown2 = normal_red;
      }
      break;
    }
  }
  if (traffic_state != old_state) showTrafficState();
  led_buffer[0] = (uint8_t)(countdown1 / 10);
  led_buffer[1] = (uint8_t)(countdown1 % 10);
  led_buffer[2] = (uint8_t)(countdown2 / 10);
  led_buffer[3] = (uint8_t)(countdown2 % 10);
}

void displayMode(void)
{
  static int previous_mode = 0;
  uint16_t selected_leds = 0;
  int value = 0;
  switch (mode) {
  case 2:
    selected_leds = RED_Op_Pin | RED_Pin;
    value = red_edit;
    break;
  case 3:
    selected_leds = YELLOW_Op_Pin | YELLOW_Pin;
    value = yellow_edit;
    break;
  case 4:
    selected_leds = GREEN_Op_Pin | GREEN_Pin;
    value = green_edit;
    break;
  }

  if (mode != previous_mode) {
    allTrafficOff();
    setTimer(0);
    if (mode == 1) {
      startNormal();
    } else if (selected_leds != 0) {
      HAL_GPIO_WritePin(GPIOA, selected_leds, GPIO_PIN_RESET);
      setTimer(250);
    }
    previous_mode = mode;
  }

  if (mode == 1) {
    runNormal();
    return;
  }

  led_buffer[0] = 0;
  led_buffer[1] = (uint8_t)mode;
  led_buffer[2] = (uint8_t)(value / 10);
  led_buffer[3] = (uint8_t)(value % 10);
  if (selected_leds != 0 && timer_flag == 1) {
    setTimer(250);
    HAL_GPIO_TogglePin(GPIOA, selected_leds);
  }
}
