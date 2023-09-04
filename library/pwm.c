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

enum PWM_Regs {
  PWM_REG_DUTY = 0,
  PWM_REG_PERIOD = 1,
  PWM_REG_NEW_STEP_COUNT = 2,
  PWM_REG_CUR_STEP_COUNT = 3,
};

static struct arm_shared_t channels[NUM_PWMS] = {
    0,
};
static volatile uint32_t *initializedChannel[NUM_PWMS] = {
    NULL,
};

bool pwm_initialized(const int pwm) {
  if (pwm < 0 || pwm >= NUM_PWMS) {
    pynq_error("pwm_initialized: invalid pwm=%d, must be 0..%d-1\n", pwm,
               NUM_PWMS);
  }
  if (initializedChannel[pwm] == NULL) {
    return false;
  }
  return true;
}

bool check_initialized_pwm(const int pwm) {
  if (pwm < 0 || pwm >= NUM_PWMS) {
    pynq_error("pwm_initialized: invalid pwm=%d, must be 0..%d-1\n", pwm,
               NUM_PWMS);
  }
  if (initializedChannel[pwm] == NULL) {
    pynq_error("pwm_initialized: channel of pwm %d has not been initialized\n",
               pwm);
  }
  return true;
}

void pwm_init(const int pwm, const uint32_t period) {
  if (pwm < 0 || pwm >= NUM_PWMS) {
    pynq_error("pwm_init: invalid pwm=%d, must be 0..%d-1\n", pwm, NUM_PWMS);
  }
  uint32_t channelAddr = axi_pwm_base + (pwm * 0x10000);
  initializedChannel[pwm] = arm_shared_init(&channels[pwm], channelAddr, 512);
  initializedChannel[pwm][PWM_REG_DUTY] = 0;
  initializedChannel[pwm][PWM_REG_PERIOD] = period;
  initializedChannel[pwm][PWM_REG_NEW_STEP_COUNT] = -1;
}

void pwm_destroy(const int pwm) {
  (void)check_initialized_pwm(pwm);
  arm_shared_close(&channels[pwm]);
  initializedChannel[pwm] = NULL;
}

uint32_t pwm_get_duty_cycle(const int pwm) {
  (void)check_initialized_pwm(pwm);
  return initializedChannel[pwm][PWM_REG_DUTY];
}

uint32_t pwm_get_period(const int pwm) {
  (void)check_initialized_pwm(pwm);
  return initializedChannel[pwm][PWM_REG_PERIOD];
}

void pwm_set_period(const int pwm, const uint32_t period) {
  (void)check_initialized_pwm(pwm);
  initializedChannel[pwm][PWM_REG_PERIOD] = period;
}

void pwm_set_duty_cycle(const int pwm, const uint32_t duty) {
  (void)check_initialized_pwm(pwm);
  initializedChannel[pwm][PWM_REG_DUTY] = duty;
}

uint32_t pwm_get_steps(const int pwm) {
  (void)check_initialized_pwm(pwm);
  return initializedChannel[pwm][PWM_REG_NEW_STEP_COUNT];
}

void pwm_set_steps(const int pwm, const uint32_t steps) {
  (void)check_initialized_pwm(pwm);
  initializedChannel[pwm][PWM_REG_NEW_STEP_COUNT] = steps;
}
