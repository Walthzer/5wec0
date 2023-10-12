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
#include "switchbox.h"
#include <libpynq.h>

char *const switchbox_names[NUM_SWITCHBOX_NAMES] = {
    /** Map pin to GPIO */
    "SWB_GPIO",
    /** Map pin to unused interrupt in pin */
    "SWB_Interrupt_In",
    /** Map pin to TX channel of UART 0 */
    "SWB_UART0_TX",
    /** Map pin to RX channel of UART 0 */
    "SWB_UART0_RX",
    /** Map pin to clock channel of SPI 0 */
    "SWB_SPI0_CLK",
    /** Map pin to miso channel of SPI 0 */
    "SWB_SPI0_MISO",
    /** Map pin to mosi channel of SPI 0 */
    "SWB_SPI0_MOSI",
    /** Map pin to ss channel of SPI 0 */
    "SWB_SPI0_SS",
    /** Map pin to clock channel of SPI 1 */
    "SWB_SPI1_CLK",
    /** Map pin to miso channel of SPI 1 */
    "SWB_SPI1_MISO",
    /** Map pin to mosi channel of SPI 1 */
    "SWB_SPI1_MOSI",
    /** Map pin to ss channel of SPI 1 */
    "SWB_SPI1_SS",
    /** Map pin to sda channel of IIC 0 */
    "SWB_IIC0_SDA",
    /** Map pin to scl channel of IIC 0 */
    "SWB_IIC0_SCL",
    /** Map pin to sda channel of IIC 1 */
    "SWB_IIC1_SDA",
    /** Map pin to scl channel of IIC 1 */
    "SWB_IIC1_SCL",
    /** Map pin to output channel of PWM 0 */
    "SWB_PWM0",
    /** Map pin to output channel of PWM 1 */
    "SWB_PWM1",
    /** not connected */
    "SWB_PWM2",
    /** not connected */
    "SWB_PWM3",
    /** not connected */
    "SWB_PWM4",
    /** not connected */
    "SWB_PWM5",
    "SWB_TIMER_G0",
    "SWB_TIMER_G1",
    /** not connected */
    "SWB_TIMER_G2",
    /** not connected */
    "SWB_TIMER_G3",
    /** not connected */
    "SWB_TIMER_G4",
    /** not connected */
    "SWB_TIMER_G5",
    /** not connected */
    "SWB_TIMER_G6",
    /** not connected */
    "SWB_TIMER_G7",
    "SWB_UART1_TX",
    "SWB_UART1_RX",
    "SWB_TIMER_IC0",
    "SWB_TIMER_IC1",
    "SWB_TIMER_IC2",
    "SWB_TIMER_IC3",
    "SWB_TIMER_IC4",
    "SWB_TIMER_IC5",
    "SWB_TIMER_IC6",
    "SWB_TIMER_IC7",
};

arm_shared ioswitch_handle;
volatile uint32_t *ioswitch = NULL;

typedef struct {
  char *name;
  char *state;
  io_configuration_t channel; // was uint8_t
} pin;

void switchbox_init(void) {
  // allocate shared memory for the switchbox and store the pointer in
  // `ioswitch`
  check_version();
  ioswitch = arm_shared_init(&ioswitch_handle, io_switch_0, 4096);
}

void switchbox_destroy(void) {
  // free the sared memory in the switchbox
  arm_shared_close(&ioswitch_handle);
}

// reset all switchbox pins to 0
void switchbox_reset(void) {
  // 32 pins to remap, 4 per word.
  for (uint_fast32_t i = 0; i < (64 / 4); i++) {
    // set all words to 0
    ioswitch[i] = 0;
  }
}

void switchbox_set_pin(const io_t pin_number,
                       const io_configuration_t io_type) {
  int numWordstoPass, byteNumber;
  uint32_t switchConfigValue;

  PIN_CHECK(pin_number);

  // If gpio is initialized, set the pin as input, if PIN_TYPE is
  // not gpio
  if (io_type != SWB_GPIO && gpio_is_initialized()) {
    // set pin as input.
    if (gpio_get_direction(pin_number) != GPIO_DIR_INPUT) {
      pynq_warning("pin: %s is set as GPIO ouput, but not mapped as GPIO. "
                   "Reconfiguring as input.",
                   pin_names[pin_number]);
      gpio_set_direction(pin_number, GPIO_DIR_INPUT);
    }
  }

  // calculate the word and byte number for the given pin number
  numWordstoPass = pin_number / 4;
  byteNumber = pin_number % 4;

  // get the current value of the word containing the pin
  switchConfigValue = ioswitch[numWordstoPass];

  // clear the byte containing the pin type and set it to the new value
  switchConfigValue = (switchConfigValue & (~(0xFF << (byteNumber * 8)))) |
                      (io_type << (byteNumber * 8));

  // update the word in the switchbox with the new value
  ioswitch[numWordstoPass] = switchConfigValue;
}

// pin_number: the number of the pin to get
// returns: the type of the given pin
io_configuration_t switchbox_get_pin(const io_t pin_number) {
  int numWordstoPass, byteNumber;
  uint32_t switchConfigValue;

  PIN_CHECK(pin_number);

  // calculate the word and byte number for the given pin number
  numWordstoPass = pin_number / 4;
  byteNumber = pin_number % 4;

  // get the value of the word containing the pin and extract the value of the
  // byte containing the pin type
  switchConfigValue = ioswitch[numWordstoPass];
  switchConfigValue = (switchConfigValue >> (byteNumber * 8)) & 0xFF;

  // return pintype
  return switchConfigValue;
}
