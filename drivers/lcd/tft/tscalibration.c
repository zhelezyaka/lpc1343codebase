/**************************************************************************/
/*! 
    @file     tscalibration.c
    @author   K. Townsend (microBuilder.eu)

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
#include "tscalibration.h"

#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/touchscreen.h"
#include "drivers/lcd/tft/fonts/consolas11.h"

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Centers a line of text horizontally
*/
/**************************************************************************/
static void centerText(char* text, uint16_t y, uint16_t color)
{
  drawString((240 - drawGetStringWidth(&consolas11ptFontInfo, text)) / 2, y, color, &consolas11ptFontInfo, text);
}

/**************************************************************************/
/*!
    @brief  Draws a circular test point
*/
/**************************************************************************/
static void drawTestPoint(uint16_t x, uint16_t y, uint16_t radius)
{
  drawCircle(x, y, radius, COLOR_RED);
  drawCircle(x, y, radius + 2, COLOR_MEDIUMGRAY);
}

/**************************************************************************/
/*!
    @brief  Displays the calibration welcome screen
*/
/**************************************************************************/
static void tscWelcome(void)
{
  // Clear the screen
  drawRectangleFilled(0, 0, 239, 319, COLOR_WHITE);

  // Display text
  centerText("To begin calibrating the", 50, COLOR_DARKGRAY);
  centerText("touch screen please click", 65, COLOR_DARKGRAY);
  centerText("on the center dot", 80, COLOR_DARKGRAY);

  // Draw test circle
  drawTestPoint(120, 160, 5);

  // Redraw the cancel button
  drawButton(20, 200, 200, 60, &consolas11ptFontInfo, 7, "Cancel", FALSE);
}

/**************************************************************************/
/*!
    @brief  Checks the four corners
*/
/**************************************************************************/
static void tscCheckCorners(void)
{
  // ToDo
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Starts the touch screen calibration process
*/
/**************************************************************************/
void tscalibrationStart(void)
{
  tsTouchData_t data;

  // Show calibration start screen
  tscWelcome();
  tsWaitForEvent(&data);

  // Get corner offsets
  tscCheckCorners();
}
