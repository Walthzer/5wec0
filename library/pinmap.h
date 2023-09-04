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
#ifndef PINMAP_H
#define PINMAP_H

/**
 * @defgroup PINMAP I/O pin mapping
 *
 * @brief Definitions of I/O pin numbers and names for the switchbox and GPIO.
 *
 * For example, when calling a function, use SWB_AR0 to specify analog reference
 * pin AR0. Specifically, symbolic pin names are prefixed with SWB_ because they
 * are used as inputs to switchbox functions, but the pin name when printed
 * omits the SWB_.
 *
 * @{
 */

/**
 * @brief Definition of the number of I/O pins we have for each category.
 */

#define NUM_ANALOG_REFERENCE_PINS 14 /* # analog reference pins */
#define NUM_ANALOG_IN_PINS 6         /* # analog input pins */

typedef enum {
  /**
   * @brief Analog reference pins.
   */
  SWB_AR0 = 0,   /* reference pin 0 */
  SWB_AR1 = 1,   /* reference pin 1 */
  SWB_AR2 = 2,   /* reference pin 2 */
  SWB_AR3 = 3,   /* reference pin 3 */
  SWB_AR4 = 4,   /* reference pin 4 */
  SWB_AR5 = 5,   /* reference pin 5 */
  SWB_AR6 = 6,   /* reference pin 6 */
  SWB_AR7 = 7,   /* reference pin 7 */
  SWB_AR8 = 8,   /* reference pin 8 */
  SWB_AR9 = 9,   /* reference pin 9 */
  SWB_AR10 = 10, /* reference pin 10 */
  SWB_AR11 = 11, /* reference pin 11 */
  SWB_AR12 = 12, /* reference pin 12 */
  SWB_AR13 = 13, /* reference pin 13 */

  /**
   * @brief Analog input pins.
   */
  SWB_A0 = 14, /* analog input pin 0 */
  SWB_A1 = 15, /* analog input pin 1 */
  SWB_A2 = 16, /* analog input pin 2 */
  SWB_A3 = 17, /* analog input pin 3 */
  SWB_A4 = 18, /* analog input pin 4 */
  SWB_A5 = 19, /* analog input pin 5 */

  /**
   * @brief Switch input pins.
   */
  SWB_SW0 = 20, /* switch input pin 0 */
  SWB_SW1 = 21, /* switch input pin 1 */

  /**
   * @brief Button input pins.
   */
  SWB_BTN0 = 22, /* button input pin 0 */
  SWB_BTN1 = 23, /* button input pin 1 */
  SWB_BTN2 = 24, /* button input pin 2 */
  SWB_BTN3 = 25, /* button input pin 3 */

  /**
   * @brief LED output pins.
   */
  SWB_LD0 = 26, /* LED output pin 0 */
  SWB_LD1 = 27, /* LED output pin 1 */
  SWB_LD2 = 28, /* LED output pin 2 */
  SWB_LD3 = 29, /* LED output pin 3 */

  /**
   * @brief I2C pins.
   */
  SWB_AR_SCL = 31, /* I2C clock pin */
  SWB_AR_SDA = 30, /* I2C data pin */

  /**
   * @brief The RGB adresses for SWB_LD4 and SWB_LD5.
   */

  SWB_LD4B = 32, /* color LED 0 blue input pin */
  SWB_LD4R = 33, /* color LED 0 red input pin */
  SWB_LD4G = 34, /* color LED 0 green input pin */

  SWB_LD5B = 35, /* color LED 1 blue input pin */
  SWB_LD5R = 36, /* color LED 1 red input pin */
  SWB_LD5G = 37, /* color LED 1 green input pin */

  /**
   * @brief The RaspberryPi header-pin indexing.
   */
  SWB_RBPI40 = 38, /* RaspberryPi header pin */
  SWB_RBPI37 = 39, /* RaspberryPi header pin */
  SWB_RBPI38 = 40, /* RaspberryPi header pin */
  SWB_RBPI35 = 41, /* RaspberryPi header pin */
  SWB_RBPI36 = 42, /* RaspberryPi header pin */
  SWB_RBPI33 = 43, /* RaspberryPi header pin */
  SWB_RBPI18 = 44, /* RaspberryPi header pin */
  SWB_RBPI32 = 45, /* RaspberryPi header pin */
  SWB_RBPI10 = 46, /* RaspberryPi header pin */
  SWB_RBPI27 = 47, /* RaspberryPi header pin */
  SWB_RBPI28 = 48, /* RaspberryPi header pin */
  SWB_RBPI22 = 49, /* RaspberryPi header pin */
  SWB_RBPI23 = 50, /* RaspberryPi header pin */
  SWB_RBPI24 = 51, /* RaspberryPi header pin */
  SWB_RBPI21 = 52, /* RaspberryPi header pin */
  SWB_RBPI26 = 53, /* RaspberryPi header pin */
  SWB_RBPI19 = 54, /* RaspberryPi header pin */
  SWB_RBPI31 = 55, /* RaspberryPi header pin */
  SWB_RBPI15 = 56, /* RaspberryPi header pin */
  SWB_RBPI16 = 57, /* RaspberryPi header pin */
  SWB_RBPI13 = 58, /* RaspberryPi header pin */
  SWB_RBPI12 = 59, /* RaspberryPi header pin */
  SWB_RBPI29 = 60, /* RaspberryPi header pin */
  SWB_RBPI08 = 61, /* RaspberryPi header pin */
  SWB_RBPI07 = 62, /* RaspberryPi header pin */
  SWB_RBPI05 = 63, /* RaspberryPi header pin */

  SWB_NUM_PINS = 64,
} pin_t;

/**
 * macro that checks if the pin number is valid, throws an error if not.
 */
#define PIN_CHECK(pin)                                                         \
  do {                                                                         \
    if (pin >= SWB_NUM_PINS) {                                                 \
      pynq_error("pin %u is invalid, must be 0..%u-1.", pin, SWB_NUM_PINS);    \
    }                                                                          \
  } while (0);

/**
 * @brief Pin names.
 */
extern char *const pin_names[64];
/**
 * @}
 */
#endif // PINMAP_H
