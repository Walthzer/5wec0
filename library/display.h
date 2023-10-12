/*
MIT License

Copyright (c) 2020

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by Eindhoven University of Technology 2023.
*/
#ifndef SCREEN_H
#define SCREEN_H

#include <fontx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * @defgroup DISPLAY Display library
 *
 * @brief Wrappers to simplify the use of the TFT LCD
 * display.
 *
 * Define a display_t display (called the display "handle"), initialise it,
 * and pass this as the first parameter to all functions.
 *
 * @warning All functions fail with program exit if any pixel of the shape
 * that is drawn is outside the display dimensions.
 *
 * An example of how to use this library.
 * @code
 * #include <libpynq.h>
 * int main (void)
 * {
 *   // initialise all I/O
 *   pynq_init();
 *   display_t display;
 *   display_init(&display);
 *
 *   displayFillScreen(&display, RGB_RED);
 *   // drawing is simple
 *   displayDrawPixel(&display, 50, 50, RGB_YELLOW);
 *   displayDrawFillRect(&display, 10, 100, 110, 200, RGB_RED);
 *   displayDrawCircle(&display, 60, 40, 15, RGB_RED);
 *   // text is more involved
 *   FontxFile fx16G[2];
 *   // the font file must be reachable from the directory
 *   // from which the executable is run -- see InitFontx
 *   InitFontx(fx16G, "../../fonts/ILGH16XB.FNT", "");
 *   GetFontx(fx16G, 0, buffer_fx16G, &fontWidth_fx16G, &fontHeight_fx16G);
 *   displaySetFontDirection(&display, TEXT_DIRECTION0);
 *   uint8_t text[] = "hello";
 *   displayDrawString(&display, fx16G, 15, fontHeight_fx16G * 6, text1,
 * RGB_WHITE);
 *
 *   // clean up after use
 *   display_destroy(&display);
 *   pynq_destroy();
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * @{
 */

#define DISPLAY_HEIGHT 240
#define DISPLAY_WIDTH 240

/**
 * @brief Colors that can be used with the display.
 */
enum colors {
  RGB_RED = 0xf800,
  RGB_GREEN = 0x07e0,
  RGB_BLUE = 0x001f,
  RGB_BLACK = 0x0000,
  RGB_WHITE = 0xffff,
  RGB_GRAY = 0x8c51,
  RGB_YELLOW = 0xFFE0,
  RGB_CYAN = 0x07FF,
  RGB_PURPLE = 0xF81F
};

/**
 * @brief Enum of directions the text can be printed on on the display.
 */
enum directions {
  TEXT_DIRECTION0 = 0,
  TEXT_DIRECTION90 = 1,
  TEXT_DIRECTION180 = 2,
  TEXT_DIRECTION270 = 3,
  NUM_TEXT_DIRECTIONS
};

/**
 * @brief Internal type, do not use. Type of display that stores parameters for
 * usage in different functions.
 */
typedef struct {
  uint16_t _width;
  uint16_t _height;
  uint16_t _offsetx;
  uint16_t _offsety;
  uint16_t _font_direction;
  uint16_t _font_fill;
  uint16_t _font_fill_color;
  uint16_t _font_underline;
  uint16_t _font_underline_color;
  int16_t _dc;
  int16_t _bl;
} display_t;

/**
 * @brief Initialize the display display.
 * @param display Handle to display.
 */
extern void display_init(display_t *display);

/**
 * @brief Stop using the display.
 * @param display Handle to display.
 */
extern void display_destroy(display_t *display);

/**
 * @brief Draw a single pixel to the display.
 * @param display Handle to display.
 * @param x The X coordinate of the pixel.
 * @param y The Y coordinate of the pixel.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawPixel(display_t *display, uint16_t x, uint16_t y,
                             uint16_t color);

/**
 * @brief Draw a filled rectangle to the display.
 * @param display Handle to display.
 * @param x1 The X coordinate of the top-left corner of the rectangle.
 * @param y1 The Y coordinate of the top-left corner of the rectangle.
 * @param x2 The X coordinate of the bottom-right corner of the rectangle.
 * @param y2 The Y coordinate of the bottom-right corner of the rectangle.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawFillRect(display_t *display, uint16_t x1, uint16_t y1,
                                uint16_t x2, uint16_t y2, uint16_t color);

/**
 * @brief Fill entire display with a single color using the ldcDrawFillRect
 * function.
 * @param display Handle to display.
 * @param color Fill color in RGB format.
 */
extern void displayFillScreen(display_t *display, uint16_t color);

/**
 * @brief Draw a line from two coordinates.
 * @param display Handle to display.
 * @param x1 Starting x-coordinate of line.
 * @param y1 Starting y-coordinate of line.
 * @param x2 Ending x-coordinate of line.
 * @param y2 Ending y-coordinate of line.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawLine(display_t *display, uint16_t x1, uint16_t y1,
                            uint16_t x2, uint16_t y2, uint16_t color);

/**
 * @brief Draw a filled rectangle.
 * @param display Handle to display.
 * @param x1 Top-left x-coordinate of rectangle.
 * @param y1 Top-left y-coordinate of rectangle.
 * @param x2 Bottom-right x-coordinate of rectangle.
 * @param y2 Bottom-right y-coordinate of rectangle.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawRect(display_t *display, uint16_t x1, uint16_t y1,
                            uint16_t x2, uint16_t y2, uint16_t color);

/**
 * @brief Draws a rectangle with rounded corners at a specified angle on the
 * display.
 * @param display Handle to display.
 * @param xc X-coordinate of the center of the rectangle.
 * @param yc Y-coordinate of the center of the rectangle.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param angle Angle of rotation in degrees.
 * @param color The 16-bit color value to write.
 *
 */
extern void displayDrawRectAngle(display_t *display, uint16_t xc, uint16_t yc,
                                 uint16_t w, uint16_t h, uint16_t angle,
                                 uint16_t color);

/**
 * @brief Draws a triangle at a specified angle on the display.
 * @param display Handle to display.
 * @param xc X-coordinate of the center of the rectangle.
 * @param yc Y-coordinate of the center of the rectangle.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param angle Angle of rotation in degrees.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawTriangleCenter(display_t *display, uint16_t xc,
                                      uint16_t yc, uint16_t w, uint16_t h,
                                      uint16_t angle, uint16_t color);

/**
 * @brief Draw a circle without infill on the display.
 * @param display Handle to display.
 * @param x_center X-coordinate of the center of the circle.
 * @param y_center Y-coordinate of the center of the circle.
 * @param r The radius of the circle in pixels.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawCircle(display_t *display, uint16_t x_center,
                              uint16_t y_center, uint16_t r, uint16_t color);

/**
 * @brief Draw a circle with infill on the display.
 * @param display Handle to display.
 * @param x_center X-coordinate of the center of the circle.
 * @param y_center Y-coordinate of the center of the circle.
 * @param r The radius of the circle in pixels.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawFillCircle(display_t *display, uint16_t x_center,
                                  uint16_t y_center, uint16_t r,
                                  uint16_t color);

/**
 * @brief Draw a rectangle with rounded angles.
 * @param display Handle to display.
 * @param x1 Top-left x-coordinate of rectangle.
 * @param y1 Top-left y-coordinate of rectangle.
 * @param x2 Bottom-right x-coordinate of rectangle.
 * @param y2 Bottom-right y-coordinate of rectangle.
 * @param r The radius of the circle that is used for the edges.
 * @param color The 16-bit color value to write.
 */
extern void displayDrawRoundRect(display_t *display, uint16_t x1, uint16_t y1,
                                 uint16_t x2, uint16_t y2, uint16_t r,
                                 uint16_t color);

/**
 * @brief RGB conversion for generating a color.
 * @param r Red value, 5 least significant bits.
 * @param g Green value, 6 least significant bits.
 * @param b Blue value, 5 least significant bits.
 */
extern uint16_t rgb_conv(uint16_t r, uint16_t g, uint16_t b);

/**
 * @brief Draws a character on the given coordinates of the display.
 * @param display Handle to display.
 * @param fx Pointer to font-file that is used for drawing the text.
 * @param x The x-coordinate of the text on the display.
 * @param y The y-coordinate of the text on the display.
 * @param ascii The ascii character to draw.
 * @param color The 16-bit color value to write.
 * @returns The x-value of the next character to be printed on the display.
 * @warning The font-file path must be valid from the directory in which the
 * executable is called, otherwise the error message "cannot get font from font
 * file" will be thrown. Absolute paths (starting with /) are safe. See
 * documentation for InitFontx.
 */
extern int displayDrawChar(display_t *display, FontxFile *fx, uint16_t x,
                           uint16_t y, uint8_t ascii, uint16_t color);

/**
 * @brief Function to draw a string on the display.
 * @param display Handle to display.
 * @param fx Pointer to font-file that is used for drawing the text.
 * @param x The x-coordinate of the text on the display.
 * @param y The y-coordinate of the text on the display.
 * @param ascii The ascii characters to draw.
 * @param color The 16-bit color value to write.
 * @returns The x or y coordinate of the next character, depending on the
 * orientation of the display.
 * @warning The font-file path must be valid from the directory in which the
 * executable is called, otherwise the error message "cannot get font from font
 * file" will be thrown. Absolute paths (starting with /) are safe. See
 * documentation for InitFontx.
 */
extern int displayDrawString(display_t *display, FontxFile *fx, uint16_t x,
                             uint16_t y, uint8_t *ascii, uint16_t color);

/**
 * @brief Changes the direction the characters will be printed.
 * @param display Handle to display.
 * @param dir The direction to set the font in the display handle.
 */
extern void displaySetFontDirection(display_t *display, uint16_t dir);

/**
 * @brief Enables the _font_fill and sets the _font_fill_color in the display
 * handle.
 * @param display Handle to display.
 * @param color The fill-color the font should have
 */
extern void displaySetFontFill(display_t *display, uint16_t color);

/**
 * @brief Sets the _font_fill parameter to false in the display handle, turns
 * off the font fill.
 * @param display Handle to display.
 *
 */
extern void displayUnsetFontFill(display_t *display);

/**
 * @brief Turns on _font_underline in the display handle and
 * sets the _font_underline_color to the specified color.
 * @param display Handle to display.
 * @param color The 16-bit color value to write.
 *
 */
extern void displaySetFontUnderLine(display_t *display, uint16_t color);

/**
 * @brief Turns off _font_underline in the display handle.
 * @param display Handle to display.
 */
extern void displayUnsetFontUnderLine(display_t *display);

/**
 * @brief Turn off the display.
 * @param display Handle to display.
 */
extern void displayDisplayOff(display_t *display);

/**
 * @brief Initialize DISPLAY screen.
 * @param display Handle to display.
 * @param width Width of screen in pixels.
 * @param height Height of screen in pixels.
 * @param offsetx Horizontal offset.
 * @param offsety Vertical offset.
 */
extern void displayDisplayOn(display_t *display);

/**
 * @brief Turn off the display backlight.
 * @param display Handle to display.
 */
extern void displayBacklightOff(display_t *display);

/**
 * @brief Turn on the display backlight.
 * @param display Handle to display.
 */
extern void displayBacklightOn(display_t *display);

/**
 * @brief Turn off inversion of the colors.
 * @param display Handle to display.
 */
extern void displayInversionOff(display_t *display);

/**
 * @brief Turn on inversion of the colors.
 * @param display Handle to display.
 */
extern void displayInversionOn(display_t *display);

/**
 * @brief Draw a triangle without infill between the three given points in the
 * given color.
 * @param display Handle to display.
 * @param x1 The first X-coordinate of the triangle.
 * @param y1 The first Y-coordinate of the triangle.
 * @param x2 The second X-coordinate of the triangle.
 * @param y2 The second Y-coordinate of the triangle.
 * @param x3 The third X-coordinate of the triangle.
 * @param y3 The third Y-coordinate of the triangle.
 * @param color The 16-bit color value to write.
 *
 */
extern void displayDrawTriangle(display_t *display, uint16_t x1, uint16_t y1,
                                uint16_t x2, uint16_t y2, uint16_t x3,
                                uint16_t y3, uint16_t color);

/**
 * @brief Flip the drawing off the screen.
 * @param display Handle to display
 * @param xflip Flip in the X direction
 * @param yflip Flip in the Y direction
 */
void display_set_flip(display_t *display, bool xflip, bool yflip);
/**
 * @}
 */

#endif /* MAIN_ST7789_H_ */
