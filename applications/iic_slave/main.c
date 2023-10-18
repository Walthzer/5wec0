 /*
 *  TU/e 5WEC0::PYNQ IIC Test Master
 *
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>
#include <time.h>
#include <stdlib.h>

int do_loop = false;
const int iic_index = 0;

void pn(char* data)
{
  printf("%s", data);
  fflush(NULL);
}

void do_iic_test(void)
{
  #ifndef IIC_H
    pn("No IIC lib present")
  #endif

  pn("Starting:\n");

  pn("  Switchbox: ");
  switchbox_init();
  pn("Done\n");
  pn("  Pinset: ");
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
  pn("Done\n");
  pn("  IIC: ");
  iic_init(iic_index);
  pn("Done\n");
  pn("Started!\n\n");

  switches_init();
  leds_init_onoff();

  uint32_t buffer[8] = {'W', 'O', 'R', 'K', 'S', '\0'};
  pn("IIC commands:\n");
  pn("  Setting to SLAVE: ");
  iic_set_slave_mode(iic_index, 0x56, buffer, 8);
  pn("Done\n");
  pn("  Setting Handler: ");
  
  unsigned int start, end;
  start = end = clock();
  int i = 0;
  do_loop = true;
  while (do_loop)
  {
    buffer[0] = i;
    iic_slave_mode_handler(iic_index);

    end = clock();
    if(get_switch_state(0) &&  ((end - start) / CLOCKS_PER_SEC * 6) > 0)
    {
      green_led_off(i);
      i = rand() % 4;
      start = clock();
      green_led_on(i);
    }
  }

  switches_destroy();
  leds_destroy();

  pn("Done\n");

  pn("Exiting: ");
  pn("  IIC destroy: ");
  iic_destroy(iic_index);
  pn("Done\n");
  pn("  Switchbox destroy: ");
  switchbox_destroy();
  pn("Done\n");
}

int main(void) {
  display_t display;
  pynq_init();

  //Initialize
  do_iic_test();

  pn("Bye!\n");

  pynq_destroy();

  return 0;
};