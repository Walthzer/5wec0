#include <display.h>
void display_init(display_t *display){};
void display_destroy(display_t *display){};
void displayDrawPixel(display_t *display, uint16_t x, uint16_t y,
                      uint16_t color){};
void displayDrawFillRect(display_t *display, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2, uint16_t color){};
void displayFillScreen(display_t *display, uint16_t color){};
void displayDrawLine(display_t *display, uint16_t x1, uint16_t y1, uint16_t x2,
                     uint16_t y2, uint16_t color){};
void displayDrawRect(display_t *display, uint16_t x1, uint16_t y1, uint16_t x2,
              uint16_t y2, uint16_t color){};
void displayDrawRectAngle(display_t *display, uint16_t xc, uint16_t yc,
                   uint16_t w, uint16_t h, uint16_t angle,
                   uint16_t color){};
void displayDrawTriangleCenter(display_t *display, uint16_t xc, uint16_t yc,
                        uint16_t w, uint16_t h, uint16_t angle,
                        uint16_t color){};
void displayDrawCircle(display_t *display, uint16_t x_center, uint16_t y_center,
                uint16_t r, uint16_t color){};
void displayDrawFillCircle(display_t *display, uint16_t x_center,
                    uint16_t y_center, uint16_t r, uint16_t color){};
void displayDrawRoundRect(display_t *display, uint16_t x1, uint16_t y1,
                   uint16_t x2, uint16_t y2, uint16_t r, uint16_t color){};
uint16_t rgb_conv(uint16_t r, uint16_t g, uint16_t b){};
int displayDrawChar(display_t *display, FontxFile *fx, uint16_t x, uint16_t y,
             uint8_t ascii, uint16_t color){};
int displayDrawString(display_t *display, FontxFile *fx, uint16_t x, uint16_t y,
                      uint8_t *ascii, uint16_t color){};
void displaySetFontDirection(display_t *display, uint16_t dir){};
void displaySetFontFill(display_t *display, uint16_t color){};
void displayUnsetFontFill(display_t *display){};
void displaySetFontUnderLine(display_t *display, uint16_t color){};
void displayUnsetFontUnderLine(display_t *display){};
void displayDisplayOff(display_t *display){};
void displayDisplayOn(display_t *display){};
void displayBacklightOff(display_t *display){};
void displayBacklightOn(display_t *display){};
void displayInversionOff(display_t *display){};
void displayInversionOn(display_t *display){};
void displayDrawTriangle(display_t *display, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3,
                         uint16_t color){};
