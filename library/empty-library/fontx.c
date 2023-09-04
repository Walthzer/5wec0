#include <fontx.h>
void AaddFontx(FontxFile *fx, const char *path){};
void InitFontx(FontxFile *fxs, const char *f0, const char *f1){};
bool OpenFontx(FontxFile *fx){};
void CloseFontx(FontxFile *fx){};
void DumpFontx(FontxFile *fxs){};
uint8_t GetFontWidth(FontxFile *fx){};
uint8_t GetFontHeight(FontxFile *fx){};
bool GetFontx(FontxFile *fxs, uint8_t ascii, uint8_t *pGlyph, uint8_t *pw,
              uint8_t *ph){};
void UnderlineBitmap(uint8_t *line, uint8_t w, uint8_t h){};
void ReversBitmap(uint8_t *line, uint8_t w, uint8_t h){};
void ShowFont(uint8_t *fonts, uint8_t pw, uint8_t ph){};
void ShowBitmap(uint8_t *bitmap, uint8_t pw, uint8_t ph){};
uint8_t RotateByte(uint8_t ch){};
