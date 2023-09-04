#include <libpynq.h>
#include <time.h>

#define TESTCASE_DELAY_MS 200

/*
    !i! REMEMBER TO SET TO 0 FOR CI RUNNER !i!
*/
#define ERRORS 0

typedef struct {
  int x;
  int y;
} coordinate;

void overviewShapes(display_t *display) {
  int yStart = 75;
  printf("Printing shape catalogue\t Page 1 of 2\n");
  displayFillScreen(display, RGB_WHITE);
  FontxFile fx32L[2];
  InitFontx(fx32L, "../../fonts/ILGH16XB.FNT", "");
  displaySetFontDirection(display, TEXT_DIRECTION90);

  uint8_t ascii_text_header[] = {"Shape        Function"};
  displayDrawString(display, fx32L, 205, 20, ascii_text_header, RGB_BLACK);

  uint8_t ascii_text_fillrectangle[] = {"displayDrawFillRect"};
  displayDrawString(display, fx32L, 180, yStart, ascii_text_fillrectangle,
                    RGB_BLACK);
  displayDrawFillRect(display, 180, 10, 200, 70, RGB_RED);

  uint8_t ascii_text_rectangle[] = {"displayDrawRect"};
  displayDrawString(display, fx32L, 150, yStart, ascii_text_rectangle,
                    RGB_BLACK);
  displayDrawRect(display, 150, 10, 170, 70, RGB_RED);

  uint8_t ascii_text_rectAngle[] = {"displayDrawRectAngle"};
  displayDrawString(display, fx32L, 120, yStart, ascii_text_rectAngle,
                    RGB_BLACK);
  displayDrawRectAngle(display, 120, 40, 20, 60, 15, RGB_RED);

  uint8_t ascii_text_triangle[] = {"displayDrawTriangle"};
  uint8_t ascii_text_triangle_center[] = {"Center"};
  displayDrawString(display, fx32L, 88, yStart, ascii_text_triangle, RGB_BLACK);
  displayDrawString(display, fx32L, 75, yStart, ascii_text_triangle_center,
                    RGB_BLACK);
  displayDrawTriangleCenter(display, 90, 40, 20, 60, 20, RGB_RED);

  uint8_t ascii_text_circle[] = {"displayDrawCircle"};
  displayDrawString(display, fx32L, 50, yStart, ascii_text_circle, RGB_BLACK);
  displayDrawCircle(display, 60, 40, 15, RGB_RED);

  uint8_t ascii_text_fullcircle[] = {"displayDrawFill"};
  uint8_t ascii_text_fullcircle_circle[] = {"Circle"};
  displayDrawString(display, fx32L, 30, yStart, ascii_text_fullcircle,
                    RGB_BLACK);
  displayDrawString(display, fx32L, 17, yStart, ascii_text_fullcircle_circle,
                    RGB_BLACK);
  displayDrawFillCircle(display, 25, 40, 15, RGB_RED);

  sleep_msec(1000);
  printf("Printing shape catalogue\t Page 2 of 2\n");
  displayDrawFillRect(display, 0, 0, 200, 239, RGB_WHITE);

  uint8_t ascii_text_roundrectangle[] = {"displayDrawRoundRect"};
  displayDrawString(display, fx32L, 180, yStart, ascii_text_roundrectangle,
                    RGB_BLACK);
  displayDrawRoundRect(display, 180, 10, 200, 70, 10, RGB_RED);

  sleep_msec(1000);
}

void powerCycledisplay(display_t *display) {
  printf("Powercycling display\n");
  displayDisplayOff(display);
  sleep_msec(500);
  displayDisplayOn(display);
  sleep_msec(500);
}

void fillColorsdisplay(display_t *display) {
  printf("Testing colors\n");
  for (int k = 150; k < 300; k += 50) {
    for (int r = 0; r < 255; r += 9) {
      int g = r + k % 255;
      int b = r + k * 2 % 255;
      displayFillScreen(display, rgb_conv(r, g, b));
    }
  }
}

