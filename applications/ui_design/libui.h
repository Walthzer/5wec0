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

//row is 20 pixel tall and 239 wide
// 24FONTS are 12 pixel wide
// 1 row => 19 Chars
#define UI_ROWS 12
#define UI_ROW_HEIGHT 19
#define UI_CHAR_WIDTH 12
#define UI_ROW_LENGTH 19
#define NR_WORDS 4

//Some shorthand for words
#define SWORD WORD("\2")
#define EWORD WORD({'\0'})
#define WORD(text) FG_WORD(text,RGB_BLUE)
#define FG_WORD(text,fg) BG_WORD(text,fg,RGB_ORANGE)
#define BG_WORD(text,fg,bg) FULL_WORD(text,fg,bg,NORMAL) 
#define FULL_WORD(text,fg,bg,font) (ui_word_t){text,fg,bg,font}

/**
 * @brief Fonts that can be used in the UI
 */
enum ui_fonts {
  NORMAL,
  COMIC
};

/**
 * @brief Holds 1 word of text. A word is a string of text with 
 * the same fg and bg colours.
 */
typedef struct {
  char text[19];
  uint16_t fg;
  uint16_t bg;
  int font;
} ui_word_t;

/**
 * @brief Holds a row of the ui
 */
typedef struct
{
  uint8_t update_mask;
  uint8_t nr_words;
  ui_word_t words[NR_WORDS];
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
 * @brief Print a maximum of NR_WORDS words to a row.
 * Use a '\2' char to mark a word to be skipped.
 * @param ui Handle to ui.
 * @param argc amount of words.
 * @param words up to NR_WORDS ui_word_t types
 */
extern void ui_printr(ui_t* ui, int idx_row, int argc, ...);
#endif