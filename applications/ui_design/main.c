 /*
 *  TU/e 5WEC0::PYNQ Display UI system
 *
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>
#include <signal.h>

#include <libui.h>

int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);
  //ui_set_row(&ui, 0, "RYB 24", RGB_BLACK, RGB_ORANGE, 1);
  //ui_set_row(&ui, 1, "HeartBeat", RGB_BLACK, RGB_ORANGE, 1);
  //ui_set_row(&ui, 2, "Connection: Lost", RGB_RED, RGB_BLACK, 0);

  ui_printr(&ui, 2, 1, WORD("Hello"));
  ui_printr(&ui, 2, 4, WORD("Hello "), WORD("World"), WORD("Be "), WORD("Kind"));
  ui_printr(&ui, 2, 2, SWORD, BG_WORD("Hell ",RGB_ORANGE,RGB_RED));
  ui_printr(&ui, 0, 1, FULL_WORD("RYB 24",RGB_ORANGE,RGB_WHITE,NORMAL));
  //draw the ui;
  ui_draw(&ui);

  for (int i = 0; i < ui.rows[0].nr_words; i++)
  {
    printf("%s", ui.rows[0].words[i].text);
  }
  
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};