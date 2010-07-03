#ifndef __DRAWING_H__
#define __DRAWING_H__

#include "projectconfig.h"
#include "lcd.h"
#include "fonts.h"

void drawPixel(uint16_t x, uint16_t y, uint16_t color);
void drawFill(uint16_t color);
void drawChar(uint16_t x, uint16_t y, uint16_t color, uint8_t c, struct FONT_DEF font);
void drawString(uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font);
void drawBatteryLevel(uint16_t x, uint16_t y, uint16_t color, uint8_t percent);

#endif
