/**************************************************************************/
/*! 
    @file     SPFD5420A.c
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
#include "SPFD5420A.h"

/*************************************************/
static void spfd5420WriteCmd(uint16_t command)
{
  CLR_CS_CD;      // Saves 7 commands compared to "CLR_CS; CLR_CD;"
  SET_RD_WR;      // Saves 7 commands compared to "SET_RD; SET_WR;"
  SPFD5420_GPIO2DATA_DATA = (command >> (8 - SPFD5420_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  SPFD5420_GPIO2DATA_DATA = command << SPFD5420_DATA_OFFSET;
  CLR_WR;
  SET_WR_CS;      // Saves 7 commands compared to "SET_WR; SET_CS;"
}

/*************************************************/
static void spfd5420WriteData(uint16_t data)
{
  CLR_CS;
  SET_CD_RD_WR;   // Saves 14 commands compared to "SET_CD; SET_RD; SET_WR"
  SPFD5420_GPIO2DATA_DATA = (data >> (8 - SPFD5420_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  SPFD5420_GPIO2DATA_DATA = data << SPFD5420_DATA_OFFSET;
  CLR_WR;
  SET_WR_CS;      // Saves 7 commands compared to "SET_WR, SET_CS;"
  
  // Necessary ?
  SPFD5420_GPIO2DATA_DATA = 0xFFFF;
}

/*************************************************/
static void spfd5420Command(uint16_t command, uint16_t data)
{
  // Provided for convenience sake ... shouldn't be used
  // in critical sections since it adds an extra
  // branch, etc.
  spfd5420WriteCmd(command);
  spfd5420WriteData(data);
}

/*************************************************/
static void spfd5420Delay(unsigned int t)
{
  unsigned char t1;
  while(t--)
  for ( t1=10; t1 > 0; t1-- )
  {
    __asm("nop");
  }
}

/*************************************************/
static void spfd5420InitDisplay(void)
{
  // Clear data line
  GPIO_GPIO2DATA &= ~SPFD5420_DATA_MASK;
    
  SET_RD;
  SET_WR;
  SET_CS;
  SET_CD;

  // Reset display
  CLR_RESET;
  spfd5420Delay(10000);
  spfd5420Delay(10000);
  SET_RESET;
  spfd5420Delay(500);

  spfd5420Command(0x0606, 0x0000);    // Pin Control (R606h)
  spfd5420Delay(50);
  spfd5420Command(0x0007, 0x0001);    // Display Control 1 (R007h)
  spfd5420Delay(50);
  spfd5420Command(0x0110, 0x0001);    // Power Control 6(R110h)
  spfd5420Delay(50);
  spfd5420Command(0x0100, 0x17B0);    // Power Control 1 (R100h)
  spfd5420Command(0x0101, 0x0147);    // Power Control 2 (R101h)
  spfd5420Command(0x0102, 0x019D);    // Power Control 3 (R102h)
  spfd5420Command(0x0103, 0x3600);    // Power Control 4 (R103h)
  spfd5420Command(0x0281, 0x0010);    // NVM read data 2 (R281h)  
  spfd5420Delay(50);
  spfd5420Command(0x0102, 0x01BD);    // Power Control 3 (R102h)
  spfd5420Delay(50);

  //--------------- Power control 1~6 ---------------//
  spfd5420Command(0x0100, 0x16B0);    // Power Control 1 (R100h)
  spfd5420Command(0x0101, 0x0147);    // Power Control 2 (R101h)
  spfd5420Command(0x0102, 0x01BD);    // Power Control 3 (R102h)
  spfd5420Command(0x0103, 0x2d00);    // Power Control 4 (R103h)
  spfd5420Command(0x0107, 0x0000);    // Power Control 5 (R107h)
  spfd5420Command(0x0110, 0x0001);    // Power Control 6(R110h)
  spfd5420Command(0x0280, 0x0000);    // NVM read data 1 (R280h)
  spfd5420Command(0x0281, 0x0006);    // NVM read data 2 (R281h)
  spfd5420Command(0x0282, 0x0000);    // NVM read data 3 (R282h)

  //------- Gamma 2.2 control (R300h to R30Fh) ------//
  spfd5420Command(0x0300, 0X0101); 
  spfd5420Command(0x0301, 0x0bB7); 
  spfd5420Command(0x0302, 0x132A); 
  spfd5420Command(0x0303, 0x2A13); 
  spfd5420Command(0x0304, 0x270B); 
  spfd5420Command(0x0305, 0x0101); 
  spfd5420Command(0x0306, 0x1205); 
  spfd5420Command(0x0307, 0x0512); 
  spfd5420Command(0x0308, 0X0005); 
  spfd5420Command(0x0309, 0x0003); 
  spfd5420Command(0x030A, 0X0F04); 
  spfd5420Command(0x030B, 0x0F00); 
  spfd5420Command(0x030C, 0x000F); 
  spfd5420Command(0x030D, 0x040F);    
  spfd5420Command(0x030E, 0x0300);    
  spfd5420Command(0x030F, 0x0500);    

  spfd5420Command(0x0400, 0x3500);    // Base Image Number of Line (R400h)
  spfd5420Command(0x0401, 0x0001);    // Base Image Display Control (R401h)
  spfd5420Command(0x0404, 0x0000);    // Based Image Vertical Scroll Control (R404h)

  //--------------- Normal set ---------------//
  spfd5420Command(0x0000, 0x0000);    // ID Read Register (R000h)
  spfd5420Command(0x0001, 0x0100);    // Driver Output Control Register (R001h)
  spfd5420Command(0x0002, 0x0100);    // LCD Driving Waveform Control (R002h)
  spfd5420Command(0x0003, 0x1030);    // Entry Mode (R003h)
  spfd5420Command(0x0006, 0x0000);    // Display Control 1 (R007h)
  spfd5420Command(0x0008, 0x0808);    // Display Control 2 (R008h)
  spfd5420Command(0x0009, 0x0001);    // Display Control 3 (R009h)
  spfd5420Command(0x000B, 0x0010);    // Low Power Control (R00Bh)
  spfd5420Command(0x000C, 0X0000);    // External Display Interface Control 1 (R00Ch)
  spfd5420Command(0x000F, 0X0000);    // External Display Interface Control 2 (R00Fh)
  spfd5420Command(0x0007, 0X0001);    // Display Control 1 (R007h)

  //--------------- Panel interface control 1~6 ---------------//
  spfd5420Command(0x0010, 0x0012);    // Panel Interface Control 1 (R010h)
  spfd5420Command(0x0011, 0x0202);    // Panel Interface Control 2 (R011h)
  spfd5420Command(0x0012, 0x0300);    // Panel Interface control 3 (R012h)
  spfd5420Command(0x0020, 0x021E);    // Panel Interface control 4 (R020h)
  spfd5420Command(0x0021, 0x0202);    // Panel Interface Control 5 (021Rh)
  spfd5420Command(0x0022, 0x0100);    // Panel Interface Control 6 (R022h)
  spfd5420Command(0x0090, 0x8000);    // Frame Marker Control (R090h)

  //--------------- Partial display ---------------//
  spfd5420Command(0x0210, 0x0000);    // Window Horizontal RAM Address Start (R210h)
  spfd5420Command(0x0211, 0x00EF);    // Window Horziontal RAM Address End (R211h)
  spfd5420Command(0x0212, 0x0000);    // Window Vertical RAM Address Start (R212h)
  spfd5420Command(0x0213, 0x018F);    // Window Vertical RAM Address End (R213h)
  spfd5420Command(0x0500, 0X0000);    // Display Position - Partial Display 1 (R500h)
  spfd5420Command(0x0501, 0X0000);    // RAM Address Start - Partial Display 1 (R501h)
  spfd5420Command(0x0502, 0X0000);    // RAM Address End - Partail Display 1 (R502h)
  spfd5420Command(0x0503, 0x0000);    // Display Position - Partial Display 2 (R503h)
  spfd5420Command(0x0504, 0x0000);    // RAM Address Start – Partial Display 2 (R504h)
  spfd5420Command(0x0505, 0x0000);    // RAM Address End – Partial Display 2 (R505h)
  spfd5420Command(0x0606, 0x0000);    // Pin Control (R606h)
  spfd5420Command(0x06F0, 0X0000);    // NVM Access Control (R6F0h)

  //--------------- Display on --------------------//
  spfd5420Command(0x0007, 0x0173);    // Display Control 1 (R007h)
  spfd5420Delay(50);
  spfd5420Command(0x0007, 0x0171);    // Display Control 1 (R007h)
  spfd5420Delay(10);
  spfd5420Command(0x0007, 0x0173);    // Display Control 1 (R007h)    
}

/*************************************************/
void spfd5420Home(void)
{
  spfd5420Command(0x0200, 0x0000);    // GRAM Address Set (Horizontal Address) (R20h)
  spfd5420Command(0x0201, 0X0000);    // GRAM Address Set (Vertical Address) (R21h)
  spfd5420WriteCmd(0x0202);           // Write Data to GRAM (R22h)
}

/*************************************************/
void spfd5420SetCursor(uint16_t x, uint16_t y)
{
  spfd5420Command(0x0200, x-1);       // GRAM Address Set (Horizontal Address) (R20h)
  spfd5420Command(0x0201, y-1);       // GRAM Address Set (Vertical Address) (R21h)
}

/*************************************************/
void lcdInit(void)
{
  // Set control line pins to output
  gpioSetDir(SPFD5420_CS_PORT, SPFD5420_CS_PIN, 1);
  gpioSetDir(SPFD5420_CD_PORT, SPFD5420_CD_PIN, 1);
  gpioSetDir(SPFD5420_WR_PORT, SPFD5420_WR_PIN, 1);
  gpioSetDir(SPFD5420_RD_PORT, SPFD5420_RD_PIN, 1);
  
  // Set data port pins to output
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN1, 1);
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN2, 1);
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN3, 1);
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN4, 1);
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN5, 1);
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN6, 1);
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN7, 1);
  gpioSetDir(SPFD5420_DATA_PORT, SPFD5420_DATA_PIN8, 1);
    
  // Set backlight
  gpioSetDir(SPFD5420_BL_PORT, SPFD5420_BL_PIN, 1);      // set to output
  gpioSetValue(SPFD5420_BL_PORT, SPFD5420_BL_PIN, 0);    // turn on

  // Initialize the display
  spfd5420InitDisplay();

  // Fill black
  lcdFillRGB(BLACK);
}

