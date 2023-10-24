 /*
 *  TU/e 5WEC0::PYNQ Display UI system
 *
 *  Written by: Walthzer
 * 
 */
#include <libui.h>
#include <stdarg.h>
#include <stdlib.h>

//PYNQ LIBS
#include <libpynq.h>
#include <log.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN "ui"

void ui_init(ui_t* ui)
{
  #ifdef PYNQLIB_H
  display_init(&ui->display);
  InitFontx(ui->fonts, "../../fonts/ILGH24XB.FNT", "../../fonts/CSMS24.FNT");
  OpenFontx(&ui->fonts[0]);
  OpenFontx(&ui->fonts[1]);

  displayFillScreen(&ui->display, RGB_BLACK);
  #endif

  for (int i = 0; i < UI_ROWS; i++)
  {
    ui->rows[i] = (ui_row_t){~0, {NULL}, 0};
  }
  
}
void ui_destroy(ui_t* ui)
{
  //Clean up memory
  for (int i = 0; i < UI_ROWS; i++)
  {
    ui_rclear(ui, i);
  }

  #ifdef PYNQLIB_H
  CloseFontx(&ui->fonts[0]);
  CloseFontx(&ui->fonts[1]);
  
  display_destroy(&ui->display);
  #endif
}

uint32_t set_bit(uint32_t mask,int bit, int state)
{
  //Sets the bit of the updateMask with or
  return mask | ((int)state << bit);
}

int get_bit(uint32_t mask,int bit)
{
  //Gets the bit of the mask with and
  return !!((mask) & (1 << (bit)));
}



#ifdef PYNQLIB_H
//Pynq board display ui draw
void ui_draw(ui_t* ui)
{
  for (int idx_row = 0; idx_row < UI_ROWS; idx_row++)
  {
    ui_row_t* row = &ui->rows[idx_row];
    //No updates or empty -> skip row
    if (row->texts[0] == NULL || row->updateMask == 0)
      continue;
    
    //If we need centered text
    if (row->center)
    {
      //No updates 
      if(row->updateMask == 0)
        continue;

      row->updateMask = 0;

      //Draw the first bg
      displayDrawFillRect(&ui->display, 
            0, MIN(MAX(idx_row*(UI_ROW_HEIGHT+UI_CHARH_OFFSET)-1, 0), DISPLAY_HEIGHT-1), //upper_l
            DISPLAY_WIDTH - 1, MIN((idx_row+1)*(UI_ROW_HEIGHT+UI_CHARH_OFFSET)-2, DISPLAY_HEIGHT-1), //lower_r
            row->texts[0]->bg);

      //Compute row text length
      int start_pos = 0;
      for (int i = 0; i < UI_ROW_LENGTH; i++)
      {
        if(row->texts[i] != NULL)
          start_pos+=strlen(row->texts[i]->str);
      }

      //Get centered start pos
      start_pos = (DISPLAY_WIDTH - start_pos*UI_CHAR_WIDTH)/2;
      for (int i = 0; i < UI_ROW_LENGTH; i++)
      {
        if(row->texts[i] == NULL)
          break;
      //printf("%s", row->texts[t]->str);
        displayDrawString(&ui->display,
                          &ui->fonts[row->texts[i]->font],
                          start_pos+1,
                          (idx_row+1)*(UI_ROW_HEIGHT+UI_CHARH_OFFSET),
                          (uint8_t*)row->texts[i]->str,
                          row->texts[i]->fg);
        
        start_pos+=strlen(row->texts[i]->str)*UI_CHAR_WIDTH;
      }
      
      continue;
    }

    //Loop over texts
    int row_pos = 0;
    int bg_pos = 0;
    for (int t = 0; t < UI_ROW_LENGTH && row->texts[t] != NULL; t++)
    {
      //No updates -> skip text
      if(get_bit(row->updateMask, t) == 0)
      {
        //Move along the row
        int length = strlen(row->texts[t]->str);
        bg_pos += length;
        row_pos += length;
        continue;
      }
      row->updateMask = set_bit(row->updateMask, t, 0);

      //Account for text background having been looked
      //ahead and prevent redraw
      //Centered text gets only 1 bg colour
      if(bg_pos <= row_pos && row->texts[t]->str[0] != '\0')
      {
        //printf("Update bg of: %s", row->texts[t]->str)
        //Execute drawcalls -> accumulate calls
        //with a look ahead
        uint16_t bg_colour = row->texts[0]->bg;
        bg_pos+=strlen(row->texts[t]->str);
        for (int i = t+1; i < UI_ROW_LENGTH && row->texts[i] != NULL; i++)
        {
          if(get_bit(row->updateMask, i) == 1 && row->texts[i]->str[0] != '\0' && row->texts[i]->bg == bg_colour)
            bg_pos+=strlen(row->texts[i]->str);
        }
        //Dubble BG draw BUG
        displayDrawFillRect(&ui->display, 
                    row_pos*UI_CHAR_WIDTH, MIN(MAX(idx_row*(UI_ROW_HEIGHT+UI_CHARH_OFFSET)-1, 0), DISPLAY_HEIGHT-1), //upper_l
                    MIN((row_pos+bg_pos)*UI_CHAR_WIDTH, DISPLAY_WIDTH-1), MIN((idx_row+1)*(UI_ROW_HEIGHT+UI_CHARH_OFFSET)-2, DISPLAY_HEIGHT-1), //lower_r
                    bg_colour);
      }
      int x = (row_pos*UI_CHAR_WIDTH)+1;
      //printf("%s", row->texts[t]->str);
      displayDrawString(&ui->display, &ui->fonts[row->texts[t]->font], x, (idx_row+1)*(UI_ROW_HEIGHT+UI_CHARH_OFFSET), (uint8_t*)row->texts[t]->str, row->texts[t]->fg);

      //Move along the row
      row_pos += strlen(row->texts[t]->str);
    }
    row->updateMask = 0;
    
  }
  
}
#else
//Linux test ui_draw
void ui_draw(ui_t* ui)
{
  for (int idx_row = 0; idx_row < UI_ROWS; idx_row++)
  {
    ui_row_t* row = &ui->rows[idx_row];
    //No updates or empty -> skip row
    if (row->texts[0] == NULL || row->updateMask == 0)
      continue;
    

    //Loop over non-empty texts
    int changes = 0;
    for (int i = 0; i < UI_ROW_LENGTH && row->texts[i] != NULL; i++)
    {
      //No updates -> skip text
      if(get_bit(row->updateMask, i) == 0)
        continue;
      //printf("\nupdate: %s\n", row->texts[i]->str);
      
      //Draw background of row
      //Start by accumulating drawcalls of equal colour
      printf("\033[38;2;%d;%d;%dm", (row->texts[i]->fg >> 8) & 0xFF, (row->texts[i]->fg >> 3) & 0xFF, (row->texts[i]->fg << 3) & 0xFF);
      printf("\033[48;2;%d;%d;%dm", (row->texts[i]->bg >> 8), (row->texts[i]->bg >> 3) & 0xFF, (row->texts[i]->bg << 3) & 0xFF);
      changes++;
      printf("%s", row->texts[i]->str);
    }

    //Reset terminal colours
    printf("\x1b[0m");
    if(changes > 0)
      printf("\n");
  }
  
}
#endif

