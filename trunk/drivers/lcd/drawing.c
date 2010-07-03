#include <string.h>

#include "drawing.h"

/*************************************************/
void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  // Redirect to LCD
  lcdDrawPixel(x, y, color);
}

/*************************************************/
void drawFill(uint16_t color)
{
  lcdFillRGB(color);
}

/*************************************************/
void drawChar(uint16_t x, uint16_t y, uint16_t color, uint8_t c, struct FONT_DEF font)
{
  uint8_t col, column[font.u8Width];

  // Check if the requested character is available
  if ((c >= font.u8FirstChar) && (c <= font.u8LastChar))
  {
    // Retrieve appropriate columns from font data
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = font.au8FontTable[((c - 32) * font.u8Width) + col];    // Get first column of appropriate character
    }
  }
  else
  {    
    // Requested character is not available in this font ... send a space instead
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = 0xFF;    // Send solid space
    }
  }

  // Render each column
  uint16_t xoffset, yoffset;
  for (xoffset = 0; xoffset < font.u8Width; xoffset++)
  {
    for (yoffset = 0; yoffset < (font.u8Height + 1); yoffset++)
    {
      uint8_t bit = 0x00;
      bit = (column[xoffset] << (8 - (yoffset + 1)));     // Shift current row bit left
      bit = (bit >> 7);                     // Shift current row but right (results in 0x01 for black, and 0x00 for white)
      if (bit)
      {
        drawPixel(x + xoffset, y + yoffset, color);
      }
    }
  }
}

/*************************************************/
void drawString(uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font)
{
  uint8_t l;
  for (l = 0; l < strlen(text); l++)
  {
    drawChar(x + (l * (font.u8Width + 1)), y, color, text[l], font);
  }
}
