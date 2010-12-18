/**************************************************************************/
/*! 
    @file     touchscreen.c
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
#include "touchscreen.h"

#include "core/adc/adc.h"
#include "core/gpio/gpio.h"
#include "core/systick/systick.h"
#include "drivers/eeprom/eeprom.h"
#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/fonts/inconsolata11.h"

static bool _tsInitialised = FALSE;
static tsCalibrationData_t _calibration;

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
void tsCalibCenterText(char* text, uint16_t y, uint16_t color)
{
  drawString((CFG_TFTLCD_WIDTH - drawGetStringWidth(&inconsolata11ptFontInfo, text)) / 2, y, color, &inconsolata11ptFontInfo, text);
}

/**************************************************************************/
/*!
    @brief  Draws a circular test point
*/
/**************************************************************************/
void tsCalibDrawTestPoint(uint16_t x, uint16_t y, uint16_t radius)
{
  drawCircle(x, y, radius, COLOR_RED);
  drawCircle(x, y, radius + 2, COLOR_MEDIUMGRAY);
}

/**************************************************************************/
/*!
    @brief  Reads the current Z/pressure level using the ADC
*/
/**************************************************************************/
void tsReadZ(uint32_t* z1, uint32_t* z2)
{
  if (!_tsInitialised) tsInit();

  // Make sure that X+/Y- are set to GPIO
  TS_XP_FUNC_GPIO;
  TS_YM_FUNC_GPIO;

  // Set X- and Y+ to inputs (necessary?)
  gpioSetDir (TS_XM_PORT, TS_XM_PIN, 0);
  gpioSetDir (TS_YP_PORT, TS_YP_PIN, 0);

  // Set X+ and Y- to output
  gpioSetDir (TS_XP_PORT, TS_XP_PIN, 1);
  gpioSetDir (TS_YM_PORT, TS_YM_PIN, 1);

  // X+ goes low, Y- goes high
  gpioSetValue(TS_XP_PORT, TS_XP_PIN, 0);   // GND
  gpioSetValue(TS_YM_PORT, TS_YM_PIN, 1);   // 3.3V

  // Set X- and Y+ to ADC
  TS_XM_FUNC_ADC;  
  TS_YP_FUNC_ADC;  

  // Get ADC results
  *z1 = adcRead(TS_YP_ADC_CHANNEL);     // Z1 (Read Y+)
  *z2 = adcRead(TS_XM_ADC_CHANNEL);     // Z2 (Read X-)
}

/**************************************************************************/
/*!
    @brief  Reads the current X position using the ADC
*/
/**************************************************************************/
uint32_t tsReadX(void)
{
  if (!_tsInitialised) tsInit();

  // Make sure Y+/Y- are set to GPIO
  TS_YP_FUNC_GPIO;
  TS_YM_FUNC_GPIO;

  // Set X- and X+ to inputs
  gpioSetDir (TS_XM_PORT, TS_XM_PIN, 0);
  gpioSetDir (TS_XP_PORT, TS_XP_PIN, 0);
  
  // Set Y- and Y+ to output
  gpioSetDir (TS_YM_PORT, TS_YM_PIN, 1);
  gpioSetDir (TS_YP_PORT, TS_YP_PIN, 1);

  // Y+ goes high, Y- goes low
  gpioSetValue(TS_YP_PORT, TS_YP_PIN, 1);   // 3.3V
  gpioSetValue(TS_YM_PORT, TS_YM_PIN, 0);   // GND

  // Set pin 1.0 (X+) to ADC1
  TS_XP_FUNC_ADC;  

  // Return the ADC results
  return adcRead(TS_XP_ADC_CHANNEL);
}

