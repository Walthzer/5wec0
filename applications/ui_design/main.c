#include <libpynq.h>

int main(void) {
  pynq_init();
  display_t display;
  display_init(&display);
  displayFillScreen(&display, RGB_BLACK);

  //Initialize
  int unused[128]; //<= removing this 'useless' line creates STACK-SMASHING
  uint8_t string[] = "RYB 24";

  FontxFile font;
  InitFontx(&font, "../../fonts/ILGH16XB.FNT", "../../fonts/ILGH24XB.FNT");

  //Print some text centered
  displayDrawString(&display, &font, DISPLAY_WIDTH/2 - (strlen(string) * 6), 24, (uint8_t*)"RYB 24", rgb_conv(255, 165, 0));

  display_destroy(&display);
  pynq_destroy();

  return 0;
};