void directionsText(display_t *display, FontxFile (*font)[2]) {
  printf("Drawing some text with different orientations\n");
  displayFillScreen(display, RGB_WHITE);
  displaySetFontDirection(display, TEXT_DIRECTION0);

  displayDrawFillRect(display, 10, 10, 20, 20, RGB_RED);
  displayDrawFillRect(display, 10, 210, 20, 220, RGB_GREEN);
  displayDrawFillRect(display, 210, 10, 220, 20, RGB_BLUE);
  displayDrawFillRect(display, 210, 210, 220, 220, RGB_BLACK);

  uint8_t *s0 = (uint8_t *)"direction 0";
  uint8_t *s90 = (uint8_t *)"direction 90";
  uint8_t *s180 = (uint8_t *)"direction 180";
  uint8_t *s270 = (uint8_t *)"direction 270";

  displayDrawString(display, font[0], DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, s0,
                    RGB_BLACK);
  displaySetFontDirection(display, TEXT_DIRECTION90);
  displayDrawString(display, font[0], DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2,
                    s90, RGB_BLACK);
  displaySetFontDirection(display, TEXT_DIRECTION180);
  displayDrawString(display, font[0], DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2,
                    s180, RGB_BLACK);
  displaySetFontDirection(display, TEXT_DIRECTION270);
  displayDrawString(display, font[0], DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2,
                    s270, RGB_BLACK);

  if (ERRORS) {
    displayFillScreen(display, RGB_WHITE);
    displaySetFontDirection(display, TEXT_DIRECTION270);
    uint8_t *errorText = (uint8_t *)"1_2_3_4_5_6_7_8";
    displayDrawString(display, font[0], DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2,
                      errorText, RGB_BLACK);
  }

  sleep_msec(1000);
}

void drawCircles(display_t *display) {
  printf("Drawing circles without fill\n");
  int mr = 10;
  for (int i = 19; i < 250; i++) {
    uint16_t r = mr + (rand() % mr);
    uint16_t x = r + (rand() % (DISPLAY_WIDTH - 2 * r));
    uint16_t y = r + (rand() % (DISPLAY_HEIGHT - 2 * r));
    displayDrawCircle(display, x, y, r, rgb_conv(r, x, y));
  }
}

void drawCirclesFilled(display_t *display) {
  printf("Drawing circles with fill\n");
  displayFillScreen(display, RGB_WHITE);
  int mr = 10;
  for (int i = 0; i < 100; i++) {
    uint16_t r = mr + (rand() % mr);
    uint16_t x = r + (rand() % (DISPLAY_WIDTH - 2 * r));
    uint16_t y = r + (rand() % (DISPLAY_HEIGHT - 2 * r));
    displayDrawFillCircle(display, x, y, r, rgb_conv(x, r, y));
  }
}

void textGallery(display_t *display, FontxFile Fonts[7][2]) {
  printf("Drawing different fonts on the display\n");
  displaySetFontDirection(display, TEXT_DIRECTION90);
  displayFillScreen(display, RGB_WHITE);
  uint8_t ascii_font[] = {"TU Eindhoven"};
  int fontCounter = 0;
  int scalar = 25;
  for (int x = 209; x > 20; x -= scalar) {
    if (x < 200) {
      scalar = 35;
    } else if (x < 190) {
      scalar = 25;
    }
    displayDrawString(display, Fonts[fontCounter], x, 30, ascii_font, RGB_RED);
    fontCounter++;
  }
  sleep_msec(2000);
}

void textGalleryUnderlined(display_t *display, FontxFile Fonts[7][2]) {
  printf("Drawing fonts with underline\n");
  displaySetFontDirection(display, TEXT_DIRECTION90);
  displaySetFontUnderLine(display, RGB_RED);
  displayFillScreen(display, RGB_WHITE);
  uint8_t ascii_font[] = {"TU Eindhoven"};
  int fontCounter = 0;
  int scalar = 25;
  for (int x = 209; x > 20; x -= scalar) {
    if (x < 200) {
      scalar = 35;
    } else if (x < 190) {
      scalar = 25;
    }
    displayDrawString(display, Fonts[fontCounter], x, 30, ascii_font, RGB_RED);
    fontCounter++;
  }
  displayUnsetFontUnderLine(display);
  sleep_msec(2000);
}

