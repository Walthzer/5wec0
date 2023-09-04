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
#ifndef GPIO_H
#define GPIO_H
#include <pinmap.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup GPIO GPIO library
 *
 * @brief Functions for General Purpose I/O (GPIO) access to leds,
 * buttons, etc.
 *
 * All functions use a GPIO number from 0..NUM_PINS_SWITCHBOX-1.
 *
 * The LED and button libraries are built on top of this library, but do not
 * expose the full functionality of this library. Use this library when that is
 * required. Also see the I/O switchbox (switchbox.h) and pin mapping
 * (pinmap.h).
 *
 * In particular, be aware that the numbering used in the high-level libraries
 * is different from the underlying GPIO numbering.
 * - The button library uses 0..3 or BUTTON0..BUTTON3, and 0..1 or
 * SWITCH0..SWITCH1, whereas GPIO uses SWB_BTN0..SWB_BTN3 and SWB_SW0..SWB_SW1.
 * - The LED library uses 0..3 or LED0..LED1 for green LEDs whereas GPIO uses
 * SWB_LD0..SWB_LD3. It uses 0..1 or COLOR_LED0..COLOR_LED1 and the three color
 * components (RGB) whereas GPIO uses SWB_LD4/5R/G/B.
 * - The PWM library uses 0..5 or PWM0..PWM5, whereas GPIO uses
 * SWB_PWM0..SWB_PWM5.
 * - The UART library uses 0..1 or UART0..UART1, whereas GPIO uses
 * SWB_UART0..SWB_UART1.
 * - The ADC library is slightly different. It uses ADC0..ADC5 (these are
 * non-consecutive numbers), whereas GPIO uses SWB_A0..SWB_A5 (which are
 * consecutive).
 *
 * An example of using this library to turn LED0 on:
 * @code
 * #include <libpynq.h>
 * int main (void)
 * {
 *   // initialize the Library
 *   gpio_init();
 *   // set LED 0 as output
 *   gpio_set_direction(SWB_LD0, GPIO_DIR_OUTPUT);
 *   // turn LED 0 on
 *   gpio_set_level(SWB_LD0, GPIO_LEVEL_HIGH);
 *   sleep_msec(1000);
 *   // cleanup after use
 *   leds_destroy(); // turn LEDs off
 *   pynq_destroy();
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * @{
 */

/**
 * Enumerate the direction state (input/output) of the pin
 */
typedef enum {
  /** The GPIO pin is an input. */
  GPIO_DIR_INPUT = 0,
  /** The GPIO pin is an output. */
  GPIO_DIR_OUTPUT = 1
} gpio_direction_t;

/**
 * Enumerate the signal level.
 */
typedef enum {
  /** A low signal */
  GPIO_LEVEL_LOW = 0,
  /** A high signal */
  GPIO_LEVEL_HIGH = 1
} gpio_level_t;

/**
 * For backwards compatibility.
 * Map gpio_t to the pin_t type.
 */
#define gpio_t pin_t

/**
 * Initializes the GPIO library.
 */
extern void gpio_init(void);
/**
 * De-initialize the GPIO library.
 * This releases the memory map and memory allocated by gpio_init.
 */
extern void gpio_destroy(void);

/**
 * @brief Function is currently a no-op placeholder for arduino compatibility.
 * @param pin The GPIO pin to reset.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void gpio_reset_pin(const pin_t pin);

/**
 * @brief Set the GPIO pin as in input or output.
 * @param pin The GPIO pin to modify direction for.
 * @param direction The direction to set on the pin.
 * @warning Fails with program exit when pin or direction is outside valid
 * range.
 */
extern void gpio_set_direction(const pin_t pin,
                               const gpio_direction_t direction);

/**
 * @brief Returns the direction the set pin is initialized in.
 * @param pin The GPIO pin to read the direction set in the shared memory system
 * on the ARM processor.
 * @warning Fails with program exit when pin is outside valid range.
 */
extern gpio_direction_t gpio_get_direction(const pin_t pin);

/**
 * @brief Set the level of the output GPIO pin.
 * If the pin is configured as input, this function does nothing.
 * @param pin The GPIO pin to modify direction for
 * @param level The level to set on the pin.
 * @warning Fails with program exit when pin is outside valid range.
 */
extern void gpio_set_level(const pin_t pin, const gpio_level_t level);

/**
 * @brief Return the level of the GPIO pin.
 * @param pin The GPIO pin to read it state.
 * @returns the output level of pin using enum gpio_level_t.
 * @warning Fails with program exit when pin is outside valid range.
 */
extern gpio_level_t gpio_get_level(const pin_t pin);

/**
 * @brief Reset all GPIO pins.
 */
extern void gpio_reset(void);

/**
 * Check if gpio library is initialized.
 *
 * @return true if initialize, false if not.
 */
extern bool gpio_is_initialized(void);
/**
 * @}
 */
#endif // GPIO_H
