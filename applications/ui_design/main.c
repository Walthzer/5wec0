 /*
 *  TU/e 5WEC0::PYNQ Display UI system
 *
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>
#include <libui.h>
#include <stdlib.h>

int main(void) {
  #ifdef PYNQLIB_H
  pynq_init();
  #endif
  ui_t ui;
  ui_init(&ui);

  //ui_rprintf(&ui, 0, "%qT%qe%qs%qt", RGB_ORANGE, RGB_RED, RGB_BLUE, RGB_YELLOW);
  ui_rclear(&ui, 0);
  ui_rcenter(&ui, 0, 1);
  ui_rprintf(&ui, 0, "%q%wHello %q%tWorld", RGB_ORANGE, RGB_GREEN, RGB_WHITE, COMIC);
  ui_rprintf(&ui, 1, "%w%q%s%d", RGB_BLACK, RGB_GREEN, "RYB ", 24);

  //draw the ui;
  ui_draw(&ui);
  
  ui_destroy(&ui);
  #ifdef PYNQLIB_H
  pynq_destroy();
  #endif
  
  return 0;
};