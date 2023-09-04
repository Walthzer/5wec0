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
#ifndef PWM_H
#define PWM_H
#include <libpynq.h>

/**
 * @defgroup PWM PWM library
 *
 * @brief Functions to use Pulse Width Modulation (PWM).
 *
 * Each of the 6 PWM channels (numbered 0..NUM_PWMS-1)
 * can be linked to any mappable pin (e.g. green or color LEDs, buttons).
 *
 * PWM can also be used through GPIO (see gpio.h and pinmap.h). Note that GPIO
 * numbering (SWB_PWM0..SWB_PWM5) is then used instead of 0..NUM_PWMS-1
 * (PWM0..PWM5).
 *
 * @{
 */

/**
 * @brief Enum of PWM channels.
 *
 * All functions use a PWM channel from 0..NUM_PWMS-1.
 * Alternatively, you can use PWMi instead of just i if you find that clearer.
 */
typedef enum { PWM0, PWM1, PWM2, PWM3, PWM4, PWM5, NUM_PWMS } pwm_index_t;

/**
 * @brief Checks if the channel index is initialized.
 * @param pwm The PWM channel
 * @return True if initialized, false if not
 * @warning Fails with program exit if pwm is outside valid range.
 */
extern bool pwm_initialized(const int pwm);

/**
 * @brief Initializes the PWM channel with the specified period.
 * @param pwm the PWM channel to initialize.
 * @param period The period to set for the PWM channel.
 * @warning Fails with program exit if pwm is outside valid range.
 */
extern void pwm_init(const int pwm, const uint32_t period);

/**
 * @brief Removes the instantiated shared memory system of the PWM channel.
 * @param pwm The PWM channel to destroy.
 * @warning Fails with program exit if pwm is outside valid range.
 */
extern void pwm_destroy(const int pwm);

/**
 * @brief Sets the duty cycle for the specified PWM channel.
 * @param pwm The PWM channel.
 * @param duty The duty cycle to set for the PWM channel.
 * @warning Fails with program exit if pwm is outside valid range or if pwm has
 * not been initialized.
 */
extern void pwm_set_duty_cycle(const int pwm, const uint32_t duty);

/**
 * @brief Sets the period for the specified PWM channel.
 * @param pwm The PWM channel.
 * @param period The period to set for the PWM channel.
 * @warning Fails with program exit if pwm is outside valid range or if pwm has
 * not been initialized.
 */
extern void pwm_set_period(const int pwm, const uint32_t period);

/**
 * @brief Returns the period of a certain PWM channel.
 * @param pwm The PWM channel.
 * @return The period of the specified PWM channel as an uint32_t.
 * @warning Fails with program exit if pwm is outside valid range or if pwm has
 * not been initialized.
 */
uint32_t pwm_get_period(const int pwm);

/**
 * @brief Gets the duty cycle of the specified PWM channel.
 * @param pwm The PWM channel.
 * @return The duty cycle of the specified PWM channel.
 * @warning Fails with program exit if pwm is outside valid range or if pwm has
 * not been initialized.
 */
extern uint32_t pwm_get_duty_cycle(const int pwm);

/**
 * @brief Generates steps steps on the PWM channel.
 * @param pwm The PWM channel.
 * @param steps The number of steps to cycle, 0 to turn off and -1 to run
 * continously.
 * @warning Fails with program exit if pwm is outside valid range or if pwm has
 * not been initialized.
 */
extern void pwm_set_steps(const int pwm, const uint32_t steps);

/**
 * @brief Get the number of steps a certain channel has taken so far
 * @param pwm PWM channel.
 * @return The number of steps that have been taken; 0 is off and -1 is
 * continous.
 * @warning Fails with program exit if pwm is outside valid range or if pwm has
 * not been initialized.
 */
extern uint32_t pwm_get_steps(const int pwm);

/**
 * @}
 */
#endif
