 /*
 *  TU/e 5WEC0::PYNQ RYB Module Testing. Decision module
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

//K9 to K1
//K9 starts at F5, A5.
//One Path
typedef struct _baby_model_
{
  Sector maze[5][5];
} Model;

typedef struct _sector_
{
  int class;
  int s_opp;
} Sector;

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
  //Outputs: Heartbeat+Crying
  ui_rprintf(&ui, 5, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 5, true);
  ui_draw(&ui);

  com_t com;
  com_init(&com, DECISION);

  //Init inputs
  switches_init();
  buttons_init();

  //init model
  Model model;
  for (int f = 0; f < 5; f++)
  {
    for (int a = 0; a < 5; a++)
    {
      model.maze[f][a] = (Sector){9, 100};
    }
  }
  

  while (get_switch_state(1))
  {
    //ADD in UI again.
  }

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};