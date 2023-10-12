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
#include <arm_shared_memory_system.h>
#include <display.h>
#include <gpio.h>
#include <lcdconfig.h>
#include <log.h>
#include <math.h>
#include <platform.h>
#include <string.h>
#include <switchbox.h>
#include <unistd.h>
#include <util.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN "display"

#define TAG "ST7789"
#define _DEBUG_ 0

#define M_PI 3.14159265358979323846

static arm_shared spi0_handle;
static volatile uint32_t *spi0 = NULL;

// states that are set for usage of the DC pin in SPI
typedef enum { SPI_Data_Mode = 1, SPI_Command_Mode = 0 } spi_mode_t;

#define GPIO_MODE_OUTPUT 1

gpio_level_t spi_to_gpio(spi_mode_t mode) {
  switch (mode) {
  case SPI_Data_Mode:
    return GPIO_LEVEL_HIGH;
  case SPI_Command_Mode:
    return GPIO_LEVEL_LOW;
  default:
    return GPIO_LEVEL_LOW;
  }
}

bool spi_master_write_command(display_t *display, uint8_t cmd) {
  gpio_set_level(display->_dc, spi_to_gpio(SPI_Command_Mode));
  spi0[0x68 / 4] = cmd;
  while (((spi0[0x64 / 4]) & 4) == 0) {
  }
  usleep(1);
  return true;
}

bool spi_master_write_data_byte(display_t *display, uint8_t data) {
  gpio_set_level(display->_dc, spi_to_gpio(SPI_Data_Mode));

  spi0[0x68 / 4] = data;
  while (((spi0[0x64 / 4]) & 4) == 0) {
  }
  return true;
}

bool spi_master_write_data_word(display_t *display, uint16_t data) {
  static uint8_t Byte[2];
  Byte[0] = (data >> 8) & 0xFF;
  Byte[1] = data & 0xFF;
  gpio_set_level(display->_dc, spi_to_gpio(SPI_Data_Mode));
  spi0[0x68 / 4] = Byte[0];
  spi0[0x68 / 4] = Byte[1];

  while (((spi0[0x64 / 4]) & 4) == 0) {
  }
  return true;
}

bool spi_master_write_addr(display_t *display, uint16_t addr1, uint16_t addr2) {
  static uint8_t Byte[4];
  Byte[0] = (addr1 >> 8) & 0xFF;
  Byte[1] = addr1 & 0xFF;
  Byte[2] = (addr2 >> 8) & 0xFF;
  Byte[3] = addr2 & 0xFF;
  gpio_set_level(display->_dc, spi_to_gpio(SPI_Data_Mode));

  // check ordering
  spi0[0x68 / 4] = Byte[0];
  spi0[0x68 / 4] = Byte[1];
  spi0[0x68 / 4] = Byte[2];
  spi0[0x68 / 4] = Byte[3];

  while (((spi0[0x64 / 4]) & 4) == 0) {
  }
  return true;
}

bool spi_master_write_color(display_t *display, uint16_t color, uint16_t size) {
  gpio_set_level(display->_dc, spi_to_gpio(SPI_Data_Mode));
  for (int i = 0; i < size; i++) {
    while (((spi0[0x64 / 4]) & 8) == 8) {
    }
    spi0[0x68 / 4] = (color >> 8) & 0xFF;
    while (((spi0[0x64 / 4]) & 8) == 8) {
    }
    spi0[0x68 / 4] = (color)&0xFF;
  }
  while (((spi0[0x64 / 4]) & 4) == 0) {
  }
  return -1;
}

bool spi_master_write_colors(display_t *display, uint16_t *colors,
                             uint16_t size) {
  gpio_set_level(display->_dc, spi_to_gpio(SPI_Data_Mode));
  for (int i = 0; i < size; i++) {
    while (((spi0[0x64 / 4]) & 8) == 8) {
    }
    spi0[0x68 / 4] = (colors[i] >> 8) & 0xFF;
    while (((spi0[0x64 / 4]) & 8) == 8) {
    }
    spi0[0x68 / 4] = (colors[i]) & 0xFF;
  }
  // wait till empty, then add a small extra buffer
  // because last byte we don't exactly know when send.
  while (((spi0[0x64 / 4]) & 4) == 0) {
  }
  return true;
}