void drawPixels(display_t *display) {
  printf("Drawing some pixels\n");
  for (int i = 0; i < 3000; i++) {
    displayDrawPixel(display, rand() % DISPLAY_WIDTH, rand() % DISPLAY_HEIGHT,
                     RGB_BLUE);
  }
  displayFillScreen(display, RGB_WHITE);
}

void drawTestRect(display_t *display) {
  printf("Drawing some rectangles\n");
  displayFillScreen(display, RGB_WHITE);
  for (int i = 0; i < 50; i++) {
    uint16_t x1 = rand() % (DISPLAY_WIDTH - 1);
    uint16_t y1 = rand() % (DISPLAY_HEIGHT - 1);
    uint16_t x2 = x1 + 1 + rand() % (DISPLAY_WIDTH - x1 - 1);
    uint16_t y2 = y1 + 1 + rand() % (DISPLAY_WIDTH - y1 - 1);
    displayDrawRect(display, x1, y1, x2, y2, rgb_conv(x1, x2, y1));
  }
}

void drawTestFilledRect(display_t *display) {
  printf("Drawing some filled rectangles\n");
  displayFillScreen(display, RGB_WHITE);
  int width = 20, height = 20;
  for (int y = 0; y < DISPLAY_HEIGHT; y += height) {
    for (int x = 0; x < DISPLAY_WIDTH; x += width) {
      displayDrawFillRect(display, x, y, x + width - 1, y + height - 1,
                          rgb_conv(x, y, (x * y) % 255));
      sleep_msec(50);
    }
  }
}

void testDrawLine(display_t *display) {
  printf("Testing displayDrawLine()\n");
  printf("No errors should be thrown\n");
  for (int x1 = 0; x1 < display->_width; x1++) {
    int x2 = display->_height - 1 - x1;
    int y1 = 0, y2 = display->_width - 1;
    displayDrawLine(display, x1, y1, x2, y2, rgb_conv(x1, x2, y1));
    sleep_msec(TESTCASE_DELAY_MS / 200);
  }
  for (int y1 = 0; y1 < display->_width; y1++) {
    int y2 = display->_height - 1 - y1;
    int x1 = 0, x2 = display->_width - 1;
    displayDrawLine(display, x1, y1, x2, y2, rgb_conv(x1, x2, y1));
    sleep_msec(TESTCASE_DELAY_MS / 200);
  }
  if (ERRORS) {
    printf("An error should be thrown\n");
    displayDrawLine(display, 0, 0, 240, 239, RGB_RED);
  }
}

void testFilledRect(display_t *display) {
  printf("Testing displayDrawFillRect()\n");
  printf("Screen should turn RED\n");
  displayDrawFillRect(display, 0, 0, 239, 239, RGB_RED);
  sleep_msec(TESTCASE_DELAY_MS);
  printf("Screen should turn GREEN\n");
  displayDrawFillRect(display, 0, 239, 239, 0, RGB_GREEN);
  sleep_msec(TESTCASE_DELAY_MS);
  printf("Screen should turn BLUE\n");
  displayDrawFillRect(display, 239, 0, 0, 239, RGB_BLUE);
  sleep_msec(TESTCASE_DELAY_MS);
  printf("Screen should turn YELLOW\n");
  displayDrawFillRect(display, 239, 239, 0, 0, RGB_YELLOW);
  sleep_msec(TESTCASE_DELAY_MS);
  if (ERRORS) {
    printf("Program should throw error\n");
    displayDrawFillRect(display, 240, 240, 0, 0, RGB_YELLOW);
  }
}

void testDisplayDrawRect(display_t *display) {
  printf("Testing displayDrawRect()\n");
  printf("Screen should turn WHITE\n");
  for (int x1 = 0; x1 < (display->_width / 2); x1++) {
    int x2 = display->_width - x1 - 1;
    int y1 = x1;
    int y2 = x2;
    displayDrawRect(display, x1, y1, x2, y2, RGB_WHITE);
  }
  if (ERRORS) {
    printf("Program should throw error\n");
    displayDrawRect(display, 0, 0, 240, 239, RGB_WHITE);
  }
}

