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

#define COM_MOTOR_LOW_B 2
#define COM_MOTOR_HIGH_B 10

int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);

  //Set header
  ui_rcenter(&ui, 0, true);
  ui_rprintf(&ui, 0, "%q%w%tRYB 24", RGB_WHITE, RGB_ORANGE, COMIC);
  ui_rcenter(&ui, 1, true);
  ui_rprintf(&ui, 1, "%q%w%tDecision Making", RGB_WHITE, RGB_ORANGE, COMIC);
  //interface layout
  //Inputs: Heartbeat+Crying
  ui_rprintf(&ui, 2, "%q--Inputs--", RGB_CYAN);
  ui_rcenter(&ui, 2, true);
  ui_rprintf(&ui, 3, "%qBPM:[0-300]:", RGB_BLUE);
  ui_rprintf(&ui, 4, "%qCrying:[0-100]:", RGB_BLUE);
  //Outputs: Heartbeat+Crying
  ui_rprintf(&ui, 5, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 5, true);
  ui_draw(&ui);

  com_t com;
  com_init(&com, DECISION);

  //Init inputs
  switches_init();
  buttons_init();

  int state = 0;
  uint32_t counters[] = {2,2};
  int button0 = get_button_state(0);
  int button1 = get_button_state(1);
  int do_com = 10;
  while (get_switch_state(1))
  {
    uint32_t data = 0;
    //inputs
    com_get(&com, HEARTBEAT, &data);
    ui_rprintf(&ui, 3, "%qBPM:[0-300]: %q%d", RGB_BLUE, RGB_RED, data);
    com_get(&com, CRYING, &data);
    ui_rprintf(&ui, 4, "%qCrying:[0-100]: %q%d", RGB_BLUE, RGB_RED, data);

    //Dynamic Values
    state = get_button_state(0);
    if(state != button0)
    {
      counters[get_switch_state(0)]-= state + 10*get_button_state(2);
      button0 = state;
    }
    state = get_button_state(1);
    if(state != button1)
    {
      counters[get_switch_state(0)]+= state + 10*get_button_state(2);
      button1 = state;
    }
    //Reset counter
    if(get_button_state(3))
      counters[get_switch_state(0)] = 1;

    //outputs
    uint16_t frequency = counters[0];
    uint16_t amplitude = counters[1];

    com_putm(&com, MOTOR, frequency, amplitude);
    com_getm(&com, MOTOR, &frequency, &amplitude);
    
    ui_rprintf(&ui, 6, "%qValue: %q%d", RGB_PURPLE, get_switch_state(0) ? RGB_WHITE : RGB_GREEN, counters[0]);
    ui_rprintf(&ui, 7, "%qFrequency[1-5]: %q%d", RGB_PURPLE, get_switch_state(0) ? RGB_WHITE : RGB_GREEN, frequency);
    
    ui_rprintf(&ui, 8, "%qValue: %q%d", RGB_BLUE, get_switch_state(0) ? RGB_GREEN : RGB_WHITE, counters[1]);
    ui_rprintf(&ui, 9, "%qAmplitude[1-5]: %q%d", RGB_BLUE, get_switch_state(0) ? RGB_GREEN : RGB_WHITE, amplitude);

    ui_draw(&ui);
    do_com++;
    if(do_com > 50)
    {
      com_run(&com);
      do_com = 0;
    }
  }

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};