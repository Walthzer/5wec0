 /*
 *  TU/e 5WEC0::PYNQ IIC Test Master
 *
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>
#include <stdlib.h>
#include <libui.h>
#include <libcom.h>

#define MAXSLAVES 3

int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);

  //Set header
  ui_rcenter(&ui, 0, true);
  ui_rprintf(&ui, 0, "%t%q%wRYB 24", COMIC, RGB_WHITE, RGB_ORANGE);
  ui_rprintf(&ui, 0, "%q%wIIC Slave ???", RGB_WHITE, RGB_ORANGE);

  //Initialize switchbox
  switchbox_init();
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);


  //ID setup
  int id = 0;
  printf("Slave ID? ");
  scanf(" %d", &id);
  if(id > MAXSLAVES - 1) id = MAXSLAVES - 1;
  if(id < 0) id = 0;
  printf("Using ID: %d  Address: %x\n", id, 0x54 + id);
  fflush(NULL);

  ui_rprintf(&ui, 0, "%q%wIIC Slave %d", RGB_WHITE, RGB_ORANGE, id);

  //Init IIC
  iic_init(0);
  uint32_t buffer[8] = {0};
  iic_set_slave_mode(0, 0x54 + id, buffer, 8);

  //Init UI
  switches_init();
  leds_init_onoff();

  while (get_switch_state(1))
  {
    buffer[0] = get_switch_state(0);
    green_led_onoff(id, buffer[0]);
    iic_slave_mode_handler(0);
  }

  iic_destroy(0);

  switches_destroy();
  leds_destroy();
  //switchbox_destroy();
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};