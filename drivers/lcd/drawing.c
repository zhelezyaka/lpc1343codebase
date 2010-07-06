/**************************************************************************/
/*! 
    @file     drawing.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <string.h>

#include "drawing.h"

/*************************************************/
static void drawCharBitmap(const uint16_t xPixel, const uint16_t yPixel, uint16_t color, const uint8_t *glyph, uint8_t glyphHeightPages, uint8_t glyphWidthBits)
{
  uint16_t verticalPage, horizBit, currentY, currentX;
  uint16_t indexIntoGlyph;

  // set initial current y
  currentY = yPixel;
  currentX = xPixel;

  // for each page of the glyph
  for (verticalPage = glyphHeightPages; verticalPage > 0; --verticalPage)
  {
    // for each horizontol bit
    for (horizBit = 0; horizBit < glyphWidthBits; ++horizBit)
    {
      // next byte
      indexIntoGlyph = (glyphHeightPages * horizBit) + verticalPage - 1;
      
      currentX = xPixel + (horizBit);
      // send the data byte
      // ToDo: This really needs to be optimized!
      if (glyph[indexIntoGlyph] & (0X80)) drawPixel(currentX, currentY, color);
      if (glyph[indexIntoGlyph] & (0X40)) drawPixel(currentX, currentY - 1, color);
      if (glyph[indexIntoGlyph] & (0X20)) drawPixel(currentX, currentY - 2, color);
      if (glyph[indexIntoGlyph] & (0X10)) drawPixel(currentX, currentY - 3, color);
      if (glyph[indexIntoGlyph] & (0X08)) drawPixel(currentX, currentY - 4, color);
      if (glyph[indexIntoGlyph] & (0X04)) drawPixel(currentX, currentY - 5, color);
      if (glyph[indexIntoGlyph] & (0X02)) drawPixel(currentX, currentY - 6, color);
      if (glyph[indexIntoGlyph] & (0X01)) drawPixel(currentX, currentY - 7, color);
    }
    // next line of pages
    currentY += 8;
  }
}

#if defined CFG_LCD_INCLUDESMALLFONTS & CFG_LCD_INCLUDESMALLFONTS == 1
/*************************************************/
static void drawCharSmall(uint16_t x, uint16_t y, uint16_t color, uint8_t c, struct FONT_DEF font)
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
#endif

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
void drawTestPattern(void)
{
  lcdTest();
}


#if defined CFG_LCD_INCLUDESMALLFONTS & CFG_LCD_INCLUDESMALLFONTS == 1
/*************************************************/
void drawStringSmall(uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font)
{
  uint8_t l;
  for (l = 0; l < strlen(text); l++)
  {
    drawCharSmall(x + (l * (font.u8Width + 1)), y, color, text[l], font);
  }
}
#endif

/*************************************************/
void drawString(uint16_t x, uint16_t y, uint16_t color, const FONT_INFO *fontInfo, char *str)
{
  uint16_t currentX, charWidth, characterToOutput;
  const FONT_CHAR_INFO *charInfo;
  uint16_t charOffset;
  
  // set current x, y to that of requested
  currentX = x;

  // while not NULL
  while (*str != 0)
  {
    // get character to output
    characterToOutput = *str;
    
    // get char info
    charInfo = fontInfo->charInfo;
    
    // some fonts have character descriptors, some don't
    if (charInfo != NULL)
    {
      // get correct char offset
      charInfo += (characterToOutput - fontInfo->startChar);
      
      // get width from char info
      charWidth = charInfo->widthBits;
      
      // get offset from char info
      charOffset = charInfo->offset;
    }        
    else
    {
      // if no char info, char width is always 5
      charWidth = 5;
      
      // char offset - assume 5 * letter offset
      charOffset = (characterToOutput - fontInfo->startChar) * 5;
    }        
    
    // Send individual characters
    drawCharBitmap(currentX, y, color, &fontInfo->data[charOffset], fontInfo->heightPages, charWidth);

    // next char X
    currentX += charWidth + 1;
    
    // next char
    str++;
  }
}

/*************************************************/
uint32_t drawGetStringWidth(const FONT_INFO *fontInfo, char *str)
{
  uint32_t currChar, width = 0;
  uint32_t startChar = fontInfo->startChar;

  // until termination
  for (currChar = *str; currChar; currChar = *(++str))
  {
    // if char info exists for the font, use width from there
    if (fontInfo->charInfo != NULL)
    {
      width += fontInfo->charInfo[currChar - startChar].widthBits + 1;
    }
    else
    {
      width += 5 + 1;
    }
  }

  /* return the wdith */
  return width;
}

/*************************************************/
uint16_t drawRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}


