#ifndef MAIN_FONTX_H_
#define MAIN_FONTX_H_
#define FontxGlyphBufSize (32 * 32 / 8)
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup FONTX Font library
 *
 * @brief Do not use. Low-level library to work with bitmap fonts on the
 * display.
 *
 * It provides functionality for loading and manipulating font files, rendering
 * fonts and bitmaps to the screen, and performing various transformations on
 * bitmaps. The library also includes a struct, FontxFile, which represents a
 * font file and contains metadata about the font.
 *
 * This is an internal library and should not be directly used.
 *
 * @{
 */

typedef struct _IO_FILE FILE;

/**
 * @brief Struct representing a font file.
 */
typedef struct {
  const char *path; /**< Path to the font file. */
  char fxname[10];  /**< Name of the font file. */
  bool opened;      /**< Flag indicating whether the font file is open. */
  bool valid;       /**< Flag indicating whether the font file is valid. */
  bool is_ank;  /**< Flag indicating whether the font file contains only ASCII
                   characters. */
  uint8_t w;    /**< Width of each character in the font file. */
  uint8_t h;    /**< Height of each character in the font file. */
  uint16_t fsz; /**< Size of the font file in bytes. */
  uint8_t bc;   /**< Background color of the font file. */
  FILE *file;   /**< Pointer to the font file stream. */
} FontxFile;

/**
 * @param fx Pointer to the FontxFile structure
 * @param path Path to the font file
 *
 * @brief Adds a font file to the given FontxFile structure
 */
void AaddFontx(FontxFile *fx, const char *path);

/**
 * @param fxs Pointer to the FontxFile structure
 * @param f0 Path to the 8x16 font file
 * @param f1 Path to the 16x16 font file
 *
 * @brief Initializes the given FontxFile structure with the specified font
 * files
 */
void InitFontx(FontxFile *fxs, const char *f0, const char *f1);

/**
 * @param fx Pointer to the FontxFile structure
 *
 * @brief Opens the font file and reads the font data into the FontxFile
 * structure
 *
 * @return True if the font file was opened successfully, false otherwise
 *
 * @warning The font-file path must be valid from the directory in which
 * the executable is called, otherwise the error message
 * "cannot get font from font file" will be thrown.
 * Absolute paths (starting with /) are safe.
 */
bool OpenFontx(FontxFile *fx);

/**
 * @param fx Pointer to the FontxFile structure
 *
 * @brief Closes the font file
 */
void CloseFontx(FontxFile *fx);

/**
 * @param fxs Pointer to the FontxFile structure
 *
 * @brief Dumps the font data stored in the FontxFile structure
 */
void DumpFontx(FontxFile *fxs);

/**
 * @param fx Pointer to the FontxFile structure
 *
 * @brief Gets the width of a character in the font
 *
 * @return The width of a character in pixels
 */
uint8_t GetFontWidth(FontxFile *fx);

/**
 * @param fx Pointer to the FontxFile structure
 *
 * @brief Gets the height of a character in the font
 *
 * @return The height of a character in pixels
 */
uint8_t GetFontHeight(FontxFile *fx);

/**
 * @param fxs Pointer to the FontxFile structure
 * @param ascii ASCII value of the character to get the glyph for
 * @param pGlyph Pointer to the buffer to store the glyph data
 * @param pw Pointer to the variable to store the width of the glyph
 * @param ph Pointer to the variable to store the height of the glyph
 *
 * @brief Gets the glyph data for the specified ASCII character
 *
 * @return True if the glyph was found, false otherwise
 */
bool GetFontx(FontxFile *fxs, uint8_t ascii, uint8_t *pGlyph, uint8_t *pw,
              uint8_t *ph);

/**
 * @param fonts Pointer to the font data buffer
 * @param line Pointer to the bitmap buffer
 * @param w Width of the bitmap in pixels
 * @param h Height of the bitmap in pixels
 * @param inverse If true, the bitmap will be inverted
 *
 * @brief Converts a font data buffer into a bitmap
 */
void Font2Bitmap(uint8_t *fonts, uint8_t *line, uint8_t w, uint8_t h,
                 uint8_t inverse);

/**
 * @param line Pointer to the bitmap buffer
 * @param w Width of the bitmap in pixels
 * @param h Height of the bitmap in pixels
 *
 * @brief Adds an underline to a bitmap
 */
void UnderlineBitmap(uint8_t *line, uint8_t w, uint8_t h);

/**
 * @param line Pointer to the bitmap buffer
 * @param w Width of the bitmap in pixels
 * @param h Height of the bitmap in pixels
 *
 * @brief Reverses the bits in each byte of a bitmap
 */
void ReversBitmap(uint8_t *line, uint8_t w, uint8_t h);

/**
 * @param fonts Pointer to the font buffer
 * @param pw Width of the font in pixels
 * @param ph Height of the font in pixels
 *
 * @brief Displays a font on the screen
 */
void ShowFont(uint8_t *fonts, uint8_t pw, uint8_t ph);

/**
 * @param bitmap Pointer to the bitmap buffer
 * @param pw Width of the font in pixels
 * @param ph Height of the font in pixels
 *
 * @brief Displays a bitmap on the screen
 */
void ShowBitmap(uint8_t *bitmap, uint8_t pw, uint8_t ph);

/**
 * @param ch Byte to be rotated
 *
 * @brief Rotates a byte by 90 degrees
 *
 * @return The rotated byte
 */
uint8_t RotateByte(uint8_t ch);

/**
 * @}
 */

#endif
