#include <pwm.h>
bool pwm_initialized(const int pwm){};
void pwm_init(const int pwm, const uint32_t period){};
void pwm_destroy(const int pwm){};
void pwm_set_duty_cycle(const int pwm, const uint32_t duty){};
void pwm_set_period(const int pwm, const uint32_t period){};
uint32_t pwm_get_period(const int pwm){};
uint32_t pwm_get_duty_cycle(const int pwm){};
void pwm_set_steps(const int pwm, const uint32_t steps){};
uint32_t pwm_get_steps(const int pwm){};
