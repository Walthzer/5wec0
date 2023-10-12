/*
Copyright (c) 2023 Eindhoven University of Technology

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
*/
#ifndef LCDCONFIG_H
#define LCDCONFIG_H

#include <pinmap.h>
/**
 * @defgroup LCD Pin Config
 *
 * @brief This library is used to make the pins of the LCD easily changable so that 
 * we can easily and quickly change the pinout of the SPI-driven LCD on the board.
 *
 * @{
 */

#define LCD_MOSI    IO_RBPI33 /* AR_SDA */
#define LCD_SCLK    IO_RBPI35 /* AR_SCL */
#define LCD_CS      IO_RBPI36 /* AR13 */
#define LCD_DC      IO_RBPI37 /* AR12 */
#define LCD_RESET   IO_RBPI38 /* AR11 */
#define LCD_BL      IO_RBPI40 /* AR10 */

/**
 * @}
 */

#endif