void spi_master_init(display_t *display) {
  // linking given pins in the switchbox
  switchbox_set_pin(LCD_MOSI, SWB_SPI1_MOSI);
  switchbox_set_pin(LCD_SCLK, SWB_SPI1_CLK);
  switchbox_set_pin(LCD_CS, SWB_SPI1_SS);
  switchbox_set_pin(LCD_DC, SWB_GPIO);
  switchbox_set_pin(LCD_RESET, SWB_GPIO);
  switchbox_set_pin(LCD_BL, SWB_GPIO);

  // setting the appropriate direction of each protocol pin
  gpio_set_direction(LCD_DC, GPIO_DIR_OUTPUT);
  gpio_set_direction(LCD_RESET, GPIO_DIR_OUTPUT);
  gpio_set_direction(LCD_BL, GPIO_DIR_OUTPUT);
  gpio_set_level(LCD_DC, GPIO_LEVEL_LOW);
  gpio_set_level(LCD_RESET, GPIO_LEVEL_LOW);
  gpio_set_level(LCD_BL, GPIO_LEVEL_LOW);

  // creating a shared memory instance for communicating the hardware addresses
  // of the linked pins
  spi0 = arm_shared_init(&spi0_handle, axi_quad_spi_1, 4096);
  if (_DEBUG_)
    printf("spi reset: %08X\n", spi0[0x40 / 4]);
  spi0[0x40 / 4] = 0x0000000a;
  if (_DEBUG_)
    printf("spi control: %08X\n", spi0[0x60 / 4]);
  spi0[0x60 / 4] = (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1);
  if (_DEBUG_)
    printf("spi control: %08X\n", spi0[0x60 / 4]);
  if (_DEBUG_)
    printf("spi status: %08X\n", spi0[0x64 / 4]);

  // select slave 1
  spi0[0x70 / 4] = 0;
  if (_DEBUG_)
    printf("spi control: %08X\n", spi0[0x60 / 4]);
  if (_DEBUG_)
    printf("testing DISPLAY\n");
  if (_DEBUG_)
    printf("LCD_CS=%d\n", LCD_CS);
  if (LCD_CS >= 0) {
    gpio_reset_pin(LCD_CS);
    gpio_set_direction(LCD_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_CS, 0);
  }

  if (_DEBUG_)
    printf("LCD_DC=%d", LCD_DC);
  gpio_reset_pin(LCD_DC);
  gpio_set_direction(LCD_DC, GPIO_MODE_OUTPUT);
  gpio_set_level(LCD_DC, 0);
  if (_DEBUG_)
    printf("LCD_RESET=%d", LCD_RESET);

  if (LCD_RESET >= 0) {
    gpio_reset_pin(LCD_RESET);
    gpio_set_direction(LCD_RESET, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_RESET, 1);
    sleep_msec(100);
    gpio_set_level(LCD_RESET, 0);
    sleep_msec(500);
    gpio_set_level(LCD_RESET, 1);
    sleep_msec(300);
  }

  if (_DEBUG_)
    printf("LCD_BL=%d", LCD_BL);
  if (LCD_BL >= 0) {
    gpio_reset_pin(LCD_BL);
    gpio_set_direction(LCD_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_BL, 0);
  }

  if (_DEBUG_)
    printf("LCD_MOSI=%d", LCD_MOSI);
  if (_DEBUG_)
    printf("LCD_SCLK=%d\n", LCD_SCLK);

  display->_dc = LCD_DC;
  display->_bl = LCD_BL;
}

void displayInit(display_t *display, int width, int height, int offsetx,
                 int offsety) {
  spi_master_init(display);
  display->_width = width;
  display->_height = height;
  display->_offsetx = offsetx;
  display->_offsety = offsety;
  display->_font_direction = TEXT_DIRECTION0;
  display->_font_fill = false;
  display->_font_underline = false;

  spi_master_write_command(display, 0x01); // software Reset
  sleep_msec(150);

  spi_master_write_command(display, 0x11); // sleep Out
  sleep_msec(255);

  spi_master_write_command(display, 0x3A); // Interface Pixel Format
  spi_master_write_data_byte(display, 0x55);
  sleep_msec(10);

  spi_master_write_command(display, 0x36); // Memory Data Access Control
  spi_master_write_data_byte(display, 0x00);

  spi_master_write_command(display, 0x2A); // Column Address Set
  spi_master_write_data_byte(display, 0x00);
  spi_master_write_data_byte(display, 0x00);
  spi_master_write_data_byte(display, 0x00);
  spi_master_write_data_byte(display, 0xF0);

  spi_master_write_command(display, 0x2B); // Row Address Set
  spi_master_write_data_byte(display, 0x00);
  spi_master_write_data_byte(display, 0x00);
  spi_master_write_data_byte(display, 0x00);
  spi_master_write_data_byte(display, 0xF0);

  spi_master_write_command(display, 0x21); // Display Inversion On
  sleep_msec(10);

  spi_master_write_command(display, 0x13); // Normal Display Mode On
  sleep_msec(10);

  spi_master_write_command(display, 0x29); // Display ON
  sleep_msec(255);

  if (display->_bl >= 0) {
    gpio_set_level(display->_bl, 1);
  }
}

