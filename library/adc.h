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
#ifndef ADC_H
#define ADC_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup ADC ADC library
 *
 * @brief Functions to use the Analog to Digital Conversion (ADC) of analog pins
 * (A0..A5 on the PYNQ board).
 *
 * Note that GPIO numbering (IO_A0..IO_A5) used in gpio.h and pinmap.h is
 * different from A0..A5.
 *
 * @{
 */

/**
 * @brief Enumerate the different available ADC channels.
 */
typedef enum {
  /** ADC channel for pin IO_A0 */
  ADC0 = ((0x240 / 4) + 1),
  /** ADC channel for pin IO_A1 */
  ADC1 = ((0x240 / 4) + 9),
  /** ADC channel for pin IO_A2 */
  ADC2 = ((0x240 / 4) + 6),
  /** ADC channel for pin IO_A3 */
  ADC3 = ((0x240 / 4) + 15),
  /** ADC channel for pin IO_A4 */
  ADC4 = ((0x240 / 4) + 5),
  /** ADC channel for pin IO_A5 */
  ADC5 = ((0x240 / 4) + 13),
} adc_channel_t;

/**
 * @brief Check if ADC has been initialized.
 * @returns True when initialized, false otherwise.
 */
extern bool initialized_adc(void);

/**
 * @brief Initialization of the ADC library.
 */
extern void adc_init(void);

/**
 * @brief De-initialize the ADC library and free up the used memory in the
 * shared memory space.
 */
extern void adc_destroy(void);

/**
 * @param channel The channel to read the analog value from.
 * Read ADC channel #channel and return the read out voltage.
 * @returns a value between 0.0 and 3.3V.
 * @warning Fails with program exit when channel is outside valid range or has
 * not been initialized..
 */
extern double adc_read_channel(adc_channel_t channel);

/**
 * @param channel The channel to read the analog value from.
 * Read ADC channel #channel and return the raw value.
 * @returns a value between 0 and 65535.
 * @warning Fails with program exit when channel is outside valid range.
 */
extern uint32_t adc_read_channel_raw(adc_channel_t channel);

/**
 * @}
 */

#endif // ADC_H
