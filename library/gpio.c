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
#include "gpio.h"
#include "arm_shared_memory_system.h"
#include <log.h>
#include <pinmap.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <version.h>

static arm_shared gpio_handle, intc0_handle;
volatile uint32_t *gpio = NULL;
volatile uint32_t *intc0 = NULL;

bool gpio_is_initialized(void) {
  /* if gpio == NULL we know we are not inialized */
  return (gpio != NULL) ? true : false;
}

void gpio_init(void) {
  pynq_info("Initialize");
  check_version();
  gpio = arm_shared_init(&gpio_handle, axi_gpio_0, 4096);
  intc0 = arm_shared_init(&intc0_handle, axi_intc_0, 4096);
}

void gpio_destroy(void) {
  pynq_info("Destroy");
  arm_shared_close(&gpio_handle);
  arm_shared_close(&intc0_handle);
  gpio = NULL;
  intc0 = NULL;
}

void gpio_reset_pin(const io_t pin) {
  PIN_CHECK(pin);
  pynq_info("Reset pin: %d", pin);
  gpio_set_direction(pin, GPIO_DIR_INPUT);
  gpio_set_level(pin, GPIO_LEVEL_LOW);
}

void gpio_reset(void) {
  pynq_info("Reset all pins");
  // set all pins as input
  gpio[1] = 0xFFFFFFFF;
  // re-set all outputs to 0
  gpio[0] = 0x0;

  // set all pins as input
  gpio[3] = 0xFFFFFFFF;
  // re-set all outputs to 0
  gpio[2] = 0x0;
  // disable all interrupts
  intc0[0] = 0;
  intc0[1] = 0;
  // remove all pending interrupts
  intc0[2] = 0;
  intc0[3] = 0;
}

void gpio_set_direction(const io_t pin, const gpio_direction_t dir) {
  PIN_CHECK(pin);
  if (!(dir == GPIO_DIR_INPUT || dir == GPIO_DIR_OUTPUT)) {
    pynq_error("gpio_set_direction: invalid direction %d", dir);
  }
  int pin_bank = pin % 32;
  int bank = pin < 32 ? 1 : 3;
  if (dir == GPIO_DIR_INPUT) {
    gpio[bank] = gpio[bank] | (1 << pin_bank);
  } else {
    gpio[bank] = gpio[bank] & ~(1 << pin_bank);
  }
}

gpio_direction_t gpio_get_direction(const io_t pin) {
  PIN_CHECK(pin);
  int pin_bank = pin % 32;
  int bank = pin < 32 ? 1 : 3;
  int dir =
      ((gpio[bank] & (1 << pin_bank)) != 0) ? GPIO_DIR_INPUT : GPIO_DIR_OUTPUT;
  return dir;
}

void gpio_set_level(const io_t pin, const gpio_level_t level) {
  PIN_CHECK(pin);
  if (!(level == GPIO_LEVEL_HIGH || level == GPIO_LEVEL_LOW)) {
    pynq_error("gpio_set_level: level %d is invalid", level);
  }
  int pin_bank = pin % 32;
  int bank = pin < 32 ? 0 : 2;
  if (level == GPIO_LEVEL_HIGH) {
    gpio[bank] = gpio[bank] | (1 << pin_bank);
  } else {
    gpio[bank] = gpio[bank] & ~(1 << pin_bank);
  }
}

gpio_level_t gpio_get_level(const io_t pin) {
  PIN_CHECK(pin);
  int pin_bank = pin % 32;
  int bank = pin < 32 ? 0 : 2;
  return (gpio[bank] & (1 << pin_bank)) != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
}
