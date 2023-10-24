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

int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);

  //Set header
  ui_rcenter(&ui, 0, true);
  ui_rcenter(&ui, 1, true);
  ui_rprintf(&ui, 0, "%w%t%qRYB 24", RGB_ORANGE, COMIC, RGB_WHITE);
  ui_rprintf(&ui, 1, "%q%wIIC Slave ???", RGB_WHITE, RGB_ORANGE);
  
  ui_draw(&ui);

  //ID setup
  int label = 1;
  printf("Slave ID? ");
  scanf(" %d", &label);
  printf("Using ID: %d  Address: %x\n", label, COM_BASE_ADRESS + label);
  fflush(NULL);

  ui_rprintf(&ui, 1, "%q%wIIC Slave %d", RGB_WHITE, RGB_ORANGE, label);

  com_t com;
  com_init(&com, label);

  //Init UI
  switches_init();
  uint32_t state = 0;
  printf("Starting loop\n");
  fflush(NULL);
  while (get_switch_state(1))
  {
    state = get_switch_state(0);
    com_put(&com, label, state);
    
    ui_rprintf(&ui, 2, "%q%wSending: %q%d", RGB_WHITE, RGB_BLACK, RGB_GREEN, state);
    ui_draw(&ui);
    com_run(&com);
  }

  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};