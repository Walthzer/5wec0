//#include <libpynq.h>
#include <stdio.h>

#ifndef PYNQLIB_H
  #define DISPLAY_WIDTH 240
#endif


int main(void) {
  #ifdef PYNQLIB_H
    pynq_init();
    display_t display;
    display_init(&display);
    display_set_flip(&display, true, false); //Display is mounted flipped on y-axis
    displayFillScreen(&display, RGB_BLACK);
  #endif

  //Initialize
  
  printf("%d\n", __CHAR_BIT__);
  printf("Bye!\n");

  #ifdef PYNQLIB_H
    display_destroy(&display);
    pynq_destroy();
  #endif

  return 0;
}
