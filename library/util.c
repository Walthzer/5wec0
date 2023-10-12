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
#include <libpynq.h>
#include <unistd.h>

typedef struct {
  char *name;
  gpio_direction_t state;
  uint8_t channel;
  char *level;
} pin_state_t;

void sleep_msec(int msec) {
  if (msec > 0)
    usleep(msec * 1000);
}

void mapping_info(void) {
  const char *const dir[2] = {"Input", "Output"};
  printf("Pin\tName\tI/O\tLevel\tChannel\tCh_Name\t\tState\n");
  for (int i = 0; i < IO_NUM_PINS; i++) {
    pin_state_t pin_array = {
        0,
    };
    pin_array.name = pin_names[i];
    pin_array.state = gpio_get_direction(i);
    if (gpio_get_level(i) == GPIO_LEVEL_HIGH) {
      pin_array.level = "high";
    } else if (gpio_get_level(i) == GPIO_LEVEL_LOW) {
      pin_array.level = "low";
    } else {
      pin_array.level = "undef";
    }
    // get the index of the channel the pin is mapped to, 0 for none
    pin_array.channel = switchbox_get_pin(i);

    printf("%i\t%s\t%s\t%s\t%u\t", i, pin_array.name, dir[pin_array.state],
           pin_array.level, pin_array.channel);

    printf("%s\t", switchbox_names[pin_array.channel]);
    if (pin_array.channel != SWB_GPIO && pin_array.state != GPIO_DIR_INPUT) {
      printf("Invalid\n");
    } else {
      printf("Valid\n");
    }
  }
}
