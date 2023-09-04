#include <buttons.h>
void switches_init(void){};
void switches_destroy(void){};
extern void buttons_init(void){};
extern void buttons_destroy(void){};
extern int get_button_state(const int button){};
extern int wait_until_button_state(const int button, const int state){return 0;};
extern int sleep_msec_button_pushed(const int button, const int msec){return 0;};
extern void sleep_msec_buttons_pushed(int button_states[], const int ms){};
extern int wait_until_button_pushed(const int button){return 0;};
extern int wait_until_button_released(const int button){return 0;};
extern int wait_until_any_button_pushed(void){return 0;};
extern int wait_until_any_button_released(void){return 0;};
extern int get_switch_state(const int switch_num){return 0;};
