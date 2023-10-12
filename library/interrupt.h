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
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <gpio.h>

/**
 * @defgroup INTERRUPTS Interrupt library
 *
 * @brief Functions for interrupt handling.
 *
 * An example of using this library
 * @code
 * #include <libpynq.h>
 * int main (void)
 * {
 *   gpio_init(void);
 *   gpio_reset(void);
 *   switchbox_init(void);
 *   switchbox_reset(void);
 *   gpio_set_direction(IO_LD0, GPIO_DIR_OUTPUT);
 *   // initialize the interrupt
 *   gpio_interrupt_init(void);
 *   gpio_enable_interrupt(IO_BTN0);
 *   gpio_set_direction(IO_LD0, GPIO_DIR_OUTPUT);
 *   while(1) {
 *     gpio_wait_for_interrupt(64); //Wait untill an interupt arrives
 *     uint8_t* interruptPin = gpio_get_interrupt_pins(void);
 *     if (interruptPin[0] == IO_BTN0) {
 *       printf("interrupt on IO_BTN0, turning on IO_LD0\n");
 *       gpio_set_level(IO_LD0, 1);
 *     } else {
 *       printf("interrupt on pin %d\n",interruptPin[0]);
 *       gpio_set_level(IO_LD0, 0);
 *     }
 *     gpio_ack_interrupt(void);
 *   }
 *   gpio_destroy(void);
 *   switchbox_destroy(void);
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * @{
 */

/**
 * @brief Enables interrupts to be set and read.
 *
 */

extern int gpio_interrupt_init(void);

/**
 * @brief acknowledges the raised interrupts and resets the interrupt word.
 * Allows new interrupts to occur on the previously triggered pins.
 */

extern void gpio_ack_interrupt(void);

/**
 * @brief Checks for error in enabled pin. Terminates the process if the pin is
 * not enabled.
 *
 * @param pin indicates a specific pin or if larger than 63, if any interrupt
 * pin is enabled
 *
 */
extern void verify_interrupt_request(const io_t pin);

/**
 * @brief prints the current interrupt word
 */
extern void gpio_print_interrupt(void);

/**
 * @param pin to raise interrupts
 *
 * @brief enables a specific pin to raise interrupts.
 */
extern void gpio_enable_interrupt(const io_t pin);

/**
 * @param pin to be disabled from obtianing interrupts
 *
 * @brief Disables interrupts from occuring on the specific pin.
 * Hereafter, the pin will not trigger an interrupt.
 */
extern void gpio_disable_interrupt(const io_t pin);

/**
 * @brief Disables all interrupts from being raised.
 */
extern void gpio_disable_all_interrupts(void);

/**
 * @returns the 64 bits on which interrupts are indicated by a one.
 * The bits are in accordance with the pins described in pinmap.h
 *
 */
extern uint64_t gpio_get_interrupt(void);

/**
 * @brief Gets all pins on which an interrupt occurred.
 *
 * @returns a pointer to an array of maximum 64 intergers.
 * The integers correspond to pins with a pending interrupt.
 */
extern uint8_t *gpio_get_interrupt_pins(uint8_t *positions);

/**
 * @param pin The pin on which an interrupt should occur
 *
 * @brief Waits untill an interrupt occurs on the specified pin or if the value
 * of pin is larger than 63, if any interrupt has occurred.
 */
extern void gpio_wait_for_interrupt(const io_t pin);

/**
 * @}
 */
#endif