void display_set_flip(display_t *display, bool xflip, bool yflip) {
  if (display == NULL) {
    pynq_error("display_destroy: display has not been initialized\n");
  }
  if (display->_width != DISPLAY_WIDTH || display->_height != DISPLAY_HEIGHT) {
    pynq_error("display_destroy: internal error (wrong display hardware)\n");
  }
  spi_master_write_command(display, 0x36); // Memory Data Access Control
  uint8_t set = (yflip << 7) | (xflip << 6);
  spi_master_write_data_byte(display, set);
  if (yflip) {
    display->_offsety = 320 - display->_height;
  } else {
    display->_offsety = 0;
  }
  if (xflip) {
    display->_offsetx = 240 - display->_width;
  } else {
    display->_offsetx = 0;
  }
}

void display_init(display_t *display) {
  displayInit(display, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0);
  display_set_flip(display, true, true);
}

void display_destroy(display_t *display __attribute__((unused))) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("display_destroy: display has not been initialized\n");
  }
  // if channel is open
  if (spi0 != NULL) {
    (void)arm_shared_close(&spi0_handle);
    spi0 = NULL;
  }
}

void displayDrawPixel(display_t *display, uint16_t x, uint16_t y,
                      uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawPixel: display has not been initialized\n");
  }
  if (x >= display->_width || y >= display->_height) {
    pynq_error("displayDrawPixel: x=%d y=%d outside screen boundaries\n", x, y);
  }
  uint16_t _x = x + display->_offsetx;
  uint16_t _y = y + display->_offsety;

  spi_master_write_command(display, 0x2A); // set column(x) address
  spi_master_write_addr(display, _x, _x);
  spi_master_write_command(display, 0x2B); // set Page(y) address
  spi_master_write_addr(display, _y, _y);
  spi_master_write_command(display, 0x2C); // memory write
  spi_master_write_data_word(display, color);
}

void displayDrawMultiPixels(display_t *display, uint16_t x, uint16_t y,
                            uint16_t size, uint16_t *colors) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawMultiPixels: display has not been initialized\n");
  }
  if (x > display->_width || x + size > display->_width ||
      y >= display->_height) {
    pynq_error(
        "displayDrawMultiPixels: x=%d y=%d size=%d outside screen boundaries\n",
        x, y, size);
  }

  uint16_t _x1 = x + display->_offsetx;
  uint16_t _x2 = _x1 + size;
  uint16_t _y1 = y + display->_offsety;
  uint16_t _y2 = _y1;

  spi_master_write_command(display, 0x2A); // set column(x) address
  spi_master_write_addr(display, _x1, _x2);
  spi_master_write_command(display, 0x2B); // set Page(y) address
  spi_master_write_addr(display, _y1, _y2);
  spi_master_write_command(display, 0x2C); // memory write
  spi_master_write_colors(display, colors, size);
}

void displayDrawFillRect(display_t *display, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawPixel: display has not been initialized\n");
  }
  if (x1 >= display->_width || x2 >= display->_width ||
      y1 >= display->_height || y2 >= display->_height) {
    pynq_error("displayDrawFillRect: x1=%d y1=%d x2=%d y2=%d outside screen "
               "boundaries\n",
               x1, y1, x2, y2);
  }
  // swapping points so that it is always plotted from x1 y1 bottom left, x2 y2
  // top right
  uint16_t x1_temp = x1, x2_temp = x2;
  uint16_t y1_temp = y1, y2_temp = y2;
  if (x1 > x2) {
    x1 = x2_temp;
    x2 = x1_temp;
  }

  if (y1 > y2) {
    y1 = y2_temp;
    y2 = y1_temp;
  }

  // printf("offset(x)=%d offset(y)=%d",display->_offsetx,display->_offsety);
  uint16_t _x1 = x1 + display->_offsetx;
  uint16_t _x2 = x2 + display->_offsetx;
  uint16_t _y1 = y1 + display->_offsety;
  uint16_t _y2 = y2 + display->_offsety;

  spi_master_write_command(display, 0x2A); // set column(x) address
  spi_master_write_addr(display, _x1, _x2);
  spi_master_write_command(display, 0x2B); // set Page(y) address
  spi_master_write_addr(display, _y1, _y2);
  spi_master_write_command(display, 0x2C); // memory write
  for (int i = _x1; i <= _x2; i++) {
    uint16_t size = _y2 - _y1 + 1;
    spi_master_write_color(display, color, size);
  }
}

