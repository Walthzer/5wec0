#include <leds.h>
void leds_init_onoff(void){};
void green_leds_init_pwm(void){};
void color_leds_init_pwm(void){};
void leds_destroy(void){};
void green_led_onoff(const int led, const int onoff){};
void green_led_on(const int led){};
void green_led_off(const int led){};
void color_led_red_onoff(const int onoff){};
void color_led_green_onoff(const int onoff){};
void color_led_blue_onoff(const int onoff){};
void color_led_onoff(const int red_onoff, const int green_onoff,
                            const int blue_onoff){};
void color_led_on(void){};
void color_led_off(void){};
