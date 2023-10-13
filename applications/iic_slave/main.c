#include <libpynq.h>
#include <signal.h>

int do_loop = false;

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
  pn("Done\n");
  pn("  IIC: ");
  iic_init(0);
  pn("Done\n");
  pn("Started!\n\n");

  switches_init();

  uint32_t buffer[1024] = {'W', 'O', 'R', 'K', 'S', '\0'};
  pn("IIC commands:\n");
  pn("  Setting to SLAVE: ");
  iic_set_slave_mode(0, 0x56, buffer, 1024);
  pn("Done\n");
  pn("  Setting Handler: ");
  
  

  uint32_t i = 0;
  uint32_t display[1024] = {'\0'};
  do_loop = true;
  signal(SIGINT, &exit_loop);
  while (do_loop)
  {
    buffer[0] = get_switch_state(SWITCH0);
    iic_slave_mode_handler(0);
    //i++;
  }

  switches_destroy();
  
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
  pynq_init();

  //Initialize
  do_iic_test();

  pn("Bye!\n");

  pynq_destroy();

  return 0;
};