void displayDisplayOff(display_t *display) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDisplayOff: display has not been initialized\n");
  }
  spi_master_write_command(display, 0x28); // display off
}

void displayDisplayOn(display_t *display) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDisplayOn: display has not been initialized\n");
  }
  spi_master_write_command(display, 0x29); // display on
}

void displayFillScreen(display_t *display, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayFillScreen: display has not been initialized\n");
  }
  displayDrawFillRect(display, 0, 0, display->_width - 1, display->_height - 1,
                      color);
}

void displayDrawLine(display_t *display, uint16_t x1, uint16_t y1, uint16_t x2,
                     uint16_t y2, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawLine: display has not been initialized\n");
  }
  if (x1 >= display->_width || y1 >= display->_height) {
    pynq_error("displayDrawLine: x1=%d y1=%d outside screen boundaries\n", x1,
               y1);
  } else if (x2 >= display->_width || y2 >= display->_height) {
    pynq_error("displayDrawLine: x2=%d y2=%d outside screen boundaries\n", x2,
               y2);
  }
  int i;
  int dx, dy;
  int sx, sy;
  int E;

  /* distance between two points */
  dx = (x2 > x1) ? x2 - x1 : x1 - x2;
  dy = (y2 > y1) ? y2 - y1 : y1 - y2;

  /* direction of two point */
  sx = (x2 > x1) ? 1 : -1;
  sy = (y2 > y1) ? 1 : -1;

  /* inclination < 1 */
  if (dx > dy) {
    E = -dx;
    for (i = 0; i <= dx; i++) {
      displayDrawPixel(display, x1, y1, color);
      x1 += sx;
      E += 2 * dy;
      if (E >= 0) {
        y1 += sy;
        E -= 2 * dx;
      }
    }

    /* inclination >= 1 */
  } else {
    E = -dy;
    for (i = 0; i <= dy; i++) {
      displayDrawPixel(display, x1, y1, color);
      y1 += sy;
      E += 2 * dx;
      if (E >= 0) {
        x1 += sx;
        E -= 2 * dy;
      }
    }
  }
}

void displayDrawRect(display_t *display, uint16_t x1, uint16_t y1, uint16_t x2,
                     uint16_t y2, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawRect: display has not been initialized\n");
  }
  if (x1 >= display->_width || y1 >= display->_height) {
    pynq_error("displayDrawRect: x1=%d y1=%d outside screen boundaries\n", x1,
               y1);
  } else if (x2 >= display->_width || y2 >= display->_height) {
    pynq_error("displayDrawRect: x2=%d y2=%d outside screen boundaries\n", x2,
               y2);
  }
  displayDrawLine(display, x1, y1, x2, y1, color);
  displayDrawLine(display, x2, y1, x2, y2, color);
  displayDrawLine(display, x2, y2, x1, y2, color);
  displayDrawLine(display, x1, y2, x1, y1, color);
}

