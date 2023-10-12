#include <libpynq.h>

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
  const uint8_t slave_addr = 100;

  pn("  Switchbox: ");
  switchbox_init();
  pn("Done\n");
  pn("  Pinset: ");
  switchbox_set_pin((pin_t)SWB_IIC0_SCL, SWB_RBPI12);
  switchbox_set_pin((pin_t)SWB_IIC0_SDA, SWB_RBPI13);
  pn("Done\n");
  pn("  IIC: ");
  iic_init(0);
  pn("Done\n");
  pn("Started!\n\n");


  char* byteBuffer = "Hello World";
  uint16_t data_length = 11;
  pn("IIC commands:\n");
  pn("  writing reg: ");
  iic_write_register(0, slave_addr, 0, (uint8_t*)byteBuffer, data_length);
  pn("Done\n");
  byteBuffer = "OVERWRITE THIS PLEASE";
  pn("  reading reg: ");
  iic_read_register(0, slave_addr, 0, (uint8_t*)byteBuffer, data_length);
  pn("Done\n");

  pn("Exiting: ");
  pn("  IIC destroy: ");
  iic_destroy(0);
  pn("Done\n");
  pn("  Switchbox destroy: ");
  switchbox_destroy();
  pn("Done\n");
}

int main(void) {
  display_t display;
  #ifdef PYNQLIB_H
    pynq_init();
    display_init(&display);
    display_set_flip(&display, true, true); //Display is mounted flipped on y-axis
    displayFillScreen(&display, RGB_BLACK);
  #endif

  //Initialize
  do_iic_test();

  pn("Bye!\n");

  #ifdef PYNQLIB_H
    display_destroy(&display);
    pynq_destroy();
  #endif

  return 0;
};