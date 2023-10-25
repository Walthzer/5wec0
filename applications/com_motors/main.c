 /*
 *  TU/e 5WEC0::PYNQ RYB Communication Demonstration. Motors Drivers module.
 *
 *  Interface:
 *  Switch 1 -> Exits the main loop
 *  Switch 0: Changes our value selection
 *  Button 3: Resets the value
 *  Button 2: Modify increase/decrease with a factor of ten;
 *  Button 1: Increases value
 *  Button 0: Decreases value
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
  
  while (get_switch_state(1))
  {
    uint16_t freq;
    uint16_t ampl;
    com_getm(&com, MOTOR, &freq, &ampl);

    ui_rprintf(&ui, 4, "%qFrequency: %q%d", RGB_BLUE, RGB_GREEN, freq);
    ui_rprintf(&ui, 5, "%qAmplitude: %q%d", RGB_BLUE, RGB_GREEN, ampl);

    ui_draw(&ui);
    com_run(&com);
  }
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};