void displayDrawRectAngle(display_t *display, uint16_t xc, uint16_t yc,
                          uint16_t w, uint16_t h, uint16_t angle,
                          uint16_t color) {
  double xd, yd, rd;
  int x1, y1;
  int x2, y2;
  int x3, y3;
  int x4, y4;
  rd = -angle * M_PI / 180.0;
  xd = 0.0 - w / 2;
  yd = h / 2;
  x1 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
  y1 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

  yd = 0.0 - yd;
  x2 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
  y2 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

  xd = w / 2;
  yd = h / 2;
  x3 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
  y3 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

  yd = 0.0 - yd;
  x4 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
  y4 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawRectAngle: display has not been initialized\n");
  }
  if (x1 >= display->_width || y1 >= display->_height) {
    pynq_error("displayDrawRectAngle: x1=%d y1=%d outside screen boundaries\n",
               x1, y1);
  } else if (x2 >= display->_width || y2 >= display->_height) {
    pynq_error("displayDrawRectAngle: x2=%d y2=%d outside screen boundaries\n",
               x2, y2);
  } else if (x3 >= display->_width || y3 >= display->_height) {
    pynq_error("displayDrawRectAngle: x3=%d y3=%d outside screen boundaries\n",
               x3, y3);
  } else if (x4 >= display->_width || y4 >= display->_height) {
    pynq_error("displayDrawRectAngle: x4=%d y4=%d outside screen boundaries\n",
               x4, y4);
  }

  displayDrawLine(display, x1, y1, x2, y2, color);
  displayDrawLine(display, x1, y1, x3, y3, color);
  displayDrawLine(display, x2, y2, x4, y4, color);
  displayDrawLine(display, x3, y3, x4, y4, color);
}

// x1: First X coordinate of triangle point
// y1: First Y coordinate of triangle point
// x2: Second X coordinate of triangle point
// y2: Second Y coordinate of triangle point
// x3: Third X coordinate of triangle point
// y3: Third Y coordinate of triangle point
// color:color
void displayDrawTriangle(display_t *display, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3,
                         uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawTriangle: display has not been initialized\n");
  }
  if (x1 >= display->_width || y1 >= display->_height) {
    pynq_error("displayDrawRectAngle: x1=%d y1=%d outside screen boundaries\n",
               x1, y1);
  } else if (x2 >= display->_width || y2 >= display->_height) {
    pynq_error("displayDrawRectAngle: x2=%d y2=%d outside screen boundaries\n",
               x2, y2);
  } else if (x3 >= display->_width || y3 >= display->_height) {
    pynq_error("displayDrawRectAngle: x3=%d y3=%d outside screen boundaries\n",
               x3, y3);
  }

  // draw the lines for the basic triangle
  displayDrawLine(display, x1, y1, x2, y2, color);
  displayDrawLine(display, x2, y2, x3, y3, color);
  displayDrawLine(display, x3, y3, x1, y1, color);
}

// when the origin is (0, 0), the point (x1, y1) after rotating the point (x, y)
// by the angle is obtained by the following calculation.
//  x1 = x * cos(angle) - y * sin(angle)
//  y1 = x * sin(angle) + y * cos(angle)
void displayDrawTriangleCenter(display_t *display, uint16_t xc, uint16_t yc,
                               uint16_t w, uint16_t h, uint16_t angle,
                               uint16_t color) {
  double xd, yd, rd;
  int x1, y1;
  int x2, y2;
  int x3, y3;
  rd = -angle * M_PI / 180.0;
  xd = 0.0;
  yd = h / 2;
  x1 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
  y1 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

  xd = w / 2;
  yd = 0.0 - yd;
  x2 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
  y2 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

  xd = 0.0 - w / 2;
  x3 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
  y3 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawTriangleCenter: display has not been initialized\n");
  }
  if (x1 >= display->_width || y1 >= display->_height) {
    pynq_error("displayDrawRectAngle: x1=%d y1=%d outside screen boundaries\n",
               x1, y1);
  } else if (x2 >= display->_width || y2 >= display->_height) {
    pynq_error("displayDrawRectAngle: x2=%d y2=%d outside screen boundaries\n",
               x2, y2);
  } else if (x3 >= display->_width || y3 >= display->_height) {
    pynq_error("displayDrawRectAngle: x3=%d y3=%d outside screen boundaries\n",
               x3, y3);
  }

  displayDrawLine(display, x1, y1, x2, y2, color);
  displayDrawLine(display, x1, y1, x3, y3, color);
  displayDrawLine(display, x2, y2, x3, y3, color);
}

void displayDrawCircle(display_t *display, uint16_t x_center, uint16_t y_center,
                       uint16_t r, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawCircle: display has not been initialized\n");
  }
  if (r == 0) {
    pynq_error(
        "displayDrawCircle: x_center=%d y_center=%d r=%d r cannot be 0\n",
        x_center, y_center, r);
  }

  int x_max = x_center + r, x_min = x_center - r, y_max = y_center + r,
      y_min = y_center - r;

  if (x_max >= display->_width || x_min < 0 || y_max >= display->_height ||
      y_min < 0) {
    pynq_error("displayDrawCircle: x_center=%d y_center=%d r=%d outside screen "
               "boundaries\n",
               x_center, y_center, r);
  }

  int x;
  int y;
  int err;
  int old_err;

  x = 0;
  y = -r;
  err = 2 - 2 * r;
  do {
    displayDrawPixel(display, x_center - x, y_center + y, color);
    displayDrawPixel(display, x_center - y, y_center - x, color);
    displayDrawPixel(display, x_center + x, y_center - y, color);
    displayDrawPixel(display, x_center + y, y_center + x, color);
    if ((old_err = err) <= x)
      err += ++x * 2 + 1;
    if (old_err > y || err > x)
      err += ++y * 2 + 1;
  } while (y < 0);
}

