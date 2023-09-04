#include <libpynq.h>
#include <stdio.h>

int main(void) {
  pynq_init();

  // set up screen
  display_t display;
  display_init(&display);
  displayFillScreen(&display, RGB_BLACK);

  uint8_t buffer_fx16G[FontxGlyphBufSize];
  uint8_t fontWidth_fx16G, fontHeight_fx16G;
  FontxFile fx16G[2];
  InitFontx(fx16G, "/boot/ILGH16XB.FNT", "");
  GetFontx(fx16G, 0, buffer_fx16G, &fontWidth_fx16G, &fontHeight_fx16G);

  char *lineptr = NULL;
  size_t line_size = 0;
  size_t i = 0;
  ssize_t x = 0;
  do {
    printf("text? ");
    if (getline(&lineptr, &line_size, stdin) < 1)
      break;
    int y = (fontHeight_fx16G * (i + 1));

    if (y > (DISPLAY_HEIGHT - fontHeight_fx16G)) {
      break;
    }
    if (*lineptr == '\a') {
      displayDrawLine(&display, 1, y - fontHeight_fx16G / 2, DISPLAY_WIDTH-2,
                      y - fontHeight_fx16G / 2, RGB_WHITE);
      i++;
    } else if (*lineptr == '\x0e') {
      x += fontHeight_fx16G;
      if (x > 120)
        x = 120;
    } else if (*lineptr == '\x0f') {
      x -= fontHeight_fx16G;
      if (x < 0)
        x = 0;
    } else {
      char *str = lineptr;
      displayDrawString(&display, fx16G, x, y, (uint8_t *)str, RGB_GREEN);
      i++;
    }
  } while (true);
  free(lineptr);

  display_destroy(&display);
  pynq_destroy();
  return EXIT_SUCCESS;
}