/**************************************************************************/
/*!
    @brief  Reads the current Y position using the ADC
*/
/**************************************************************************/
uint32_t tsReadY(void)
{
  if (!_tsInitialised) tsInit();

  // Make sure X+/X- are set to GPIO
  TS_XP_FUNC_GPIO;
  TS_XM_FUNC_GPIO;

  // Set Y- and Y+ to inputs
  gpioSetDir (TS_YM_PORT, TS_YM_PIN, 0);
  gpioSetDir (TS_YP_PORT, TS_YP_PIN, 0);
  
  // Set X- and X+ to output
  gpioSetDir (TS_XM_PORT, TS_XM_PIN, 1);
  gpioSetDir (TS_XP_PORT, TS_XP_PIN, 1);

  // X+ goes high, X- goes low
  gpioSetValue(TS_XP_PORT, TS_XP_PIN, 1);   // 3.3V
  gpioSetValue(TS_XM_PORT, TS_XM_PIN, 0);   // GND

  // Set pin 0.11 (Y+) to ADC0
  TS_YP_FUNC_ADC;

  // Return the ADC results
  return adcRead(TS_YP_ADC_CHANNEL);
}

/**************************************************************************/
/*!
    @brief  Converts Z variables into 'pressure' value (0..255)
*/
/**************************************************************************/
uint8_t tsCalculatePressure(uint32_t x, uint32_t z1, uint32_t z2)
{
  // Calculate pressure level
  int32_t t = z2 * x / z1;
  t-=64;
  if (t < 0)
  {
    return 0;
  }
  else if (t > 255)
  {
    return 255;
  }
  else
  {
    return (uint8_t)t;
  }
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Initialises the appropriate GPIO pins and ADC for the
            touchscreen
*/
/**************************************************************************/
void tsInit(void)
{
  // Make sure that ADC is initialised
  adcInit();

  // Set initialisation flag
  _tsInitialised = TRUE;

  // Check if the touch-screen has been calibrated
  if (eepromReadU8(CFG_EEPROM_TOUCHSCREEN_CALIBRATED) == 1)
  {
    // Load calibration data
    _calibration.offsetLeft   = eepromReadU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_LEFT);
    _calibration.offsetRight  = eepromReadU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_RIGHT);
    _calibration.offsetTop    = eepromReadU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_TOP);
    _calibration.offsetBottom = eepromReadU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_BOT);
    _calibration.divisorX     = eepromReadU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_DIVX);
    _calibration.divisorY     = eepromReadU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_DIVY);
  }
  else
  {
    // Start touch-screen calibration
    tsCalibrate();
  }
}

