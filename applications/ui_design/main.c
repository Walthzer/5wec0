 /*
 *  TU/e 5WEC0::PYNQ Display UI system
 *
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>


#define RGB_ORANGE 64800

#define MIN(a,b) a > b ? b : a

//row is 20 pixel tall and 239 wide
// 24FONTS are 12 pixel wide
// 1 row => 19 Chars
#define UI_ROWS 12
#define UI_ROW_HEIGHT 19
#define UI_CHAR_WIDTH 12
typedef struct
{
  int updated;
  char text[19];
  uint16_t colour_fg;
  uint16_t colour_bg;
  int center;
} ui_row_t;

//Ui has room for 24 rows
typedef struct {
  int unused[512];
  display_t display;
  FontxFile fonts[2];
  ui_row_t rows[UI_ROWS];
} ui_container_t;

//Initialise the UI and its display
void ui_init(ui_container_t* container);
//Clean up the UI and its display
void ui_destroy(ui_container_t* container);
//Draw the UI
void ui_draw(ui_container_t* container);
//Update a whole row
void ui_set_row(ui_container_t* container, int row_index, char str[], uint16_t colour_fg, uint16_t colour_bg, int alignment);

void ui_init(ui_container_t* container)
{
  display_init(&container->display);
  InitFontx(container->fonts, "../../fonts/ILGH24XB.FNT", "../../fonts/CSMS24.FNT");
  OpenFontx(&container->fonts[0]);
  OpenFontx(&container->fonts[1]);

  displayFillScreen(&container->display, RGB_BLACK);

  for (int i = 0; i < UI_ROWS; i++)
  {
    ui_set_row(container, i, "", RGB_BLACK, RGB_BLACK, 0);
  }
  
}
void ui_destroy(ui_container_t* container)
{
  CloseFontx(&container->fonts[0]);
  CloseFontx(&container->fonts[1]);
  display_destroy(&container->display);
}
void ui_draw(ui_container_t* container)
{
  for (int i = 0; i < UI_ROWS; i++)
  {
    ui_row_t* row = &container->rows[i];
    //Row only gets draw if display is stale
    if (!row->updated)
      continue;
    
    displayDrawFillRect(&container->display, 
                        0, MIN(i*UI_ROW_HEIGHT, 239), //upper_l
                        239, MIN((i+1)*UI_ROW_HEIGHT, 239), //lower_r
                        row->colour_bg);
    
    //Do we need to draw text
    int str_length = strlen(row->text);
    if(str_length && i+1 < UI_ROWS)
    {
      int x = row->center ? (DISPLAY_WIDTH - (UI_CHAR_WIDTH * str_length))/2 : 0;
      int offset = row->center ? 0 : +2;
      displayDrawString(&container->display, &container->fonts[row->center], x, (i+1)*UI_ROW_HEIGHT+offset, (uint8_t*)row->text, row->colour_fg);
    }
  }
  
}
void ui_set_row(ui_container_t* container, int row_index, char str[], uint16_t colour_fg, uint16_t colour_bg, int alignment)
{
  ui_row_t row = {1, "", colour_fg, colour_bg, alignment};
  if(strlen(str) > 19)
  {
    pynq_warning("Oversized string: skipping");
    return;
  }
  strcpy((char* restrict)&row.text, str);
  container->rows[row_index] = row;
}

int main(void) {
  pynq_init();
  ui_container_t ui_container;
  ui_init(&ui_container);
  ui_set_row(&ui_container, 0, "RYB 24", RGB_WHITE, RGB_ORANGE, 1);
  ui_set_row(&ui_container, 1, "HeartBeat", RGB_WHITE, RGB_ORANGE, 1);
  ui_set_row(&ui_container, 2, "Connection: Lost", RGB_RED, RGB_BLACK, 1);
  ui_set_row(&ui_container, 3, "Curr HeartBeat: 69", RGB_GREEN, RGB_BLACK, 1);

  

  //draw the ui;
  ui_draw(&ui_container);

  ui_destroy(&ui_container);
  pynq_destroy();

  return 0;
};