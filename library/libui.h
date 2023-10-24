 /*
 *  TU/e 5WEC0::PYNQ Display UI system
 *
 *  Written by: Walthzer
 * 
 */
#include <fontx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//PYNQ libs
#include <display.h>

#ifndef UI_H
#define UI_H

/**
 * @defgroup UI ui library
 *
 * @brief Wrapper that heavily abstracts the display behind
 * a UI object.
 *
 * Define a ui_t ui (called the ui "handle"), initialise it,
 * and pass this as the first parameter to all functions.
 *
 * @{
 */

#define RGB_ORANGE 64800
#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

//row is 20 pixel tall and 239 wide
// 24FONTS are 12 pixel wide
// 1 row => 19 Chars
#define UI_ROWS 10
#define UI_ROW_HEIGHT 19
#define UI_CHAR_WIDTH 12
#define UI_CHARH_OFFSET 1
#define UI_ROW_LENGTH 19

/**
 * @brief Fonts that can be used in the UI
 */
enum ui_fonts {
  NORMAL,
  COMIC
};

/**
 * @brief Internal type, do not use.
 * Holds formatted text
 */
typedef struct
{
  int fg;
  int bg;
  int font;
  char *str;
} ui_text_t;

/**
 * @brief Internal type, do not use.
 * Holds a row of the ui
 */
typedef struct
{
  uint32_t updateMask;
  //uint8_t sizeText;
  ui_text_t *texts[UI_ROW_LENGTH];
  int center;
} ui_row_t;

/**
 * @brief Internal type, do not use. Type of ui that hold all
 * ui data
 */
typedef struct {
  display_t display;
  FontxFile fonts[2];
  ui_row_t rows[UI_ROWS];
} ui_t;

/**
 * @brief Initialize the display and ui.
 * @param ui Handle to ui.
 */
extern void ui_init(ui_t* ui);

/**
 * @brief Clean up the display and ui.
 * @param ui Handle to ui.
 */
extern void ui_destroy(ui_t* ui);

/**
 * @brief Perform all drawcalls to update stale display elements
 * @param ui Handle to ui.
 */
extern void ui_draw(ui_t* ui);

/**
 * @brief Write formatted output to given row.
 * @param ui Handle to ui.
 * @param row index of the desired row.
 * @param fmt Format string
 * 
 * Supported formats, printf like:
 * @param %d -> integer
 * @param %f -> float
 * @param %c -> char
 * @param %s -> string
 * 
 * Special formats:
 * @param %q -> fg / text colour
 * @param %w -> bg / highlight colour
 * @param %t -> typeface/font
 * 
 * @warning Background is having BUGS fix pls
 */
extern void ui_rprintf(ui_t* ui, int row, char const *fmt, ...);

/**
 * @brief clear the row.
 * @param ui Handle to ui.
 * @param row Index of row
 */
extern void ui_rclear(ui_t *ui, int row);

/**
 * @brief Sets the row centering atribute
 * @param ui Handle to ui.
 * @param row Index of row
 * @param center set centering to true or false
 */
extern void ui_rcenter(ui_t *ui, int row, int center);
#endif