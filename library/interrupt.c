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
#include "arm_shared_memory_system.h"
#include <fcntl.h>
#include <gpio.h>
#include <log.h>
#include <platform.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <util.h>

#define DOMAIN "Interrupt"

extern uint32_t *gpio;
extern uint32_t *intc0;

static bool gpio_initialized = false;

void check_initialization(void) {
  if (gpio_initialized == false) {
    pynq_error("Interrupts have not been initialized. Call "
               "gpio_interupt_init() first.\n");
  }
}

int gpio_interrupt_init(void) {
  int fd = open("/dev/uio1", O_RDWR, O_CLOEXEC);
  if (fd < 0) {
    pynq_error("failed to open interrupts\n");
  }
  int32_t m = 1;
  write(fd, &m, 4);
  gpio_initialized = true;
  return fd;
}

void gpio_enable_interrupt(const io_t pin) {
  check_initialization();
  int pin_bank = pin % 32;
  int bank = pin < 32 ? 0 : 1;
  if (bank == 0) {
    printf("interrupt set 0: %08X %08X\r\n", pin, pin_bank);
    intc0[0] |= (1 << pin_bank);
  } else {
    printf("interrupt set 1: %08X %08X\r\n", pin, pin_bank);
    intc0[1] |= (1 << (pin_bank));
  }
}

void gpio_disable_interrupt(const io_t pin) {
  check_initialization();
  intc0[0] &= ~(1 << pin);
}

void gpio_disable_all_interrupts(void) {
  check_initialization();
  intc0[0] = 0;
  intc0[1] = 0;
}

uint64_t gpio_get_interrupt(void) {
  check_initialization();
  uint64_t retv = intc0[3];
  retv <<= 32;
  retv |= intc0[2];
  return retv;
}

void gpio_ack_interrupt(void) {
  check_initialization();
  intc0[2] = 1;
}

void verify_interrupt_request(const io_t pin) {
  // TODO check if interrupts are initialized when using other interrupt
  // functions
  uint64_t retv = intc0[1];
  retv <<= 32;
  retv |= intc0[0];
  if (pin < 64) {
    uint64_t bitMask = 1ULL << pin;
    if (!(bitMask & retv)) {
      pynq_error("Pin %d is not enabled. Enable by using "
                 "gpio_enable_interrupt(pin). \n",
                 pin);
    }
  } else {
    if (retv == 0) {
      pynq_error("No interrupts enabled. Enable by using "
                 "gpio_enable_interrupt(pin). \n");
    }
  }
}

void gpio_print_interrupt(void) {
  check_initialization();
  //  printf("11c: %08X\r\n", gpio[0x11c / 4]);
  //  printf("128: %08X\r\n", gpio[0x128 / 4]);
  //  printf("120: %08X\r\n", gpio[0x120 / 4]);
  printf("interrupt 0: %08X %08X\r\n", intc0[0], intc0[2]);
  printf("interrupt 1: %08X %08X\r\n", intc0[1], intc0[3]);
}

void findSetBitPositions(uint64_t word, uint8_t *positions) {
  int index = 0;
  int count = 0;
  while (word) {
    if (word & 1) {
      positions[count++] = index;
    }
    word >>= 1;
    index++;
  }
}

void gpio_wait_for_interrupt(const io_t pin) {
  check_initialization();
  verify_interrupt_request(pin);
  if (pin > 63) {
    while (1) {
      uint64_t interrupt = gpio_get_interrupt();
      if (interrupt != 0) {
        break;
      }
    }
  } else {
    while (1) {
      uint64_t interrupt = gpio_get_interrupt();
      uint64_t bitMask = 1ULL << pin;
      if (bitMask & interrupt) {
        break;
      }
      sleep_msec(100);
    }
  }
}

uint8_t *gpio_get_interrupt_pins(uint8_t *positions) {
  check_initialization();
  verify_interrupt_request(64); // check if any interupt pin is enabled
  // uint8_t *positions = (uint8_t *)malloc(64 * sizeof(uint8_t));
  uint64_t pin = (uint64_t)((uint64_t)(intc0[3]) << 32 | intc0[2]);
  findSetBitPositions(pin, positions);
  // printf("Interrupted pin(s): ");
  bool empty = true;
  for (int i = 0; i < 64; i++) {
    if (positions[i] != 0) {
      empty = false;
      // printf("%d ", positions[i]);
      break;
    }
  }
  if (empty) {
    printf("WARNING: gpio_get_interrupt_pins: No pins interrupted. ");
  }
  printf("\n");
  return (positions);
}