void displayDrawFillCircle(display_t *display, uint16_t x_center,
                           uint16_t y_center, uint16_t r, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawFillCircle: display has not been initialized\n");
  }
  if (r == 0) {
    pynq_error(
        "displayDrawFillCircle: x_center=%d y_center=%d r=%d r cannot be 0\n",
        x_center, y_center, r);
  }

  int x_max = x_center + r, x_min = x_center - r, y_max = y_center + r,
      y_min = y_center - r;

  if (x_max >= display->_width || x_min < 0 || y_max >= display->_height ||
      y_min < 0) {
    pynq_error("displayDrawFillCircle: x_center=%d y_center=%d r=%d outside "
               "screen boundaries\n",
               x_center, y_center, r);
  }

  int x;
  int y;
  int err;
  int old_err;
  int ChangeX;

  x = 0;
  y = -r;
  err = 2 - 2 * r;
  ChangeX = 1;
  do {
    if (ChangeX) {
      displayDrawLine(display, x_center - x, y_center - y, x_center - x,
                      y_center + y, color);
      displayDrawLine(display, x_center + x, y_center - y, x_center + x,
                      y_center + y, color);
    } // endif
    ChangeX = (old_err = err) <= x;
    if (ChangeX)
      err += ++x * 2 + 1;
    if (old_err > y || err > x)
      err += ++y * 2 + 1;
  } while (y <= 0);
}

void displayDrawRoundRect(display_t *display, uint16_t x1, uint16_t y1,
                          uint16_t x2, uint16_t y2, uint16_t r,
                          uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawRoundRect: display has not been initialized\n");
  }
  if (r == 0) {
    pynq_error("displayDrawRoundRect: x_center=%d x1=%d y1=%d r cannot be 0\n",
               x1, y1, r);
  } else if (x1 >= display->_width || y1 >= display->_height) {
    pynq_error("displayDrawRoundRect: x1=%d y1=%d outside screen boundaries\n",
               x1, y1);
  } else if (x2 >= display->_width || y2 >= display->_height) {
    pynq_error("displayDrawRoundRect: x2=%d y2=%d outside screen boundaries\n",
               x2, y2);
  }
  int x;
  int y;
  int err;
  int old_err;
  unsigned char temp;

  if (x1 > x2) {
    temp = x1;
    x1 = x2;
    x2 = temp;
  }

  if (y1 > y2) {
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  if (_DEBUG_)
    printf("x1=%d x2=%d delta=%d r=%d", x1, x2, x2 - x1, r);
  if (_DEBUG_)
    printf("y1=%d y2=%d delta=%d r=%d", y1, y2, y2 - y1, r);
  if (x2 - x1 < r)
    return; // TODO add 20190517?
  if (y2 - y1 < r)
    return; // TODO add 20190517?

  x = 0;
  y = -r;
  err = 2 - 2 * r;

  do {
    if (x) {
      displayDrawPixel(display, x1 + r - x, y1 + r + y, color);
      displayDrawPixel(display, x2 - r + x, y1 + r + y, color);
      displayDrawPixel(display, x1 + r - x, y2 - r - y, color);
      displayDrawPixel(display, x2 - r + x, y2 - r - y, color);
    }
    if ((old_err = err) <= x)
      err += ++x * 2 + 1;
    if (old_err > y || err > x)
      err += ++y * 2 + 1;
  } while (y < 0);

  if (_DEBUG_)
    printf("x1+r=%d x2-r=%d", x1 + r, x2 - r);
  displayDrawLine(display, x1 + r, y1, x2 - r, y1, color);
  displayDrawLine(display, x1 + r, y2, x2 - r, y2, color);
  if (_DEBUG_)
    printf("y1+r=%d y2-r=%d", y1 + r, y2 - r);
  displayDrawLine(display, x1, y1 + r, x1, y2 - r, color);
  displayDrawLine(display, x2, y1 + r, x2, y2 - r, color);
}

