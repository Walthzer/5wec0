 /*
 *  TU/e 5WEC0::PYNQ RYB Communication Demonstration. Heartbeat module
 *
 *  Interface:
 *  Switch 1 -> Exits the main loop
 *  Switch 0: Toggles preset values
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
#include <time.h>
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
  ui_rprintf(&ui, 1, "%q%w%tHeartbeat", RGB_WHITE, RGB_ORANGE, COMIC);
  //interface layout
  //Outputs: Heartbeat
  ui_rprintf(&ui, 2, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 2, true);

  ui_draw(&ui);

  com_t com;
  com_init(&com, HEARTBEAT);

  //Init inputs
  switches_init();
  buttons_init();

  int state = 0;
  uint32_t value = 0;
  int button0 = get_button_state(0);
  int button1 = get_button_state(1);
  int do_com = 0;

  int v_counter = 0;
  time_t s_time = time(NULL);
  uint32_t values[10] = {-10,-34,48,70,150,260,93,350,10};
  while (get_switch_state(1))
  {
    if(get_switch_state(0))
    {
      ui_rprintf(&ui, 7, "%qMANUAL MODE     ", RGB_RED);
      //Dynamic Value
      state = get_button_state(0);
      if(state != button0)
      {
        value-= state + 10*get_button_state(2);
        button0 = state;
      }
      state = get_button_state(1);
      if(state != button1)
      {
        value+= state + 10*get_button_state(2);
        button1 = state;
      }
      //Reset counter
      if(get_button_state(3))
        value = 0;
    } else 
    {
      ui_rprintf(&ui, 7, "%qAUTOMATIC MODE", RGB_GREEN);
      if((time(NULL) - s_time ) > 0.1f)
      {
        v_counter++;
        v_counter = v_counter >= 10 ? 0 : v_counter;
        value = values[v_counter];
        s_time = time(NULL);
      }
    }

    uint32_t heartbeat = value;

    //outputs
    com_put(&com, HEARTBEAT, heartbeat);
    com_get(&com, HEARTBEAT, &heartbeat);
    
    ui_rprintf(&ui, 4, "%qValue: %q%d", RGB_PURPLE, RGB_GREEN, value);
    ui_rprintf(&ui, 5, "%qHearbeat[0-300]: %q%d", RGB_PURPLE, RGB_GREEN, heartbeat);

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