void ui_rclear(ui_t* ui, int row)
{
  for (int t = 0; t < UI_ROW_LENGTH; t++)
  {
    if (ui->rows[row].texts[t] == NULL)
      continue;

    if(ui->rows[row].texts[t]->str != NULL)
    {
      free(ui->rows[row].texts[t]->str);
      ui->rows[row].texts[t]->str = NULL;
    }
    
    free(ui->rows[row].texts[t]);
    ui->rows[row].texts[t] = NULL;
  }
}
void ui_rcenter(ui_t *ui, int row, int center)
{
  ui->rows[row].center = center ? 1 : 0;
}
//Updates the text object in a row if there are changes
//Sets the update mask bit for this text if changed
void ui_row_update_text(ui_row_t *row_p, int index, ui_text_t text)
{
    //printf("Update text: %d  str: %s len:%d\n", index, text.str, strlen(text.str));

    //Going insane rn so check for default values
    if(text.bg == -1)
      text.bg = RGB_BLACK;
    if(text.fg == -1)
      text.fg = RGB_WHITE;

    //text is non-existant
    if (row_p->texts[index] == NULL)
    {
      row_p->texts[index] = (ui_text_t*) malloc (sizeof(ui_text_t));
      //Assign lvalues
      *row_p->texts[index] = text; 
      //Assign *str heap pointer 
      row_p->texts[index]->str = (char*) malloc (strlen(text.str) * sizeof(char) + 1);
      strcpy(row_p->texts[index]->str, text.str);

      //Sets the index bit of the updateMask
      row_p->updateMask = set_bit(row_p->updateMask, index, 1);
    }
    
    //text exists -> preform checks
    ui_text_t *rtext_p = row_p->texts[index];
    int changed = 0;
    if(rtext_p->fg != text.fg)
    {
      rtext_p->fg = text.fg;
      changed = 1;
    }
    if(rtext_p->bg != text.bg)
    {
      rtext_p->bg = text.bg;
      changed = 1;
    }
    if(rtext_p->font != text.font)
    {
      rtext_p->font = text.font;
      changed = 1;
    }
    if(rtext_p->str == NULL || strcmp(rtext_p->str, text.str) != 0)
    {
      if(rtext_p->str != NULL)
      {
        free(rtext_p->str );
        rtext_p->str  = NULL;
      }
      
      rtext_p->str = (char*) malloc (strlen(text.str) * sizeof(char) + 1);
      strcpy(rtext_p->str, text.str);
      changed = 1;
    }

    //Clear str in text -> assume its consumed
    memset(text.str, '\0', UI_ROW_LENGTH);

    //Sets the index bit of the updateMask if we updated the text
    if(changed)
      row_p->updateMask = set_bit(row_p->updateMask, index, 1);
}