uint16_t rgb_conv(uint16_t r, uint16_t g, uint16_t b) {
  return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

int displayDrawChar(display_t *display, FontxFile *fxs, uint16_t x, uint16_t y,
                    uint8_t ascii, uint16_t color) {
  uint16_t xx, yy, bit, ofs;
  unsigned char fonts[128]; // font pattern
  unsigned char pw, ph;
  int h, w;
  uint16_t mask;
  bool rc = GetFontx(fxs, ascii, fonts, &pw, &ph);

  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawChar: display has not been initialized\n");
  }
  if (_DEBUG_) {
    printf("_font_direction=%d\n", display->_font_direction);
    printf("GetFontx rc=%d pw=%d ph=%d\n", rc, pw, ph);
  }

  if (!rc) {
    pynq_error("displayDrawChar: cannot get font from font file\n");
  }

  switch (display->_font_direction) {
  case TEXT_DIRECTION0:
    if (x + pw >= display->_width || y + ph >= display->_height) {
      pynq_error("displayDrawChar: x=%d y=%d for font height=%d width=%d and "
                 "direction=%d outside screen boundaries\n",
                 x, y, ph, pw, display->_font_direction);
    }
    break;
  case TEXT_DIRECTION90:
    if (x + ph >= display->_height || y + pw >= display->_width) {
      pynq_error("displayDrawChar: x=%d y=%d for font height=%d width=%d and "
                 "direction=%d outside screen boundaries\n",
                 x, y, ph, pw, display->_font_direction);
    }
    break;
  case TEXT_DIRECTION180:
    if (x - pw <= 0 || y - ph <= 0) {
      pynq_error("displayDrawChar: x=%d y=%d for font height=%d width=%d and "
                 "direction=%d outside screen boundaries\n",
                 x, y, ph, pw, display->_font_direction);
    }
    break;
  case TEXT_DIRECTION270:
    if (x - ph <= 0 || y - pw <= 0) {
      pynq_error("displayDrawChar: x=%d y=%d for font height=%d width=%d and "
                 "direction=%d outside screen boundaries\n",
                 x, y, ph, pw, display->_font_direction);
    }
    break;
  }

  int16_t xd1 = 0, yd1 = 0, xd2 = 0, yd2 = 0;
  uint16_t xss = 0, yss = 0;
  int16_t xsd = 0, ysd = 0, next = 0;
  uint16_t x0 = 0, x1 = 0, y0 = 0, y1 = 0;
  if (display->_font_direction == 0) {
    xd1 = +1;
    yd1 = +1; //-1;
    xd2 = 0;
    yd2 = 0;
    xss = x;
    yss = y - (ph - 1);
    xsd = 1;
    ysd = 0;
    next = x + pw;

    x0 = x;
    y0 = y - (ph - 1);
    x1 = x + (pw - 1);
    y1 = y;
  } else if (display->_font_direction == 2) {
    xd1 = -1;
    yd1 = -1; //+1;
    xd2 = 0;
    yd2 = 0;
    xss = x;
    yss = y + ph + 1;
    xsd = 1;
    ysd = 0;
    next = x - pw;

    x0 = x - (pw - 1);
    y0 = y;
    x1 = x;
    y1 = y + (ph - 1);
  } else if (display->_font_direction == 1) {
    xd1 = 0;
    yd1 = 0;
    xd2 = -1;
    yd2 = +1; //-1;
    xss = x + ph;
    yss = y;
    xsd = 0;
    ysd = 1;
    next = y + pw; // y - pw;

    x0 = x;
    y0 = y;
    x1 = x + (ph - 1);
    y1 = y + (pw - 1);
  } else if (display->_font_direction == 3) {
    xd1 = 0;
    yd1 = 0;
    xd2 = +1;
    yd2 = -1; //+1;
    xss = x - (ph - 1);
    yss = y;
    xsd = 0;
    ysd = 1;
    next = y - pw; // y + pw;

    x0 = x - (ph - 1);
    y0 = y - (pw - 1);
    x1 = x;
    y1 = y;
  }

  // TODO: fix the problem of underflow properly some time
  if (display->_font_fill && x0 < DISPLAY_WIDTH && y0 < DISPLAY_HEIGHT &&
      x1 < DISPLAY_WIDTH && y1 < DISPLAY_HEIGHT) {
    displayDrawFillRect(display, x0, y0, x1, y1, display->_font_fill_color);
  }

  int bits;
  if (_DEBUG_)
    printf("xss=%d yss=%d\n", xss, yss);
  ofs = 0;
  yy = yss;
  xx = xss;
  for (h = 0; h < ph; h++) {
    if (xsd)
      xx = xss;
    if (ysd)
      yy = yss;
    bits = pw;
    for (w = 0; w < ((pw + 4) / 8); w++) {
      mask = 0x80;
      for (bit = 0; bit < 8; bit++) {
        bits--;
        if (bits < 0)
          continue;
        // TODO: fix the problem of underflow properly some time
        if (fonts[ofs] & mask && xx < DISPLAY_WIDTH && yy < DISPLAY_HEIGHT) {
          displayDrawPixel(display, xx, yy, color);
        }
        // TODO: fix the problem of underflow properly some time
        if (h == (ph - 2) && display->_font_underline && xx < DISPLAY_WIDTH &&
            yy < DISPLAY_HEIGHT)
          displayDrawPixel(display, xx, yy, display->_font_underline_color);
        // TODO: fix the problem of underflow properly some time
        if (h == (ph - 1) && display->_font_underline && xx < DISPLAY_WIDTH &&
            yy < DISPLAY_HEIGHT)
          displayDrawPixel(display, xx, yy, display->_font_underline_color);
        xx = xx + xd1;
        yy = yy + yd2;
        mask = mask >> 1;
      }
      ofs++;
    }
    yy = yy + yd1;
    xx = xx + xd2;
  }

  if (next < 0)
    next = 0;
  return next;
}