/**************************************************************************/
/*!
    @brief  Starts the screen calibration process
*/
/**************************************************************************/
void tsCalibrate(void)
{
  uint32_t z1, z2;
  z1 = z2 = 0;

  /* -------------- Welcome Screen --------------- */
  drawFill(COLOR_WHITE);
  tsCalibCenterText("To begin calibrating the", 50, COLOR_DARKGRAY);
  tsCalibCenterText("touch screen please click", 65, COLOR_DARKGRAY);
  tsCalibCenterText("on the center dot", 80, COLOR_DARKGRAY);
  tsCalibDrawTestPoint(CFG_TFTLCD_WIDTH / 2, CFG_TFTLCD_HEIGHT / 2, 5);

  // Wait for touch
  while (z2 < CFG_TFTLCD_TS_THRESHOLD) 
  { 
    tsReadZ(&z1, &z2); 
  }
  systickDelay(250);

  /* -------------- CALIBRATE X --------------- */
  drawFill(COLOR_WHITE);
  tsCalibCenterText("Touch the center of the", 50, COLOR_DARKGRAY);
  tsCalibCenterText("upper left circle using", 65, COLOR_DARKGRAY);
  tsCalibCenterText("a pen or stylus", 80, COLOR_DARKGRAY);
  tsCalibDrawTestPoint(5, 5, 5);

  // Wait for touch
  z1 = z2 = 0;
  while (z2 < CFG_TFTLCD_TS_THRESHOLD) 
  {
    tsReadZ(&z1, &z2); 
  }

  _calibration.offsetLeft = tsReadY();
  _calibration.offsetTop = tsReadX();

  // Delay to avoid multiple touch events
  systickDelay(250);

  /* -------------- CALIBRATE Y --------------- */
  drawFill(COLOR_WHITE);
  tsCalibCenterText("Touch the center of the", 50, COLOR_DARKGRAY);
  tsCalibCenterText("bottom right circle using", 65, COLOR_DARKGRAY);
  tsCalibCenterText("a pen or stylus", 80, COLOR_DARKGRAY);
  tsCalibDrawTestPoint(CFG_TFTLCD_WIDTH - 5, CFG_TFTLCD_HEIGHT - 5, 5);

  // Wait for touch
  z1 = z2 = 0;
  while (z2 < CFG_TFTLCD_TS_THRESHOLD)
  {
    tsReadZ(&z1, &z2);
  }
  _calibration.offsetRight = (1024 - tsReadY());
  _calibration.offsetBottom = (1024 - tsReadX());

  _calibration.divisorX = ((1024 - (_calibration.offsetLeft + _calibration.offsetRight)) * 100) / CFG_TFTLCD_WIDTH;
  _calibration.divisorY = ((1024 - (_calibration.offsetTop + _calibration.offsetBottom)) * 100) / CFG_TFTLCD_HEIGHT;

  /* -------------- Test Results --------------- */
  // ToDo: Confirm values and redo calibration if necessary

  /* -------------- Persist Data --------------- */
  // Persist data to EEPROM
  eepromWriteU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_LEFT, _calibration.offsetLeft);
  eepromWriteU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_RIGHT, _calibration.offsetRight);
  eepromWriteU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_TOP, _calibration.offsetTop);
  eepromWriteU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_BOT, _calibration.offsetBottom);
  eepromWriteU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_DIVX, _calibration.divisorX);
  eepromWriteU16(CFG_EEPROM_TOUCHSCREEN_OFFSET_DIVY, _calibration.divisorY);
  eepromWriteU8(CFG_EEPROM_TOUCHSCREEN_CALIBRATED, 1);

  // Clear the screen
  drawFill(COLOR_BLACK);
}

/**************************************************************************/
/*!
    @brief  Causes a blocking delay until a valid touch event occurs

    @note   Thanks to 'rossum' and limor for this nifty little tidbit on
            debouncing the signals via pressure sensitivity (using Z)

    @section Example

    @code 
    #include "drivers/lcd/tft/touchscreen.h"

    // Create an object to hold the eventual event data
    tsTouchData_t data;

    // Cause a blocking delay until a valid touch event occurs
    tsWaitForEvent(&data);

    // A valid touch event occurred ... parse data
    if (data.x > 100 && data.x < 200)
    {
      // Do something
      printf("Touch Event: X = %d, Y = %d %s", 
          (int)data.x, 
          (int)data.y, 
          CFG_PRINTF_NEWLINE);
    }

    @endcode
*/
/**************************************************************************/
void tsWaitForEvent(tsTouchData_t* data)
{
  if (!_tsInitialised) tsInit();

  // Read Z for pressure
  uint32_t z1, z2, xRaw, yRaw;
  z1 = z2 = 0;

  // Wait for touch
  while (z2 < CFG_TFTLCD_TS_THRESHOLD)
  {
    tsReadZ(&z1, &z2);
  }

  // Get raw conversion results
  xRaw = tsReadY();    // X and Y are reversed
  yRaw = tsReadX();    // X and Y are reverse

  // Normalise X
  data->x = ((xRaw - _calibration.offsetLeft > 1024 ? 0 : xRaw - _calibration.offsetLeft) * 100) / _calibration.divisorX;
  if (data->x > CFG_TFTLCD_WIDTH - 1) data->x = CFG_TFTLCD_WIDTH - 1;

  // Normalise Y
  data->y = ((yRaw - _calibration.offsetTop > 1024 ? 0 : yRaw - _calibration.offsetTop) * 100) / _calibration.divisorY;
  if (data->y > CFG_TFTLCD_HEIGHT - 1) data->y = CFG_TFTLCD_HEIGHT - 1;
}
