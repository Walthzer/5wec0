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
#ifndef BUTTONS_H
#define BUTTONS_H

#include <gpio.h>

/**
 * @defgroup BUTTONS Button library
 *
 * @brief Wrappers to simplify the use of buttons.
 *
 * - Buttons are numbered 0..NUM_BUTTONS-1, and
 *   return values are BUTTON_PUSHED and BUTTON_NOT_PUSHED
 * - Switches are numbered 0..NUM_SWITCHES-1, and
 *   return values are SWITCH_ON and SWITCH_OFF.
 * - wait_ functions return early, i.e. as soon as the stated condition is true.
 * - sleep_ functions do not return early, i.e. always wait until the specified
 *   number of milliseconds.
 *
 * An example of how to use this library.
 * @code
 * #include <libpynq.h>
 * int main (void)
 * {
 *   // initialise all I/O
 *   pynq_init();
 *   buttons_init();
 *
 *   printf("Waiting until button 0 is pushed...\n");
 *   printf("Waited %d milliseconds\n\n", wait_until_button_pushed(0));
 *   printf("Waiting until button 0 is released...\n");
 *   printf("Waited %d milliseconds\n\n", wait_until_button_released(0));
 *
 *   // clean up after use
 *   buttons_destroy();
 *   pynq_destroy();
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * Buttons can also be used through GPIO (see gpio.h and pinmap.h). Note that
 * GPIO numbering (IO_BTN0..IO_BTN3) is then used instead of 0..NUM_BUTTONS-1
 * (BUTTON0..BUTTON3). GPIO return values are GPIO_LEVEL_LOW/HIGH
 * instead of BUTTON_(NOT_)PUSHED.
 *
 * Switches can also be used through GPIO (see gpio.h and pinmap.h). Note that
 * GPIO numbering (IO_SW0..IO_SW1) is then used instead of 0..NUM_SWITCHES-1
 * (SWITCH0..SWITCH1). GPIO return values are GPIO_LEVEL_LOW/HIGH
 * instead of SWITCH_ON/OFF.
 *
 * @{
 */

#define BUTTON_NOT_PUSHED 0
#define BUTTON_PUSHED 1
#define SWITCH_OFF 0
#define SWITCH_ON 1

/**
 * @brief Enum of buttons.
 *
 * Functions use a button numbered from 0..NUM_BUTTONS-1.
 * Alternatively, you can use BUTTONi instead of just i if you find that
 * clearer.
 */
typedef enum { BUTTON0, BUTTON1, BUTTON2, BUTTON3, NUM_BUTTONS } button_index_t;

/**
 * @brief Enum of switches.
 * Functions use a switch numbered from 0..NUM_SWITCHES-1.
 * Alternatively, you can use SWITCHi instead of just i if you find that
 * clearer.
 */
typedef enum { SWITCH0, SWITCH1, NUM_SWITCHES } switches_index_t;

/**
 * @brief Initialise the switches before they can be used.
 */
extern void switches_init(void);

/**
 * @brief Unitialize the buttons.
 */
extern void switches_destroy(void);

/**
 * @brief Initialise the buttons before they can be used.
 */
extern void buttons_init(void);

/**
 * @brief Unitialize the buttons.
 */
extern void buttons_destroy(void);

/**
 * @brief Return the state of the button (BUTTON_(NOT_)PUSHED).
 * @param button The button the state of which is returned.
 * @warning Fails with program exit when button is outside valid range.
 * @warning Fails with program exit when the direction of the button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int get_button_state(const int button);

/**
 * @brief Wait until the given button is in state (which may be immediately).
 * @param button The button of which the state is monitored.
 * @param state The state that is waited for. Must be BUTTON_PUSHED or
 * BUTTON_NOT_PUSHED.
 * @returns The number of milliseconds that was waited.
 * @warning Fails with program exit when button is outside valid range.
 * @warning Fails with program exit when the direction of the button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int wait_until_button_state(const int button, const int state);

/**
 * @brief Check if the given button is pushed in msec milliseconds.
 * The function does NOT return early.
 * @param button The button of which the state is monitored.
 * @param msec The number of milliseconds to wait.
 * @returns BUTTON_PUSHED or BUTTON_NOT_PUSHED.
 * @warning Fails with program exit when button is outside valid range.
 * @warning Fails with program exit when the direction of the button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int sleep_msec_button_pushed(const int button, const int msec);

/**
 * @brief Check if any button is pushed in msec milliseconds.
 * The function does NOT return early.
 * @param button_states The array of button states that are updated with
 * BUTTON_PUSHED or BUTTON_NOT_PUSHED.
 * @warning Fails with program exit when the direction of any button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern void sleep_msec_buttons_pushed(int button_states[], const int ms);

/**
 * @brief Wait until the given button is pushed (which may be immediately).
 * @param button The button of which the state is monitored.
 * @returns The number of milliseconds waited until the button was pushed.
 * @warning Fails with program exit when button is outside valid range.
 * @warning Fails with program exit when the direction of the button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int wait_until_button_pushed(const int button);

/**
 * @brief Wait until the given button is not pushed (which may be immediately).
 * @param button The button of which the state is monitored.
 * @returns The number of milliseconds waited until the button was released.
 * @warning Fails with program exit when button is outside valid range.
 * @warning Fails with program exit when the direction of the button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int wait_until_button_released(const int button);

/**
 * @brief Wait until any button is not pushed (which may be immediately).
 * @returns Wait until any button is pushed, return the number of the button
 * that was pushed (0..NUM_BUTTONS-1).
 * @warning Fails with program exit when the direction of any button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int wait_until_any_button_pushed(void);

/**
 * @brief Wait until the given button is not pushed (which may be immediately).
 * @returns Wait until any button is released, return the number of the button
 * that was pushed (0..NUM_BUTTONS-1).
 * @warning Fails with program exit when the direction of any button was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int wait_until_any_button_released(void);

/**
 * @returns The state of the switch number (1 for on, 0 for off).
 * @warning Fails with program exit when switch is outside valid range.
 * @warning Fails with program exit when the direction of any switch was not set
 * to input (e.g. because buttons_init was not called before).
 */
extern int get_switch_state(const int switch_num);

/**
 * @}
 */

#endif
