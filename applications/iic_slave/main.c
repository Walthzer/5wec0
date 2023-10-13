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

  uint32_t buffer[1024] = {'W', 'O', 'R', 'K', 'S', '\0'};
  pn("IIC commands:\n");
  pn("  Setting to SLAVE: ");
  iic_set_slave_mode(iic_index, 0x56, buffer, 1024);
  pn("Done\n");
  pn("  Setting Handler: ");
  
  
  do_loop = true;
  while (do_loop)
  {
    buffer[0] = get_switch_state(SWITCH0);
    iic_slave_mode_handler(iic_index);
    //i++;
  }

  switches_destroy();
  
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