void ui_rprintf(ui_t* ui, int row, char const *fmt, ...)
{
  va_list args;

  /* initialize args*/
  va_start(args, fmt);

  char str[UI_ROW_LENGTH] = {'\0'};
  ui_text_t tmp_text = {-1, -1, NORMAL, (char *)&str};
  int len_row = 0;
  int len_str = 0;
  int tmp = 0;
  
  //Null char is 0 and thus false.
  //Loop through args and create new text structs when
  //we require conflicting formatting
  //Stop processing text if we run out of space in the row
  //Ugly pointer incrementation because the care ran out
  //halfway through. 
  int idx_text = 0;
  while(len_row < UI_ROW_LENGTH - 1 && *fmt)
  {
    //printf("len_row: %d  L1: %c\n", len_row, *fmt);
    //printf("L1: %c ", *fmt);
    if(*fmt == '%') //handle instructions
    {
      fmt++;
      //printf("L2: %c\n", *fmt);
      switch (*fmt)
      {
      //Foreground/Text colour
      case 'q':
        uint16_t fg = (uint16_t)va_arg(args, int);
        if(tmp_text.fg != -1 && fg != tmp_text.fg)
        {
          ui_row_update_text(&ui->rows[row], idx_text, tmp_text);
          len_str=0;
          idx_text++;
        }
        tmp_text.fg = fg;
      break;

      //Background/Highlight colour
      case 'w':
        //printf("Case w");
        uint16_t bg = (uint16_t)va_arg(args, int);
        if(tmp_text.bg != -1 && bg != tmp_text.bg)
        {
          ui_row_update_text(&ui->rows[row], idx_text, tmp_text);
          len_str=0;
          idx_text++;
        }
        tmp_text.bg = bg;
      break;

      //Font
      case 't':
        int font = va_arg(args, int);
        if(tmp_text.str[0] != '\0' && font != tmp_text.font)
        {
          ui_row_update_text(&ui->rows[row], idx_text, tmp_text);
          len_str=0;
          idx_text++;
        }
        tmp_text.font = font;
      break;

      //Integer conversion
      case 'd':
        //Will create a str no longer then the space left
        //in the row from the passed int and append it 
        //after the last char in text
        //printf("Case d\n");
        snprintf(tmp_text.str + len_str, UI_ROW_LENGTH - len_row - 1, "%d", va_arg(args, int));
        tmp = strlen(tmp_text.str);
        len_row+= tmp - len_str;
        len_str = tmp;
      break;

      //Float conversion
      case 'f':
        //Will create a str no longer then the space left
        //in the row from the passed int and append it 
        //after the last char in text
        //printf("Case d\n");
        snprintf(tmp_text.str + len_str, UI_ROW_LENGTH - len_row - 1, "%f", (float)va_arg(args, double));
        tmp = strlen(tmp_text.str);
        len_row+= tmp - len_str;
        len_str = tmp;
      break;

      //Char
      case 'c':
        tmp_text.str[len_str] = (char)va_arg(args, int);
        len_str++; len_row++;
      break;

      //String
      case 's':
        //Will append chars from input str onto
        //text str until we run out of line space
        //printf("Case s\n");
        strncpy(tmp_text.str + len_str, va_arg(args, char*), UI_ROW_LENGTH - len_row - 1);
        //printf("Text: %s\n", tmp_text.str);
        tmp = strlen(tmp_text.str);
        //printf("text.str: %s  len: %d\n", tmp_text.str, tmp);
        len_row+= tmp - len_str;
        len_str = tmp;
      break;

      default:
        #ifdef PYNQLIB_H
          pynq_warning("ui_rprintf::Unkown format Instruction\n");
        #else
          printf("ui_rprintf::Unkown format Instruction\n");
        #endif
        //Awful, Evil but quick fix!
        goto end_loop;
      break;
      }
    } else //Copy text
    {
      //printf("%s", "Trivial Char\n");
      tmp_text.str[len_str] = *fmt;
      //printf("%s\n", tmp_text.str);
      len_str++; len_row++;
    }
    //Increment str pointer;
    fmt++;
  }

  //printf("Text: %s Row_len: %d  *fmt: %d\n", tmp_text.str, len_row, *fmt);

  //We ended the loop before consuming all chars
  if(*fmt != 0)
  {
    #ifdef PYNQLIB_H
      pynq_warning("ui_rprintf::Ran out of line space\n");
    #else
      printf("ui_rprintf::Ran out of line space\n");
    #endif
  }

  //Code jumps here when we are unable to process an instruction
  end_loop:

  //Make sure the current text is appended to row
  //Prevent appending empty text
  if(tmp_text.str[0] != '\0')
  {
    ui_row_update_text(&ui->rows[row], idx_text, tmp_text);
  }

  /* clean memory reserved for args */
  va_end(args);
}