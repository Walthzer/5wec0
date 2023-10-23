 /*
 *  TU/e 5WEC0::PYNQ IIC Test Slave
 *
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>

#define MAXSLAVES 2

int main(void) {
  pynq_init();

  //Initialize
  switchbox_init();
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);

  
  leds_init_onoff();
  switches_init();

  iic_init(0);
  color_led_blue_onoff(1);
  green_led_onoff(0, 0);
  green_led_onoff(1, 0);

  uint8_t buffer[8] = {0};
  while (get_switch_state(1))
  {
    for (size_t i = 0; i < MAXSLAVES; i++)
    {
      printf("Reading...%d\n", i);
      fflush(NULL);
      iic_read_register(0, 0x54 + i, 0, buffer, 1);
      green_led_onoff(i, buffer[0]);
      sleep_msec(1);
    } 
  }
  
  switches_destroy();
  leds_destroy();
  iic_destroy(0);
  //switchbox_destroy();
  pynq_destroy();

  return 0;
};