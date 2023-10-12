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
#include <buttons.h>
#include <gpio.h>
#include <log.h>
#include <pinmap.h>
#include <platform.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN "buttons"

static bool buttons_initialized = false;
static bool switches_initialized = false;

void buttons_init(void) {
  if (buttons_initialized == true) {
    pynq_error("buttons_destroy: buttons already initialized\n");
  }
  gpio_set_direction(IO_BTN0, GPIO_DIR_INPUT);
  gpio_set_direction(IO_BTN1, GPIO_DIR_INPUT);
  gpio_set_direction(IO_BTN2, GPIO_DIR_INPUT);
  gpio_set_direction(IO_BTN3, GPIO_DIR_INPUT);
  buttons_initialized = true;
}

void buttons_destroy(void) { /* Anything to do here? */
  if (buttons_initialized == false) {
    pynq_error("buttons_destroy: buttons weren't initialized\n");
  }
}

void switches_init(void) {
  if (switches_initialized == true) {
    pynq_error("switches_destroy: switches already initialized\n");
  }
  gpio_set_direction(IO_SW0, GPIO_DIR_INPUT);
  gpio_set_direction(IO_SW1, GPIO_DIR_INPUT);
  switches_initialized = true;
}

void switches_destroy(void) { /* Anything to do here? */
  if (switches_initialized == false) {
    pynq_error("switches_destroy: switches weren't initialized\n");
  }
}

int get_button_state(const int button) {
  if (buttons_initialized == false) {
    pynq_error("get_button_state: buttons weren't initialized\n");
  }
  if (button < 0 || button >= NUM_BUTTONS) {
    pynq_error("get_button_state: invalid button=%d, must be 0..%d-1\n",
               NUM_BUTTONS);
  }
  return (gpio_get_level(IO_BTN0 + button) == GPIO_LEVEL_LOW ? BUTTON_NOT_PUSHED
                                                             : BUTTON_PUSHED);
}

