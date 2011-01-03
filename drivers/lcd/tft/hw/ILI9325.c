/**************************************************************************/
/*! 
    @file     ILI9325.c
    @author   K. Townsend (microBuilder.eu)

    @section  DESCRIPTION

    Driver for ILI9325 240x320 pixel TFT LCD displays.
    
    This driver uses an 8-bit interface and a 16-bit RGB565 colour palette.
    Should also work with SPFD5408B, ST7783 or OTM3225A-based LCDs, though
    there are sometimes minor differences (for example vertical scrolling
    via register 0x6A isn't supported with the ST7783 controller).

    @section  UPDATES

    26-11-2010: ili9325ReadData contributed by Adafruit Industries

    @section  LICENSE

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
#include "ILI9325.h"
#include "core/systick/systick.h"
#include "drivers/lcd/tft/touchscreen.h"

/*************************************************/
/* Private Methods                               */
/*************************************************/

/*************************************************/
void ili9325Delay(unsigned int t)
{
  unsigned char t1;
  while(t--)
  for ( t1=10; t1 > 0; t1-- )
  {
    __asm("nop");
  }
}

/*************************************************/
void ili9325WriteCmd(uint16_t command) 
{
  // Compiled with -Os on GCC 4.4 this works out to 25 cycles
  // (versus 36 compiled with no optimisations).  I'm not sure it
  // can be improved further, so that means 25 cycles/350nS for
  // continuous writes (cmd, data, data, data, ...) or ~150 cycles/
  // ~2.1uS for a random pixel (Set X [cmd+data], Set Y [cmd+data],
  // Set color [cmd+data]) (times assumes 72MHz clock).

  CLR_CS_CD_SET_RD_WR;  // Saves 18 commands compared to "CLR_CS; CLR_CD; SET_RD; SET_WR;" 
  ILI9325_GPIO2DATA_DATA = (command >> (8 - ILI9325_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  ILI9325_GPIO2DATA_DATA = command << ILI9325_DATA_OFFSET;
  CLR_WR;
  SET_WR_CS;            // Saves 7 commands compared to "SET_WR; SET_CS;"
}

/*************************************************/
void ili9325WriteData(uint16_t data)
{
  CLR_CS_SET_CD_RD_WR;  // Saves 18 commands compared to SET_CD; SET_RD; SET_WR; CLR_CS"
  ILI9325_GPIO2DATA_DATA = (data >> (8 - ILI9325_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  ILI9325_GPIO2DATA_DATA = data << ILI9325_DATA_OFFSET;
  CLR_WR;
  SET_WR_CS;            // Saves 7 commands compared to "SET_WR, SET_CS;"
}

/*************************************************/
uint16_t ili9325ReadData(void)
{
  // ToDo: Optimise this method!

  uint16_t high, low;
  high = low = 0;
  uint16_t d;

  SET_CD_RD_WR;   // Saves 14 commands compared to "SET_CD; SET_RD; SET_WR"
  CLR_CS;
  
  // set inputs
  ILI9325_GPIO2DATA_SETINPUT;
  CLR_RD;
  ili9325Delay(100);
  high = ILI9325_GPIO2DATA_DATA;  
  high >>= ILI9325_DATA_OFFSET;
  high &= 0xFF;
  SET_RD;
  
  CLR_RD;
  ili9325Delay(100);
  low = ILI9325_GPIO2DATA_DATA;
  low >>= ILI9325_DATA_OFFSET;
  low &=0xFF;
  SET_RD;
  
  SET_CS;
  ILI9325_GPIO2DATA_SETOUTPUT;

  d = high;
  d <<= 8;
  d |= low;
  
  return d;
}

/*************************************************/
uint16_t ili9325Read(uint16_t addr)
{
  ili9325WriteCmd(addr);
  return ili9325ReadData();
}

/*************************************************/
void ili9325Command(uint16_t command, uint16_t data)
{
  // Provided for convenience sake ... shouldn't be used
  // in critical sections since it adds an extra
  // branch, etc.
  ili9325WriteCmd(command);
  ili9325WriteData(data);
}

/*************************************************/
uint16_t ili9325BGR2RGB(uint16_t color)   
{   
  uint16_t r, g, b;   
   
  b = (color>>0)  & 0x1f;   
  g = (color>>5)  & 0x3f;   
  r = (color>>11) & 0x1f;   
     
  return( (b<<11) + (g<<5) + (r<<0) );
}  

/*************************************************/
/* Returns the 4-hexdigit controller code         */
/*************************************************/
uint16_t ili9325Type(void)
{
  ili9325WriteCmd(0x0);
  return ili9325ReadData();
}

/*************************************************/
void ili9325SetCursor(uint16_t x, uint16_t y)
{
  ili9325Command(0x0020, x);       // GRAM Address Set (Horizontal Address) (R20h)
  ili9325Command(0x0021, y);       // GRAM Address Set (Vertical Address) (R21h)
}

/*************************************************/
void ili9325InitDisplay(void)
{
  // Clear data line
  GPIO_GPIO2DATA &= ~ILI9325_DATA_MASK;
    
  SET_RD;
  SET_WR;
  SET_CS;
  SET_CD;

  // Reset display
  CLR_RESET;
  ili9325Delay(10000);
  SET_RESET;
  ili9325Delay(500);

  ili9325Command(0x00FF, 0x0001);
  ili9325Command(0x00F3, 0x0008);
  ili9325WriteCmd(0x00F3);

  ili9325Command(0x0001, 0x0100);     // Driver Output Control Register (R01h)
  ili9325Command(0x0002, 0x0700);     // LCD Driving Waveform Control (R02h)
  ili9325Command(0x0003, 0x1030);     // Entry Mode (R03h)  
  ili9325Command(0x0008, 0x0302);
  ili9325Command(0x0009, 0x0000);
  ili9325Command(0x0010, 0x0000);     // Power Control 1 (R10h)
  ili9325Command(0x0011, 0x0007);     // Power Control 2 (R11h)  
  ili9325Command(0x0012, 0x0000);     // Power Control 3 (R12h)
  ili9325Command(0x0013, 0x0000);     // Power Control 4 (R13h)
  ili9325Delay(1000);  
  ili9325Command(0x0010, 0x14B0);     // Power Control 1 (R10h)  
  ili9325Delay(500);  
  ili9325Command(0x0011, 0x0007);     // Power Control 2 (R11h)  
  ili9325Delay(500);  
  ili9325Command(0x0012, 0x008E);     // Power Control 3 (R12h)
  ili9325Command(0x0013, 0x0C00);     // Power Control 4 (R13h)
  ili9325Command(0x0029, 0x0015);     // NVM read data 2 (R29h)
  ili9325Delay(500);
  ili9325Command(0x0030, 0x0000);     // Gamma Control 1
  ili9325Command(0x0031, 0x0107);     // Gamma Control 2
  ili9325Command(0x0032, 0x0000);     // Gamma Control 3
  ili9325Command(0x0035, 0x0203);     // Gamma Control 6
  ili9325Command(0x0036, 0x0402);     // Gamma Control 7
  ili9325Command(0x0037, 0x0000);     // Gamma Control 8
  ili9325Command(0x0038, 0x0207);     // Gamma Control 9
  ili9325Command(0x0039, 0x0000);     // Gamma Control 10
  ili9325Command(0x003C, 0x0203);     // Gamma Control 13
  ili9325Command(0x003D, 0x0403);     // Gamma Control 14
  ili9325Command(0x0050, 0x0000);     // Window Horizontal RAM Address Start (R50h)
  ili9325Command(0x0051, 0x00EF);     // Window Horizontal RAM Address End (R51h)
  ili9325Command(0x0052, 0X0000);     // Window Vertical RAM Address Start (R52h)
  ili9325Command(0x0053, 0x013F);     // Window Vertical RAM Address End (R53h)
  ili9325Command(0x0060, 0xa700);     // Driver Output Control (R60h)
  ili9325Command(0x0061, 0x0001);     // Driver Output Control (R61h)
  ili9325Command(0x0090, 0X0029);     // Panel Interface Control 1 (R90h)

  // Display On
  ili9325Command(0x0007, 0x0133);     // Display Control (R07h)
  ili9325Delay(500);
  ili9325WriteCmd(0x0022);
}

/*************************************************/
void ili9325Home(void)
{
  ili9325Command(0x0020, 0X0000);     // GRAM Address Set (Horizontal Address) (R20h)
  ili9325Command(0x0021, 0X0000);     // GRAM Address Set (Vertical Address) (R21h)
  ili9325WriteCmd(0x0022);            // Write Data to GRAM (R22h)
}

/*************************************************/
void ili9325SetWindow(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1)
{
  ili9325Command(0x0050, x);       // Window Horizontal RAM Address Start (R50h)
  ili9325Command(0x0051, x1);      // Window Horizontal RAM Address End (R51h)
  ili9325Command(0x0052, y);       // Window Vertical RAM Address Start (R52h) )
  ili9325Command(0x0053, y1);      // Window Vertical RAM Address End (R53h)
}

/*************************************************/
/* Public Methods                                */
/*************************************************/

/*************************************************/
void lcdInit(void)
{
  // Set control line pins to output
  gpioSetDir(ILI9325_CS_PORT, ILI9325_CS_PIN, 1);
  gpioSetDir(ILI9325_CD_PORT, ILI9325_CD_PIN, 1);
  gpioSetDir(ILI9325_WR_PORT, ILI9325_WR_PIN, 1);
  gpioSetDir(ILI9325_RD_PORT, ILI9325_RD_PIN, 1);
  
  // Set data port pins to output
  ILI9325_GPIO2DATA_SETOUTPUT;

  // Disable pullups
  ILI9325_DISABLEPULLUPS();
  
  // Set backlight pin to input and turn it on
  gpioSetDir(ILI9325_BL_PORT, ILI9325_BL_PIN, 1);      // set to output
  lcdBacklightOn();

  // Set reset pin to output
  gpioSetDir(ILI9325_RES_PORT, ILI9325_RES_PIN, 1);    // Set to output
  gpioSetValue(ILI9325_RES_PORT, ILI9325_RES_PIN, 0);  // Low to reset
  systickDelay(50);
  gpioSetValue(ILI9325_RES_PORT, ILI9325_RES_PIN, 1);  // High to exit

  // Initialize the display
  ili9325InitDisplay();

  // Fill black
  lcdFillRGB(COLOR_BLACK);
  
  // Initialise the touch screen (and calibrate if necessary)
  tsInit();
}

/*************************************************/
void lcdBacklightOn(void)
{
  // Enable backlight
  gpioSetValue(ILI9325_BL_PORT, ILI9325_BL_PIN, 0);
}

/*************************************************/
void lcdBacklightOff(void)
{
  // Disable backlight
  gpioSetValue(ILI9325_BL_PORT, ILI9325_BL_PIN, 1);
}

/*************************************************/
void lcdTest(void)
{
  uint32_t i,j;
  ili9325Home();
  
  for(i=0;i<320;i++)
  {
    for(j=0;j<240;j++)
    {
      if(i>279)ili9325WriteData(COLOR_WHITE);
      else if(i>239)ili9325WriteData(COLOR_BLUE);
      else if(i>199)ili9325WriteData(COLOR_GREEN);
      else if(i>159)ili9325WriteData(COLOR_CYAN);
      else if(i>119)ili9325WriteData(COLOR_RED);
      else if(i>79)ili9325WriteData(COLOR_MAGENTA);
      else if(i>39)ili9325WriteData(COLOR_YELLOW);
      else ili9325WriteData(COLOR_BLACK);
    }
  }
}

/*************************************************/
void lcdFillRGB(uint16_t data)
{
  unsigned int i;
  ili9325Home();
  
  uint32_t pixels = 320*240;
  for ( i=0; i < pixels; i++ )
  {
    ili9325WriteData(data);
  } 
}

/*************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  ili9325WriteCmd(0x0020); // GRAM Address Set (Horizontal Address) (R20h)
  ili9325WriteData(x);
  ili9325WriteCmd(0x0021); // GRAM Address Set (Vertical Address) (R21h)
  ili9325WriteData(y);
  ili9325WriteCmd(0x0022);  // Write Data to GRAM (R22h)
  ili9325WriteData(color);
}

/*************************************************/
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  // Allows for slightly better performance than setting individual pixels
  uint16_t x, pixels;

  if (x1 < x0)
  {
    // Switch x1 and x0
    x = x1;
    x1 = x0;
    x0 = x;
  }
  ili9325WriteCmd(0x0020); // GRAM Address Set (Horizontal Address) (R20h)
  ili9325WriteData(x0);
  ili9325WriteCmd(0x0021); // GRAM Address Set (Vertical Address) (R21h)
  ili9325WriteData(y);
  ili9325WriteCmd(0x0022);  // Write Data to GRAM (R22h)
  for (pixels = 0; pixels < x1 - x0 + 1; pixels++)
  {
    ili9325WriteData(color);
  }
}

/*************************************************/
uint16_t lcdGetPixel(uint16_t x, uint16_t y)
{
  uint16_t preFetch = 0;

  ili9325SetCursor(x, y);
  ili9325WriteCmd(0x0022);
  preFetch = ili9325ReadData();

  // Eeek ... why does this need to be done twice for a proper value?!?
  ili9325SetCursor(x, y);
  ili9325WriteCmd(0x0022);
  return ili9325ReadData();
}

/*************************************************/
void lcdScroll(int16_t pixels, uint16_t fillColor)
{
  // Note: Not all ILI9325 imitations support HW vertical scrolling!
  // ST7781 - Not supported (ex. RFTechWorld 2.8" displays)
  // OTM3225A - Supported
  int16_t y = pixels;
  while (y < 0)
    y += 320;
  while (y >= 320)
    y -= 320;
  ili9325WriteCmd(0x6A);
  ili9325WriteData(y);
}
