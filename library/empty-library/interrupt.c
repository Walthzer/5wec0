#include <interrupt.h>
int gpio_interrupt_init(void){};
void gpio_ack_interrupt(void){};
void verify_interrupt_request(const gpio_t pin){};
void gpio_print_interrupt(void){};
void gpio_enable_interrupt(const gpio_t pin){};
void gpio_disable_interrupt(const gpio_t pin){};
void gpio_disable_all_interrupts(void){};
uint64_t gpio_get_interrupt(void){};
uint8_t *gpio_get_interrupt_pins(uint8_t *positions){};
void gpio_wait_for_interrupt(const gpio_t pin){};
