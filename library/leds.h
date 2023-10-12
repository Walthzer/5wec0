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
#ifndef LED_H
#define LED_H

#include <gpio.h>
#include <pinmap.h>

/**
 * @defgroup LEDS LED library
 *
 * @brief Wrappers to simplify the use of LEDs.
 *
 * - Green LEDs are numbered 0 to NUM_GREEN_LEDS-1.
 * - Only color LED 0 is used.
 * - The color LED has three components R, G, B that can be set independently to
 * mix to a color.
 *
 * LEDs can be used in three modes:
 *
 * 1. on/off mode for all green LEDs and all color LEDs
 * 2. PWM mode for green LEDs (PWM0..PWM3 are rounted to green LEDs 0..3)
 * 3. PWM mode for color LED 0 (PWM0..PWM3 are routed to color LED 0)
 *
 * An example of how to use this library.
 * @code
 * #include <libpynq.h>
 * int main (void)
 * {
 *   // initialise all I/O
 *   gpio_reset();
 *   leds_init_onoff();
 *
 *   for (int led = 0; led < NUM_GREEN_LEDS; led++)
 *     green_led_on(led);
 *   sleep_msec(500);
 *   for (int led = 0; led < NUM_GREEN_LEDS; led++)
 *     green_led_off(led);
 *
 *   // clean up after use
 *   leds_destroy(); // switches all leds off
 *   pynq_destroy();
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * LEDs can also be used through GPIO (see gpio.h and pinmap.h). Note that GPIO
 * numbering (IO_LD0..IO_LD3) is then used instead of 0..NUM_GREEN_LEDS-1
 * (LED0..LED3). In the PWM mode for color LED 0, SWB_PWM0..SWB_PWM3
 * are routed to color LED 0 (GPIO IO_LD4R, IO_LD4G, IO_LD4B).
 *
 * @{
 */

/**
 * @brief Enum of green LEDs.
 * Functions for green LEDs use a led number from 0..NUM_GREEN_LEDS-1.
 * Alternatively, you can use LEDi instead of just i if
 * you find that clearer.
 */
typedef enum {
  LED0,
  LED1,
  LED2,
  LED3,
  NUM_GREEN_LEDS,
} green_led_index_t;

/**
 * @brief Enum of color LEDs.
 * Functions for color LEDs use a led number from 0..NUM_COLOR_LEDS-1.
 * Alternatively, you can use COLOR_LEDi instead of just i if
 * you find that clearer.
 */
typedef enum {
  COLOR_LED0,
  COLOR_LED1,
  NUM_COLOR_LEDS,
} color_led_index_t;

#define NUM_LED_COLORS 3 /* # colors per color LED (RGB) */
#define NUM_LEDS (NUM_GREEN_LEDS + NUM_COLOR_LEDS)
#define LED_OFF 0
#define LED_ON 255

/**
 * @brief Initialize the green LEDs for on/off use.
 * @warning Fails with program exit when LEDs have already been to another mode.
 */
extern void leds_init_onoff(void);

/**
 * @brief Initialize the green LEDs for use with variable intensity.
 * The LED intensity can range from 0.255.
 * @warning Fails with program exit when LEDs have already been to another mode.
 */
extern void green_leds_init_pwm(void);

/**
 * @brief Initialize the color LEDs for use with variable intensity.
 * The LED intensity can range from 0.255.
 * @warning Fails with program exit when LEDs have already been to another mode.
 */
extern void color_leds_init_pwm(void);

/**
 * Unitialize the LEDs, such that the mode of the LEDs can be changed.
 * Switch all lEDs off.
 */
extern void leds_destroy(void);

/**
 * @param led The green LED.
 * @param onoff If the LEDs are in onoff mode then onoff must be either LED_ON
 * or LED_OFF. If the LEDs are in one of the PWM modes then onoff must be 0.255.
 * @warning Fails with program exit when led is outside valid range.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void green_led_onoff(const int led, const int onoff);

/**
 * @brief Same as green_led_onoff(led, LED_ON). Works in all modes.
 * @param led The green LED.
 * @warning Fails with program exit when led is outside valid range.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void green_led_on(const int led);

/**
 * @brief Same as green_led_onoff(led, LED_OFF). Works in all modes.
 * @param led The green LED.
 * @warning Fails with program exit when led is outside valid range.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void green_led_off(const int led);

/**
 * @brief Switches on/off the red component of color LED 0.
 * @param onoff If the LEDs are in onoff mode then onoff must be either LED_ON
 * or LED_OFF. If the LEDs are in one of the PWM modes then onoff must be 0.255.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void color_led_red_onoff(const int onoff);

/**
 * @brief Switches on/off the green component of color LED 0.
 * @param onoff If the LEDs are in onoff mode then onoff must be either LED_ON
 * or LED_OFF. If the LEDs are in one of the PWM modes then onoff must be 0.255.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void color_led_green_onoff(const int onoff);

/**
 * @brief Switches on/off the blue component of color LED 0.
 * @param onoff If the LEDs are in onoff mode then onoff must be either LED_ON
 * or LED_OFF. If the LEDs are in one of the PWM modes then onoff must be 0.255.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void color_led_blue_onoff(const int onoff);

/**
 * @brief Switches on/off the red/green/blue components of color LED 0.
 * @param onoff If the LEDs are in onoff mode then *_onoff must be either LED_ON
 * or LED_OFF. If the LEDs are in one of the PWM modes then *_onoff must be
 * 0.255.
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void color_led_onoff(const int red_onoff, const int green_onoff,
                            const int blue_onoff);

/**
 * @brief Set color LED 0 to white. Same as color_led_onoff(LED_ON, LED_ON,
 * LED_ON).
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void color_led_on(void);

/**
 * @brief Set color LED 0 to black. Same as color_led_onoff(LED_OFF, LED_OFF,
 * LED_OFF).
 * @warning Fails with program exit when LEDs were not initialized with the
 * correct mode.
 */
extern void color_led_off(void);

/**
 * @}
 */

#endif
