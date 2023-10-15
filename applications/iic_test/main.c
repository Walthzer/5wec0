 /*
 *  TU/e 5WEC0::PYNQ IIC Test Slave
 *
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>
#include <signal.h>

int do_loop = false;
const int iic_index = 0;

void pn(char* data)
{
  printf("%s", data);
  fflush(NULL);
}

void exit_loop(int signal)
{
  do_loop = false;
}

void do_iic_test(void)
{
  #ifndef IIC_H
    pn("No IIC lib present")
  #endif

  pn("Starting:\n");
  const uint8_t slave_addr = 100;

  pn("  Switchbox: ");
  switchbox_init();
  pn("Done\n");
  pn("  Pinset: ");
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
  switchbox_set_pin(IO_PMODA1, SWB_GPIO);

  gpio_init();
  gpio_set_direction(IO_PMODA1, GPIO_DIR_INPUT);
  gpio_set_level(IO_PMODA1, GPIO_LEVEL_LOW);

  pn("Done\n");
  pn("  IIC: ");
  iic_init(iic_index);
  pn("Done\n");
  pn("Started!\n\n");

  leds_init_onoff();

  uint8_t byteBuffer[1024] = {0};
  uint16_t data_length = 5;
  pn("IIC commands:\n");
  pn("  reading reg: ");

  do_loop = true;
  while (do_loop)
  {
    iic_read_register(iic_index, 0x56, 0, &byteBuffer[0], 1);
    green_led_onoff(0, byteBuffer[0]);
  }
  
  gpio_destroy();
  leds_destroy();

  pn("Done\n");

  printf("%s\n", byteBuffer);

  pn("Exiting: ");
  pn("  IIC destroy: ");
  iic_destroy(iic_index);
  pn("Done\n");
  pn("  Switchbox destroy: ");
  switchbox_destroy();
  pn("Done\n");
}

int main(void) {
  pynq_init();

  //Initialize
  do_iic_test();

  pn("Bye!A\n");

  pynq_destroy();

  return 0;
};