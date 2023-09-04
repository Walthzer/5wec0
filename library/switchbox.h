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
#ifndef SWITCHBOX_H
#define SWITCHBOX_H
#include <pinmap.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup SWITCHBOX I/O Switchbox library
 *
 * @brief The switchbox enables run-time (re)mapping of I/O pins.
 *
 * For example, output pin of UART 0 (SWB_UART0_TX) can be mapped to analog pins
 * 0 and 1 (SWB_AR0 & SWB_AR1). Or output pin PWM 0 (SWB_PWM0) can be mapped to
 * green LED 0 (SWB_LD0). Or output pin PWM 0 (SWB_PWM0) can be mapped to the
 * green component of color LED 0 (SWB_LD0).
 *
 * @warning For switchbox functions use the SWB_* naming of pins that is part of
 * switchbox.h (enum io_configuration), _not_ the names in pinmap.h.
 *
 * @code
 * #include<pinmap.h>
 * #include<switchbox.h>
 *
 * int main (void)
 * {
 *   switchbox_init();
 *   // setup UART here (not shown)
 *   // remap pin SWB_AR0 (analog reference pin 0) to UART 0 transmit
 *   switchbox_set_pin(SWB_AR0, UART0_TX);
 *   // remap pin SWB_AR1 (analog reference pin 1) to UART 0 receive
 *   switchbox_set_pin(SWB_AR1, UART0_RX);
 *   // your code here
 *   switchbox_destroy();
 * }
 * @endcode
 *
 * @{
 */

enum io_configuration {
  /** Map pin to GPIO */
  SWB_GPIO = 0x00,
  /** Map pin to internal interrupt (UNUSED) */
  SWB_Interrupt_In = 0x01,
  /** Map pin to TX channel of UART 0 */
  SWB_UART0_TX = 0x02,
  /** Map pin to RX channel of UART 0 */
  SWB_UART0_RX = 0x03,
  /** Map pin to clock channel of SPI 0 */
  SWB_SPI0_CLK = 0x04,
  /** Map pin to miso channel of SPI 0 */
  SWB_SPI0_MISO = 0x05,
  /** Map pin to mosi channel of SPI 0 */
  SWB_SPI0_MOSI = 0x06,
  /** Map pin to ss channel of SPI 0 */
  SWB_SPI0_SS = 0x07,
  /** Map pin to clock channel of SPI 1 */
  SWB_SPI1_CLK = 0x08,
  /** Map pin to miso channel of SPI 1 */
  SWB_SPI1_MISO = 0x09,
  /** Map pin to mosi channel of SPI 1 */
  SWB_SPI1_MOSI = 0x0A,
  /** Map pin to ss channel of SPI 1 */
  SWB_SPI1_SS = 0x0B,
  /** Map pin to sda channel of IIC 0 */
  SWB_IIC0_SDA = 0x0C,
  /** Map pin to scl channel of IIC 0 */
  SWB_IIC0_SCL = 0x0D,
  /** Map pin to sda channel of IIC 1 */
  SWB_IIC1_SDA = 0x0E,
  /** Map pin to scl channel of IIC 1 */
  SWB_IIC1_SCL = 0x0F,
  /** Map pin to output channel of PWM 0 */
  SWB_PWM0 = 0x10,
  /** Map pin to output channel of PWM 1 */
  SWB_PWM1 = 0x11,
  /** not connected */
  SWB_PWM2 = 0x12,
  /** not connected */
  SWB_PWM3 = 0x13,
  /** not connected */
  SWB_PWM4 = 0x14,
  /** not connected */
  SWB_PWM5 = 0x15,
  SWB_TIMER_G0 = 0x18,
  SWB_TIMER_G1 = 0x19,
  /** not connected */
  SWB_TIMER_G2 = 0x1A,
  /** not connected */
  SWB_TIMER_G3 = 0x1B,
  /** not connected */
  SWB_TIMER_G4 = 0x1C,
  /** not connected */
  SWB_TIMER_G5 = 0x1D,
  /** not connected */
  SWB_TIMER_G6 = 0x1E,
  /** not connected */
  SWB_TIMER_G7 = 0x1F,
  SWB_UART1_TX = 0x22,
  SWB_UART1_RX = 0x23,
  SWB_TIMER_IC0 = 0x38,
  SWB_TIMER_IC1 = 0x39,
  SWB_TIMER_IC2 = 0x3A,
  SWB_TIMER_IC3 = 0x3B,
  SWB_TIMER_IC4 = 0x3C,
  SWB_TIMER_IC5 = 0x3D,
  SWB_TIMER_IC6 = 0x3E,
  SWB_TIMER_IC7 = 0x3F,
  /** number elements in this enum */
  NUM_IO_CONFIGURATIONS,
};

#define NUM_SWITCHBOX_NAMES 40
/**
 * @brief Taken from scpi_names.h, lookup table for channels in the
 * mapping_info function.
 */
extern char *const switchbox_names[NUM_SWITCHBOX_NAMES];

/**
 * @brief Initializes the switch box
 *
 * Initializes the shared memory and sets the io switch base address
 */
extern void switchbox_init(void);

/**
 * @brief Set the type of a switch pin
 *
 * @param pin_number The number of the pin to set
 * @param pin_type The type of the pin (0 for input, 1 for output)
 */
extern void switchbox_set_pin(const pin_t pin_number, const uint8_t pin_type);

/**
 * @brief Resets all pins of the switch box to be input
 *
 */
extern void switchbox_reset(void);

/**
 * @brief Resets all pins of the switch box to be input
 */
extern void switchbox_destroy(void);

/**
 * @brief Sets the mode of a specified pin
 *
 * @param pin_number The pin number to set the mode for
 * @param pin_type The mode to set the pin to (input/output)
 *
 * Sets the mode of the specified pin on the io switch
 */
extern uint8_t switchbox_get_pin(const pin_t pin_number);

/**
 * @}
 */
#endif // SWITCHBOX_H
