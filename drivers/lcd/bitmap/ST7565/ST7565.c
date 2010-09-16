/**************************************************************************/
/*! 
    @file     ST7565.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section DESCRIPTION

    Driver for 128x64 pixel display based on the ST7565 LCD controller.

    This driver is based on the ST7565 Library from Limor Fried
    (Adafruit Industries) at: http://github.com/adafruit/ST7565-LCD/
 
    
    
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
#include "ST7565.h"

#include "core/gpio/gpio.h"
#include "core/systick/systick.h"

void sendByte(uint8_t byte);

#define CMD(c)        do { gpioSetValue( ST7565_A0_PORT, ST7565_A0_PIN, 0 ); sendByte( c ); } while (0);
#define DATA(d)       do { gpioSetValue( ST7565_A0_PORT, ST7565_A0_PIN, 1 ); sendByte( d ); } while (0);
#define DELAY(mS)     do { systickDelay( mS / CFG_SYSTICK_DELAY_IN_MS ); } while(0);

uint8_t buffer[128*64/8];

/**************************************************************************/
/* Private Methods                                                        */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Renders the buffer contents

    @param[in]  buffer
                Pointer to the buffer containing the raw pixel data
*/
/**************************************************************************/
void writeBuffer(uint8_t *buffer) 
{
  uint8_t c, p;
  int pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

  for(p = 0; p < 8; p++) 
  {
    CMD(ST7565_CMD_SET_PAGE | pagemap[p]);
    CMD(ST7565_CMD_SET_COLUMN_LOWER | (0x0 & 0xf));
    CMD(ST7565_CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
    CMD(ST7565_CMD_RMW);
    DATA(0xff);
    
    for(c = 0; c < 128; c++) 
    {
      DATA(buffer[(128*p)+c]);
    }
  }
}

/**************************************************************************/
/*! 
    @brief Simulates an SPI write using GPIO

    @param[in]  byte
                The byte to send
*/
/**************************************************************************/
void sendByte(uint8_t byte)
{
  int8_t i;

  // Note: This code can be optimised to avoid the branches by setting
  // GPIO registers directly, but we'll leave it as is for the moment
  // for simplicity sake

  // Write from MSB to LSB
  for (i=7; i>=0; i--) 
  {
    // Set clock pin low
    gpioSetValue(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 0);
    // Set data pin high or low depending on the value of the current bit
    gpioSetValue(ST7565_SDAT_PORT, ST7565_SDAT_PIN, byte & (1 << i) ? 1 : 0);
    // Set clock pin high
    gpioSetValue(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 1);
  }
}

/**************************************************************************/
/* Public Methods                                                         */
/**************************************************************************/

/**************************************************************************/
/*! 
    @brief Initialises the ST7565 LCD display
*/
/**************************************************************************/
void st7565Init(void)
{
  // Note: This can be optimised to set all pins to output and high
  // in two commands by manipulating the registers directly (assuming
  // that the pins are located in the same GPIO bank).  The code is left
  // as is for clarity sake in case the pins are not all located in the
  // same bank.

  // Set clock pin to output and high
  gpioSetDir(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 1);
  gpioSetValue(ST7565_SCLK_PORT, ST7565_SCLK_PIN, 1);

  // Set data pin to output and high
  gpioSetDir(ST7565_SDAT_PORT, ST7565_SDAT_PIN, 1);
  gpioSetValue(ST7565_SDAT_PORT, ST7565_SDAT_PIN, 1);

  // Configure backlight pin to output and set high (off)
  gpioSetDir(ST7565_BL_PORT, ST7565_BL_PIN, 1);
  gpioSetValue(ST7565_BL_PORT, ST7565_BL_PIN, 1);

  // Configure A0 pin to output and set high
  gpioSetDir(ST7565_A0_PORT, ST7565_A0_PIN, 1);
  gpioSetValue(ST7565_A0_PORT, ST7565_A0_PIN, 1);

  // Configure Reset pin and set high
  gpioSetDir(ST7565_RST_PORT, ST7565_RST_PIN, 1);
  gpioSetValue(ST7565_RST_PORT, ST7565_RST_PIN, 1);

  // Configure select pin and set high
  gpioSetDir(ST7565_CS_PORT, ST7565_CS_PIN, 1);
  gpioSetValue(ST7565_CS_PORT, ST7565_CS_PIN, 1);

  // Reset
  gpioSetValue(ST7565_CS_PORT, ST7565_CS_PIN, 0);     // Set CS low
  gpioSetValue(ST7565_RST_PORT, ST7565_RST_PIN, 0);   // Set reset low
  DELAY(500 / CFG_SYSTICK_DELAY_IN_MS);               // Wait 500mS
  gpioSetValue(ST7565_RST_PORT, ST7565_RST_PIN, 1);   // Set reset high

  // Configure Display
  CMD(ST7565_CMD_SET_BIAS_7);                         // LCD Bias Select
  CMD(ST7565_CMD_SET_ADC_NORMAL);                     // ADC Select
  CMD(ST7565_CMD_SET_COM_NORMAL);                     // SHL Select
  CMD(ST7565_CMD_SET_DISP_START_LINE);                // Initial Display Line
  CMD(ST7565_CMD_SET_POWER_CONTROL | 0x04);           // Turn on voltage converter (VC=1, VR=0, VF=0)
  DELAY(50 / CFG_SYSTICK_DELAY_IN_MS);                // Wait 50mS
  CMD(ST7565_CMD_SET_POWER_CONTROL | 0x06);           // Turn on voltage regulator (VC=1, VR=1, VF=0)
  DELAY(50 / CFG_SYSTICK_DELAY_IN_MS);                // Wait 50mS
  CMD(ST7565_CMD_SET_POWER_CONTROL | 0x07);           // Turn on voltage follower
  DELAY(10 / CFG_SYSTICK_DELAY_IN_MS);                // Wait 10mS
  CMD(ST7565_CMD_SET_RESISTOR_RATIO | 0x6);           // Set LCD operating voltage

  // Turn display on
  CMD(ST7565_CMD_DISPLAY_ON);
  CMD(ST7565_CMD_SET_ALLPTS_NORMAL);
  st7565SetBrightness(0x18);
}

/**************************************************************************/
/*! 
    @brief Enables the backlight
*/
/**************************************************************************/
void st7565BLEnable(void)
{
  gpioSetValue( ST7565_BL_PORT, ST7565_BL_PIN, 0 );
}

/**************************************************************************/
/*! 
    @brief Disables the backlight
*/
/**************************************************************************/
void st7565BLDisable(void)
{
  gpioSetValue( ST7565_BL_PORT, ST7565_BL_PIN, 1 );
}

/**************************************************************************/
/*! 
    @brief Sets the display brightness
*/
/**************************************************************************/
void st7565SetBrightness(uint8_t val)
{
  CMD(ST7565_CMD_SET_VOLUME_FIRST);
  CMD(ST7565_CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

/**************************************************************************/
/*! 
    @brief Clears the screen
*/
/**************************************************************************/
void st7565ClearScreen(void) 
{
  uint8_t p, c;
  
  for(p = 0; p < 8; p++) 
  {
    CMD(ST7565_CMD_SET_PAGE | p);
    for(c = 0; c < 129; c++) 
    {
      CMD(ST7565_CMD_SET_COLUMN_LOWER | (c & 0xf));
      CMD(ST7565_CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      DATA(0xC);
    }     
  }
}

/**************************************************************************/
/*! 
    @brief Renders the contents of the pixel buffer on the LCD
*/
/**************************************************************************/
void st7565Refresh(void)
{
  writeBuffer(buffer);
}

/**************************************************************************/
/*! 
    @brief Draws a single pixel in image buffer

    @param[in]  x
                The x position (0..127)
    @param[in]  y
                The y position (0..63)
*/
/**************************************************************************/
void st7565DrawPixel(uint8_t x, uint8_t y) 
{
  if ((x >= 128) || (y >= 64))
    return;

  // x is which column
  buffer[x+ (y/8)*128] |= (1 << (7-(y%8)));
}

/**************************************************************************/
/*! 
    @brief Clears a single pixel in image buffer

    @param[in]  x
                The x position (0..127)
    @param[in]  y
                The y position (0..63)
*/
/**************************************************************************/
void st7565ClearPixel(uint8_t x, uint8_t y)
{
  if ((x >= 128) || (y >= 64))
    return;

  // x is which column
  buffer[x+ (y/8)*128] &= ~(1 << (7-(y%8)));
}
