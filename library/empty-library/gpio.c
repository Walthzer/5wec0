#include <gpio.h>
void gpio_init(void){};
void gpio_destroy(void){};
void gpio_reset_pin(const gpio_t pin){};
void gpio_set_direction(const gpio_t pin, const gpio_direction_t direction){};
gpio_direction_t gpio_get_direction(const gpio_t pin){};
void gpio_set_level(const gpio_t pin, const gpio_level_t level){};
gpio_level_t gpio_get_level(const gpio_t pin){};
/*
void gpio_ack_interrupt(){};
void gpio_print_interrupt(){};
void gpio_enable_interrupt(const gpio_t ping){};
void gpio_disable_interrupt(const gpio_t ping){};
void gpio_disable_all_interrupts(){};
uint8_t *gpio_get_interrupt_pins(){};
uint64_t gpio_get_interrupt(){};
*/
void gpio_reset(void){};
