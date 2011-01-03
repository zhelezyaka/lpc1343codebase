/**************************************************************************/
/*! 
    @file     colors.h
    @author   K. Townsend (microBuilder.eu)

    @brief    Common 16-bit RGB565 color definitions

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
#ifndef __COLORS_H__
#define __COLORS_H__

#include "projectconfig.h"

typedef enum
{
  COLORSCHEME_DEFAULT = 0,
  COLORSCHEME_DARKGRAY = 1,
  COLORSCHEME_BLUE = 2
} colorscheme_t;

// Basic Color definitions
#define	COLOR_BLACK         0x0000
#define	COLOR_BLUE          0x001F
#define	COLOR_RED           0xF800
#define	COLOR_GREEN         0x07E0
#define COLOR_CYAN          0x07FF
#define COLOR_MAGENTA       0xF81F
#define COLOR_YELLOW        0xFFE0
#define COLOR_WHITE         0xFFFF

// Grayscale Values
#define COLOR_DARKGRAY      0x18E3    //  30  30  30
#define COLOR_DARKERGRAY    0x528A    //  80  80  80
#define COLOR_MEDIUMGRAY    0x8410    // 128 128 128
#define COLOR_LIGHTGRAY     0xCE59    // 200 200 200
#define COLOR_PALEGRAY      0xE71C    // 225 225 225

// Progress Bar Colors (see drawing.c)
#define COLOR_PROGRESSBAR_BORDER            0x528A    //  80  80  80
#define COLOR_PROGRESSBAR_BACKGROUND1       0xEF5D    // 235 235 235
#define COLOR_PROGRESSBAR_BACKGROUND2       0xCE59    // 200 200 200

#define COLOR_PROGRESSBAR_GRAY_BORDER       0x6BD0    // 110 120 130
#define COLOR_PROGRESSBAR_GRAY_BACKGROUND2  0x4228    //  70  70  70
#define COLOR_PROGRESSBAR_GRAY_BACKGROUND1  0x4228    //  70  70  70

#define COLOR_PROGRESSBAR_BLUE_BORDER       0x018D    //   0  50 110
#define COLOR_PROGRESSBAR_BLUE_BACKGROUND1  0xD73F    // 212 230 255
#define COLOR_PROGRESSBAR_BLUE_BACKGROUND2  0xB6BF    // 178 212 255

// Button Colors (see drawing.c)
#define COLOR_BUTTON_BORDER                 0x528A    //  80  80  80
#define COLOR_BUTTON_HIGHLIGHT              0xFFFF    // 255 255 255
#define COLOR_BUTTON_HIGHLIGHTDARKER        0xB5B6    // 180 180 180
#define COLOR_BUTTON_BACKGROUND1            0xEF5D    // 235 235 235
#define COLOR_BUTTON_BACKGROUND2            0xCE59    // 200 200 200
#define COLOR_BUTTON_BACKGROUNDACTIVE1      0x528A    //  80  80  80
#define COLOR_BUTTON_BACKGROUNDACTIVE2      0x4228    //  70  70  70
#define COLOR_BUTTON_FONT                   0x2945    //  40  40  40
#define COLOR_BUTTON_FONTACTIVE             0xC618    // 195 195 195

#define COLOR_BUTTON_GRAY_BORDER                 0x528A    //  80  80  80
#define COLOR_BUTTON_GRAY_HIGHLIGHT              0xFFFF    // 255 255 255
#define COLOR_BUTTON_GRAY_HIGHLIGHTDARKER        0xB5B6    // 180 180 180
#define COLOR_BUTTON_GRAY_BACKGROUND1            0x528A    //  80  80  80
#define COLOR_BUTTON_GRAY_BACKGROUND2            0x4228    //  70  70  70
#define COLOR_BUTTON_GRAY_BACKGROUNDACTIVE1      0x528A    //  80  80  80
#define COLOR_BUTTON_GRAY_BACKGROUNDACTIVE2      0x4228    //  70  70  70
#define COLOR_BUTTON_GRAY_FONT                   0xC618    // 195 195 195
#define COLOR_BUTTON_GRAY_FONTACTIVE             0x2945    //  40  40  40

#define COLOR_BUTTON_BLUE_BORDER            0x018D  //   0  50 110
#define COLOR_BUTTON_BLUE_HIGHLIGHT         0xB69E  // 179 208 246
#define COLOR_BUTTON_BLUE_HIGHLIGHTDARKER   0x95DC  // 151 184 227
#define COLOR_BUTTON_BLUE_BACKGROUND1       0xD73F  // 212 230 255
#define COLOR_BUTTON_BLUE_BACKGROUND2       0xB6BF  // 178 212 255
#define COLOR_BUTTON_BLUE_BACKGROUNDACTIVE1 0x64BA  // 102 149 215
#define COLOR_BUTTON_BLUE_BACKGROUNDACTIVE2 0x43B7  //  66 116 184
#define COLOR_BUTTON_BLUE_FONT              0x090C  //   8  33  99
#define COLOR_BUTTON_BLUE_FONTACTIVE        0xB6BF  // 178 212 255

#endif
