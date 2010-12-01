/**************************************************************************/
/*! 
    @file     cmd_lcd_test.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Code to execute for cmd_lcd_test in the 'core/cmd'
              command-line interpretter.

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
#include <stdio.h>

#include "projectconfig.h"
#include "core/cmd/cmd.h"
#include "project/commands.h"       // Generic helper functions

#ifdef CFG_TFTLCD    
  #include "drivers/lcd/tft/lcd.h"    
  #include "drivers/lcd/tft/drawing.h"  
  #include "drivers/lcd/tft/touchscreen.h"
  #include "drivers/lcd/tft/fonts/consolas9.h"
  #include "drivers/lcd/tft/fonts/consolas11.h"
  #include "drivers/lcd/tft/fonts/consolas16.h"
  #include "drivers/lcd/smallfonts.h"

/**************************************************************************/
/*! 
    Displays a test pattern on the LCD.
*/
/**************************************************************************/
void cmd_lcd_test(uint8_t argc, char **argv)
{
  // Get 16-bit equivalent of 24-bit color
  uint16_t darkGray = drawRGB24toRGB565(0x33, 0x33, 0x33);
  uint16_t lightGray = drawRGB24toRGB565(0xCC, 0xCC, 0xCC);

  // Draw background
  drawFill(lightGray);
  drawRectangleFilled(0, 0, 239, 19, darkGray);
  drawRectangleFilled(0, 239, 239, 319, darkGray);

  // Render some text
  #if defined CFG_TFTLCD_INCLUDESMALLFONTS & CFG_TFTLCD_INCLUDESMALLFONTS == 1
    drawStringSmall(1, 210, COLOR_WHITE, "5x8 System (Max 40 Characters)", Font_System5x8);
    drawStringSmall(1, 220, COLOR_WHITE, "7x8 System (Max 30 Characters)", Font_System7x8);
  #endif
  
  drawString(5,   8,    COLOR_WHITE,    &consolas9ptFontInfo,   "LPC1343 Demo Code");

  // Draw some primitive shapes
  drawCircle(15, 300, 10, COLOR_WHITE);
  drawLine(100, 280, 200, 310, COLOR_WHITE);
  drawRectangle (220, 5, 230, 15, COLOR_WHITE);
  drawRectangleFilled (222, 7, 228, 13, lightGray);

  // Draw some compound shapes
  drawString(10,   150,    COLOR_BLACK,    &consolas9ptFontInfo,   "Green Progress");
  drawProgressBar(100, 150, 130, 9, COLOR_WHITE, COLOR_BLACK, lightGray, COLOR_GREEN, 75);
  drawString(10,   165,    COLOR_BLACK,    &consolas9ptFontInfo,   "Yellow Progress");
  drawProgressBar(100, 165, 130, 9, COLOR_WHITE, COLOR_BLACK, lightGray, COLOR_YELLOW, 23);
  drawString(10,   180,    COLOR_RED,    &consolas9ptFontInfo,   "Red Progress");
  drawProgressBar(100, 180, 130, 9, COLOR_WHITE, COLOR_BLACK, lightGray, COLOR_RED, 64);
  drawString(10,   200,    COLOR_BLACK,    &consolas9ptFontInfo,   "Battery");
  drawProgressBar(100, 195, 130, 15, COLOR_WHITE, COLOR_BLACK, lightGray, COLOR_BLUE, 90);
}

#endif  