/*************************************************/
void lcdTest(void)
{
  uint32_t i,j;
  spfd5420Home();
  
  for(i=0;i<240;i++)
  {
    for(j=0;j<400;j++)
    {
      if(i>209)spfd5420WriteData(WHITE);
      else if(i>179)spfd5420WriteData(BLUE);
      else if(i>149)spfd5420WriteData(GREEN);
      else if(i>119)spfd5420WriteData(CYAN);
      else if(i>89)spfd5420WriteData(RED);
      else if(i>59)spfd5420WriteData(MAGENTA);
      else if(i>29)spfd5420WriteData(YELLOW);
      else spfd5420WriteData(BLACK);
    }
  }
}

/*************************************************/
void lcdFillRGB(uint16_t data)
{
  unsigned int i;
  spfd5420Home();
  
  uint32_t pixels = 240*400;
  for ( i=0; i < pixels; i++ )
  {
    spfd5420WriteData(data);
  } 
}

/*************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  spfd5420WriteCmd(0x0200);     // GRAM Address Set (Horizontal Address) (R20h)
  spfd5420WriteData(x-1);
  spfd5420WriteCmd(0x0201);     // GRAM Address Set (Vertical Address) (R21h)
  spfd5420WriteData(y-1);
  spfd5420WriteCmd(0x0202);     // Write Data to GRAM (R22h)
  spfd5420WriteData(color);
}
