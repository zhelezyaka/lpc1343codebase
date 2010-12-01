/**************************************************************************/
/*! 
    @file     main.c
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
#include <stdio.h>

#include "projectconfig.h"
#include "sysinit.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

#ifdef CFG_TFTLCD
  #include "drivers/lcd/tft/drawing.h"  
  #include "drivers/lcd/tft/touchscreen.h"
  #include "drivers/lcd/tft/fonts/consolas9.h"
  #include "drivers/lcd/tft/fonts/consolas16.h"
#endif

/**************************************************************************/
/*! 
    Approximates a 1 millisecond delay using "nop".  This is less
    accurate than a dedicated timer, but is useful in certain situations.

    The number of ticks to delay depends on the optimisation level set
    when compiling (-O).  Depending on the compiler settings, one of the
    two defined values for 'delay' should be used.
*/
/**************************************************************************/
void delayms(uint32_t ms)
{
  uint32_t delay = ms * ((CFG_CPU_CCLK / 100) / 80);      // Release Mode (-Os)
  // uint32_t delay = ms * ((CFG_CPU_CCLK / 100) / 140);  // Debug Mode (No optimisations)

  while (delay > 0)
  {
    __asm volatile ("nop");
    delay--;
  }
}

/**************************************************************************/
/*! 
    Main program entry point.  After reset, normal code execution will
    begin here.

    Note: CFG_INTERFACE is normally enabled by default.  If you wish to
          enable the blinking LED code in main, you will need to open
          projectconfig.h, comment out "#define CFG_INTERFACE" and
          rebuild the project.
*/
/**************************************************************************/
int main (void)
{
  // Configure cpu and mandatory peripherals
  systemInit();

  #ifdef CFG_TFTLCD
    // Draw background
    drawRectangleFilled(0, 0, 239, 319, COLOR_BLACK);

    #ifdef CFG_SDCARD
      // Show five 48x48 icons along bottom for toolbar
      drawImageFromFile(0, 272, "/icon48.pic");
      drawImageFromFile(48, 272, "/icon48.pic");
      drawImageFromFile(96, 272, "/icon48.pic");
      drawImageFromFile(144, 272, "/icon48.pic");
      drawImageFromFile(192, 272, "/icon48.pic");
  
      // Render a larger icon in the middle (128x128)
      // drawImageFromFile(56, 120, "/consol~1.pic");
  
      // Render the time using number icons
      drawImageFromFile(0, 120, "/clock/0.pic");
      drawImageFromFile(48, 120, "/clock/6.pic");
      drawImageFromFile(96, 120, "/clock/colon.pic");
      drawImageFromFile(144, 120, "/clock/2.pic");
      drawImageFromFile(192, 120, "/clock/7.pic");
    #endif
  
    // Show some buttons
    // ToDo: Visual appearance needs to be improved, better corners and color etc.
    drawButton(20, 10, 200, 35, &consolas16ptFontInfo, 16, "Button (Released)", FALSE);
    drawButton(20, 50, 200, 35, &consolas16ptFontInfo, 16, "Button (Pressed)", TRUE);

    // Draw a simple progress bar and some smaller text
    uint16_t lightGray = drawRGB24toRGB565(0xCC, 0xCC, 0xCC);
    drawString(10, 100, COLOR_WHITE, &consolas9ptFontInfo, "Battery");
    drawProgressBar(100, 95, 130, 15, COLOR_WHITE, COLOR_BLACK, lightGray, COLOR_BLUE, 90);
  #endif

  while (1)
  {
    #ifdef CFG_INTERFACE
      // Handle any incoming command line input
      cmdPoll();
    #else
      // Toggle LED @ 1 Hz
      systickDelay(1000);
      if (gpioGetValue(CFG_LED_PORT, CFG_LED_PIN))  
        gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON);
      else 
        gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF);
    #endif
  }
}