void testDrawCircle(display_t *display) {
  printf("Testing drawCircle()\n");
  int x = display->_width / 2;
  for (int r = (display->_width / 2) - 1; r > 0; r--) {
    displayDrawCircle(display, x, x, r, rgb_conv(x / 10, r, x * r));
  }
  if (ERRORS) {
    printf("Program should throw error\n");
    displayDrawCircle(display, x, x, 120, RGB_WHITE);
  }
}

void testDrawFillCircle(display_t *display) {
  printf("Testing displayDrawFillCircle()\n");
  int x = display->_width / 2;
  for (int r = (display->_width / 2) - 1; r > 0; r -= 80) {
    displayDrawFillCircle(display, x, x, r, rgb_conv(x / 10, r, x * r));
  }
  if (ERRORS) {
    printf("Program should throw error\n");
    displayDrawFillCircle(display, x, x, 120, RGB_WHITE);
  }
}

void testDrawTriangleCenter(display_t *display) {
  printf("Testing displayDrawTriangleCenter()\n");
  int x = display->_width / 2;
  for (int angle = 10; angle < 240; angle += 4) {
    displayDrawTriangleCenter(display, x, x, angle / 2, angle / 2, angle,
                              rgb_conv(angle, x * angle, x / angle));
  }
  if (ERRORS) {
    printf("Program should throw error\n");
    displayDrawTriangleCenter(display, x, x, 240, 120, 0, RGB_WHITE);
  }
}

void testDrawRoundRect(display_t *display) {
  printf("Testing displayDrawRoundRect()\n");
  printf("Screen should turn RED\n");
  for (int x1 = 0; x1 < (display->_width / 2); x1++) {
    int x2 = display->_width - x1 - 1;
    int y1 = x1;
    int y2 = x2;
    displayDrawRoundRect(display, x1, y1, x2, y2, 10, RGB_RED);
  }
  if (ERRORS) {
    printf("Program should throw error\n");
    displayDrawRoundRect(display, 0, 0, 240, 239, 10, RGB_RED);
  }
}

int main(void) {
  pynq_init();
  srand(time(NULL));

  display_t display;
  display_init(&display);

  // Testing all edge-cases for functions and throwing an error for screen lib
  // IF I FORGOT SOME, PLEASE ADD THEM!
  testFilledRect(&display);
  testDrawLine(&display);
  testDisplayDrawRect(&display);
  testDrawCircle(&display);
  testDrawFillCircle(&display);
  testDrawTriangleCenter(&display);
  testDrawRoundRect(&display);

  FontxFile Fonts[7][2];
  InitFontx(Fonts[0], "../../fonts/ILGH16XB.FNT", "");
  InitFontx(Fonts[1], "../../fonts/ILMH16XB.FNT", "");
  InitFontx(Fonts[2], "../../fonts/ILGH24XB.FNT", "");
  InitFontx(Fonts[3], "../../fonts/ILMH24XB.FNT", "");
  InitFontx(Fonts[4], "../../fonts/ILMH32XB.FNT", "");
  InitFontx(Fonts[5], "../../fonts/LATIN32B.FNT", "");

  int i = 0;

  while (1) {
    displayFillScreen(&display, RGB_WHITE);
    powerCycledisplay(&display);
    overviewShapes(&display);
    textGallery(&display, Fonts);
    fillColorsdisplay(&display);
    textGalleryUnderlined(&display, Fonts);
    displayFillScreen(&display, RGB_WHITE);
    drawCircles(&display);
    drawCirclesFilled(&display);
    directionsText(&display, &Fonts[0]);
    drawPixels(&display);
    drawTestRect(&display);
    drawTestFilledRect(&display);
    if (i % 2 == 0) {
      displayInversionOn(&display);
    } else {
      displayInversionOff(&display);
    }
    i++;
  }

  // So that the ci-runner doesn't error
  testFilledRect(&display);
  testDrawLine(&display);

  display_destroy(&display);
  pynq_destroy();
  return EXIT_SUCCESS;
}