int wait_until_button_state(const int button, const int state) {
  if (buttons_initialized == false) {
    pynq_error("wait_until_button_state: buttons weren't initialized\n");
  }
  if (button < 0 || button >= NUM_BUTTONS) {
    pynq_error("get_button_state: invalid button=%d, must be 0..%d-1\n", button,
               NUM_BUTTONS);
  }
  const io_t btn = IO_BTN0 + button;
  if (gpio_get_direction(btn) != GPIO_DIR_INPUT) {
    pynq_error("get_button_state: button %d has not been set as input\n",
               button);
  }
  struct timeval call, close;
  int dTime;
  gettimeofday(&call, NULL);
  const unsigned int check =
      (state == BUTTON_NOT_PUSHED ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
  while (gpio_get_level(btn) != check) {
  }
  gettimeofday(&close, NULL);
  dTime = (close.tv_sec - call.tv_sec) * 1000.0;    // # of ms
  dTime += (close.tv_usec - call.tv_usec) / 1000.0; // # of usec in ms
  return dTime;
}

int sleep_msec_button_pushed(const int button, const int ms) {
  if (buttons_initialized == false) {
    pynq_error("sleep_msec_button: buttons weren't initialized\n");
  }
  if (button < 0 || button >= NUM_BUTTONS) {
    pynq_error("sleep_msec_button_pushed: invalid button=%d, must be 0..%d-1\n",
               button, NUM_BUTTONS);
  }
  const io_t btn = IO_BTN0 + button;
  if (gpio_get_direction(btn) != GPIO_DIR_INPUT) {
    pynq_error(
        "sleep_msec_button_pushed: button %d has not been set as input\n",
        button);
  }
  int status;
  struct timeval call, close;
  double dTime;
  // mapping call time to call struct
  gettimeofday(&call, NULL);
  do {
    // update level and latch if is pushed
    if (status != GPIO_LEVEL_HIGH) {
      status = gpio_get_level(btn);
    }
    (void)gettimeofday(&close, NULL);
    dTime = (close.tv_sec - call.tv_sec) * 1000.0;    // # of ms
    dTime += (close.tv_usec - call.tv_usec) / 1000.0; // # of usec in ms
  } while (dTime < ms);
  return (status == GPIO_LEVEL_LOW ? BUTTON_NOT_PUSHED : BUTTON_PUSHED);
}

void sleep_msec_buttons_pushed(int button_states[], const int ms) {
  if (buttons_initialized == false) {
    pynq_error("sleep_msec_buttons_pushed: buttons weren't initialized\n");
  }
  if (button_states == NULL) {
    pynq_error("sleep_msec_buttons_pushed: button_states is NULL\n");
  }
  struct timeval call, close;
  int dTime;
  const io_t buttons[NUM_BUTTONS] = {IO_BTN0, IO_BTN1, IO_BTN2, IO_BTN3};
  // mapping call time to call struct
  (void)gettimeofday(&call, NULL);
  do {
    for (int i = 0; i < NUM_BUTTONS; i++) {
      if (button_states[i] != BUTTON_PUSHED) {
        button_states[i] =
            (gpio_get_level(buttons[i]) == GPIO_LEVEL_HIGH ? BUTTON_PUSHED
                                                           : BUTTON_NOT_PUSHED);
      }
    }
    (void)gettimeofday(&close, NULL);
    dTime = (close.tv_sec - call.tv_sec) * 1000.0;    // # of ms
    dTime += (close.tv_usec - call.tv_usec) / 1000.0; // # of usec in ms
  } while (dTime < ms);
}

int wait_until_button_pushed(const int button) {
  // all checks are done in wait_until_button state
  return wait_until_button_state(button, BUTTON_PUSHED);
}

int wait_until_button_released(const int button) {
  // all checks are done in wait_until_button state
  return wait_until_button_state(button, BUTTON_NOT_PUSHED);
}

int wait_until_any_button_pushed(void) {
  const io_t buttons[NUM_BUTTONS] = {IO_BTN0, IO_BTN1, IO_BTN2, IO_BTN3};
  if (buttons_initialized == false) {
    pynq_error("wait_until_any_button_pushed: buttons weren't initialized\n");
  }
  for (int b = 0; b < NUM_BUTTONS; b++) {
    if (gpio_get_direction(b) != GPIO_DIR_INPUT) {
      pynq_error(
          "wait_until_any_button_pushed: button %d has not been set as input\n",
          b);
    }
  }
  do {
    for (int b = 0; b < NUM_BUTTONS; b++) {
      if (gpio_get_level(buttons[b]) == GPIO_LEVEL_HIGH) {
        return b; // we return the index, i.e. 0..NUM_BUTTONS-1
      }
    }
  } while (true);
}

int wait_until_any_button_released(void) {
  const io_t buttons[NUM_BUTTONS] = {IO_BTN0, IO_BTN1, IO_BTN2, IO_BTN3};
  if (buttons_initialized == false) {
    pynq_error("wait_until_any_button_released: buttons weren't initialized\n");
  }
  for (int b = 0; b < NUM_BUTTONS; b++) {
    if (gpio_get_direction(b) != GPIO_DIR_INPUT) {
      pynq_error("wait_until_any_button_released: button %d has not been set "
                 "as input\n",
                 b);
    }
  }
  do {
    for (int b = 0; b < NUM_BUTTONS; b++) {
      if (gpio_get_level(buttons[b]) == GPIO_LEVEL_LOW)
        return b; // we return the index, i.e. 0..NUM_BUTTONS-1
    }
  } while (true);
}

int get_switch_state(const int switch_num) {
  if (switches_initialized == false) {
    pynq_error("get_switch_state: switches weren't initialized\n");
  }
  if (switch_num != SWITCH0 && switch_num != SWITCH1) {
    pynq_error("get_switch_state: invalid switch_num=%d, must be 0..%i-1\n",
               switch_num, NUM_SWITCHES);
  }
  return (gpio_get_level(IO_SW0 + switch_num) == GPIO_LEVEL_LOW ? SWITCH_ON
                                                                : SWITCH_OFF);
}
