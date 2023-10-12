/*
Copyright (c) 2023 Eindhoven University of Technology

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <gpio.h>
#include <leds.h>
#include <log.h>
#include <pinmap.h>
#include <pwm.h>
#include <stdio.h>
#include <stdlib.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN "leds"

typedef enum _led_mode { uninitialized, binary, pwm_green, pwm_color } led_mode;
static led_mode mode = uninitialized;

// LEDs are either on or off
void leds_init_onoff(void) {
  if (mode == binary)
    return;
  if (mode != uninitialized) {
    pynq_error("leds_init_onoff: mode=%d should be uninitialized\n", mode);
  }
  gpio_set_direction(IO_LD0, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD1, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD2, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD3, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD4B, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD4G, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD4R, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD5B, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD5G, GPIO_DIR_OUTPUT);
  gpio_set_direction(IO_LD5R, GPIO_DIR_OUTPUT);
  mode = binary;
}

// can change the intensity of LEDs, the onoff parameters are then in the range
// 0..255
void green_leds_init_pwm(void) {
  if (mode == pwm_green)
    return;
  if (mode != uninitialized) {
    pynq_error("green_leds_init_pwm: mode=%d should be uninitialized\n", mode);
  }
  // initialize switchbox and routing PWM to LEDs
  switchbox_set_pin(IO_LD0, SWB_PWM0);
  switchbox_set_pin(IO_LD1, SWB_PWM1);
  switchbox_set_pin(IO_LD2, SWB_PWM2);
  switchbox_set_pin(IO_LD3, SWB_PWM3);
  // initialize the PWM channels
  pwm_init(PWM0, 256);
  pwm_init(PWM1, 256);
  pwm_init(PWM2, 256);
  pwm_init(PWM3, 256);
  mode = pwm_green;
}

// can change the intensity of LEDs, the onoff parameters are then in the range
// 0..255
void color_leds_init_pwm(void) {
  if (mode == pwm_color)
    return;
  if (mode != uninitialized) {
    pynq_error("color_leds_init_pwm: mode=%d should be uninitialized\n", mode);
  }
  // initialize switchbox and routing PWM to LEDs
  switchbox_set_pin(IO_LD4R, SWB_PWM0);
  switchbox_set_pin(IO_LD4G, SWB_PWM1);
  switchbox_set_pin(IO_LD4B, SWB_PWM2);
  // initialize the PWM channels
  pwm_init(PWM0, 256);
  pwm_init(PWM1, 256);
  pwm_init(PWM2, 256);
  mode = pwm_color;
}

void leds_destroy(void) {
  // note that pynq_destroy will also reset all GPIO and switch off all LEDs
  if (mode == binary) {
    for (int i = 0; i < NUM_GREEN_LEDS; i++)
      green_led_off(i);
  }
  if (mode == pwm_green || mode == pwm_color) {
    green_led_off(0);
    green_led_off(1);
    green_led_off(2);
    pwm_destroy(PWM0);
    pwm_destroy(PWM1);
    pwm_destroy(PWM2);
  }
  if (mode == pwm_green) {
    green_led_off(3);
    pwm_destroy(PWM3);
  }
  mode = uninitialized;
}

void green_led_onoff(const int led, const int onoff) {
  if (led < 0 || led >= NUM_GREEN_LEDS) {
    pynq_error("green_led_onoff: invalid led=%d, must be 0..%d-1\n",
               NUM_GREEN_LEDS);
  }
  int oo = onoff;
  switch (mode) {
  case binary:
    gpio_set_level(IO_LD0 + led,
                   (onoff == LED_OFF ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH));
    break;
  case pwm_green:
  case pwm_color:
    if (onoff < 0) {
      oo = 0;
    } else {
      if (onoff > 255) {
        oo = 255;
      }
    }
    pwm_set_duty_cycle(PWM0 + led, oo);
    break;
  default:
    pynq_error("green_led_onoff: LEDs have not been initialized with "
               "green_leds_init_pwm\n");
    break;
  }
}

void green_led_on(const int led) { green_led_onoff(led, LED_ON); }
void green_led_off(const int led) { green_led_onoff(led, LED_OFF); }
void color_led_red_onoff(const int onoff) {
  int oo = onoff;
  switch (mode) {
  case binary:
    gpio_set_level(IO_LD4R,
                   (onoff == LED_OFF ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH));
    break;
  case pwm_green:
  case pwm_color:
    if (onoff < 0) {
      oo = 0;
    } else {
      if (onoff > 255) {
        oo = 255;
      }
    }
    pwm_set_duty_cycle(PWM0, oo);
    break;
  default:
    pynq_error("color_led_red_onoff: LEDs have not been initialized with "
               "color_leds_init_pwm\n");
  }
}

void color_led_green_onoff(const int onoff) {
  int oo = onoff;
  switch (mode) {
  case binary:
    gpio_set_level(IO_LD4G,
                   (onoff == LED_OFF ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH));
    break;
  case pwm_color:
    if (onoff < 0) {
      oo = 0;
    } else {
      if (onoff > 255) {
        oo = 255;
      }
    }
    pwm_set_duty_cycle(PWM1, oo);
    break;
  default:
    pynq_error("color_led_green_onoff: LEDs have not been initialized with "
               "color_leds_init_pwm\n");
  }
}

void color_led_blue_onoff(const int onoff) {
  int oo = onoff;
  switch (mode) {
  case binary:
    gpio_set_level(IO_LD4B,
                   (onoff == LED_OFF ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH));
    break;
  case pwm_color:
    if (onoff < 0) {
      oo = 0;
    } else {
      if (onoff > 255) {
        oo = 255;
      }
    }
    pwm_set_duty_cycle(PWM2, oo);
    break;
  default:
    pynq_error("color_led_blue_onoff: LEDs have not been initialized with "
               "color_leds_init_pwm\n");
  }
}

void color_led_onoff(const int red_onoff, const int green_onoff,
                     const int blue_onoff) {
  color_led_red_onoff(red_onoff);
  color_led_green_onoff(green_onoff);
  color_led_blue_onoff(blue_onoff);
}

void color_led_on(void) { color_led_onoff(LED_ON, LED_ON, LED_ON); }
void color_led_off(void) { color_led_onoff(LED_OFF, LED_OFF, LED_OFF); }
