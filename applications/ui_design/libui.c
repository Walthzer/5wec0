 /*
 *  TU/e 5WEC0::PYNQ Display UI system
 *
 *  Written by: Walthzer
 * 
 */

#include <libui.h>
#include <stdarg.h>

//PYNQ LIBS
#include <log.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN "ui"

void ui_init(ui_t* ui)
{
  display_init(&ui->display);
  InitFontx(ui->fonts, "../../fonts/ILGH24XB.FNT", "../../fonts/CSMS24.FNT");
  OpenFontx(&ui->fonts[0]);
  OpenFontx(&ui->fonts[1]);

  displayFillScreen(&ui->display, RGB_BLACK);

  ui_row_t blank_row = {~0, 1, {EWORD}, 0};
  for (int i = 0; i < UI_ROWS; i++)
  {
    ui->rows[i] = blank_row;
  }
  
}
void ui_destroy(ui_t* ui)
{
  CloseFontx(&ui->fonts[0]);
  CloseFontx(&ui->fonts[1]);
  display_destroy(&ui->display);
}

void ui_draw(ui_t* ui)
{
  for (int idx_row = 0; idx_row < UI_ROWS; idx_row++)
  {
    ui_row_t* row = &ui->rows[idx_row];
    //Row only gets draw if display is stale
    if (!row->update_mask)
      continue;

    row->update_mask = 0;
    int row_chars = 0;
    for (int idx_word = 0; idx_word < row->nr_words; idx_word++)
    {
      ui_word_t* ptr_word = &row->words[idx_word];
      int word_chars = strlen(ptr_word->text);

      if(word_chars <= 0)
        continue;

      #ifdef DEBUG_UI
        printf("Drawing: %s:: fg: %d:: bg: %d\n", ptr_word->text, ptr_word->fg, ptr_word->bg);
      #endif
      //Draw word bg
      displayDrawFillRect(&ui->display, 
                          row_chars*UI_CHAR_WIDTH, MIN(idx_row*UI_ROW_HEIGHT, 239), //upper_l
                          row_chars*UI_CHAR_WIDTH + word_chars*12, MIN((idx_row+1)*UI_ROW_HEIGHT, 239), //lower_r
                          ptr_word->bg);
      
      //Do we need to draw text
      int x = row_chars*UI_CHAR_WIDTH+1;
      int offset = 2;
      displayDrawString(&ui->display, &ui->fonts[ptr_word->font], x, (idx_row+1)*UI_ROW_HEIGHT+offset, (uint8_t*)ptr_word->text, ptr_word->fg);

      row_chars+=word_chars;
    }
    
  }
  
}
void ui_printr(ui_t* ui, int idx_row, int argc, ...)
{
  va_list args;
  int valid_words[NR_WORDS] = {-1};
  ui_word_t words[NR_WORDS];

  if(idx_row >= UI_ROWS || idx_row < 0)
  {
    pynq_error("printr: Row out of bounds");
    return;
  }
  ui_row_t* row = &ui->rows[idx_row];


  /* initialize args for argc argcber of arguments */
  va_start(args, argc);


  for (int i = 0; i < argc; i++) {
    words[i] = va_arg(args, ui_word_t);
  }

  /* clean memory reserved for args */
  va_end(args);

  //Get all valid words and prevent an overflow of the row.
  //Count existing words in the row aswell
  int total_length = 0;
  for (int i = 0; i < NR_WORDS; i++)
  {
    //Word not meant to be skipped
    if(i < argc && words[i].text[0] != '\2')
    {
      valid_words[i] = 1;
      total_length+=strlen(words[i].text);
    } else
    {
      valid_words[i] = 0;
      total_length+=strlen(row->words[i].text);
    }
  }

  if (total_length > UI_ROW_LENGTH)
  {
    pynq_error("printr: Row words to large");
    return;
  }
  
  row->nr_words = 0;
  for (int i = 0; i < NR_WORDS; i++)
  {
    switch (valid_words[i])
    {
    case 0:
      row->nr_words++;
      break;
    
    case 1:
      row->nr_words++;
      row->words[i] = words[i];
      break;
    }
  }
}