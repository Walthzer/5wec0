 /*
 *  TU/e 5WEC0::PYNQ RYB Communication Demonstration. Motors Drivers module.
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
  ui_rprintf(&ui, 0, "%q%w%tRYB 24", RGB_WHITE, RGB_ORANGE, COMIC);
  ui_rcenter(&ui, 1, true);
  ui_rprintf(&ui, 1, "%q%w%tMotors Drivers", RGB_WHITE, RGB_ORANGE, COMIC);
  //interface layout
  ui_rprintf(&ui, 2, "%qRanges: [1-5]", RGB_GREEN);
  ui_rprintf(&ui, 4, "%qFrequency: %q??", RGB_BLUE, RGB_RED);
  ui_rprintf(&ui, 5, "%qAmplitude: %q??", RGB_BLUE, RGB_RED);

  ui_draw(&ui);

  com_t com;
  com_init(&com, MOTOR);

  //Init inputs
  switches_init();
  buttons_init();

  int state = 0;
  int counter = 0;
  int button0 = get_button_state(0);
  int button1 = get_button_state(1);
  while (get_switch_state(1))
  {
    //Dynamic Values
    state = get_button_state(0);
    if(state != button0)
    {
      counter-= state + 10*get_button_state(2);
      button0 = state;
    }
    state = get_button_state(1);
    if(state != button1)
    {
      counter+= state + 10*get_button_state(2);
      button1 = state;
    }
    //Reset counter
    if(get_button_state(3))
      counter = 0;

    uint32_t data = 0;
    bool is_new = com_get(&com, MOTOR, &data);

    ui_rprintf(&ui, 4, "%qFrequency: %q%d", RGB_BLUE, RGB_RED, get_switch_state(0));
    ui_rprintf(&ui, 5, "%w%qAmplitude: %w%d", RGB_BLACK, RGB_BLUE, RGB_GREEN, counter);

    ui_draw(&ui);
    com_run(&com);
  }

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};