int displayDrawString(display_t *display, FontxFile *fx, uint16_t x, uint16_t y,
                      uint8_t *ascii, uint16_t color) {
  int length = strlen((char *)ascii);
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayDrawString: display has not been initialized\n");
  }
  if (_DEBUG_)
    printf("displayDrawString length=%d\n", length);
  for (int i = 0; i < length; i++) {
    if (_DEBUG_)
      printf("ascii[%d]=%x x=%d y=%d\n", i, ascii[i], x, y);
    if (display->_font_direction == 0)
      x = displayDrawChar(display, fx, x, y, ascii[i], color);
    if (display->_font_direction == 1)
      y = displayDrawChar(display, fx, x, y, ascii[i], color);
    if (display->_font_direction == 2)
      x = displayDrawChar(display, fx, x, y, ascii[i], color);
    if (display->_font_direction == 3)
      y = displayDrawChar(display, fx, x, y, ascii[i], color);
  }
  if (display->_font_direction == 0)
    return x;
  if (display->_font_direction == 2)
    return x;
  if (display->_font_direction == 1)
    return y;
  if (display->_font_direction == 3)
    return y;
  return 0;
}

void displaySetFontDirection(display_t *display, uint16_t dir) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displaySetFontDirection: display has not been initialized\n");
  }
  display->_font_direction = dir;
}

void displaySetFontFill(display_t *display, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displaySetFontFill: display has not been initialized\n");
  }
  display->_font_fill = true;
  display->_font_fill_color = color;
}

void displayUnsetFontFill(display_t *display) { display->_font_fill = false; }

void displaySetFontUnderLine(display_t *display, uint16_t color) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displaySetFontUnderLine: display has not been initialized\n");
  }
  display->_font_underline = true;
  display->_font_underline_color = color;
}

void displayUnsetFontUnderLine(display_t *display) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayUnsetFontUnderLine: display has not been initialized\n");
  }
  display->_font_underline = false;
}

void displayBacklightOff(display_t *display) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayBacklightOff: display has not been initialized\n");
  }
  if (display->_bl >= 0) {
    gpio_set_level(display->_bl, 0);
  }
}

void displayBacklightOn(display_t *display) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayBacklightOn: display has not been initialized\n");
  }
  if (display->_bl >= 0) {
    gpio_set_level(display->_bl, 1);
  }
}

void displayInversionOff(display_t *display) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayInversionOff: display has not been initialized\n");
  }
  spi_master_write_command(display, 0x21); // display Inversion Off
}

void displayInversionOn(display_t *display) {
  if (display == NULL || display->_width != DISPLAY_WIDTH) {
    pynq_error("displayInversionOn: display has not been initialized\n");
  }
  spi_master_write_command(display, 0x20); // display Inversion On
}
