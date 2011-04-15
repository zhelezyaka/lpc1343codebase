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
#include "drivers/lcd/tft/lcd.h"
#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/fonts/dejavusans9.h"

#define TS_CALIBRATION_OUTOFRANGE (300)     // Maximum value for calibration
#define TS_ADC_LIMIT              (0x03FF)  // 10-bit ADC = 0..1023

#define TS_LINE1                  "Touch the center of"
#define TS_LINE2                  "the red circle using"
#define TS_LINE3                  "a pen or stylus"

static bool _tsInitialised = FALSE;
static tsCalibrationData_t _calibration;
static uint8_t _tsThreshhold = CFG_TFTLCD_TS_DEFAULTTHRESHOLD;

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/

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

  lcdOrientation_t orientation;
  orientation = lcdGetOrientation();

  if (orientation == LCD_ORIENTATION_LANDSCAPE)
  {
    // Make sure X+/X- are set to GPIO
    TS_XP_FUNC_GPIO;
    TS_XM_FUNC_GPIO;
  
    // Set Y- and Y+ to inputs
    gpioSetDir (TS_YM_PORT, TS_YM_PIN, 0);
    gpioSetDir (TS_YP_PORT, TS_YP_PIN, 0);
    
    // Set X- and X+ to output
    gpioSetDir (TS_XM_PORT, TS_XM_PIN, 1);
    gpioSetDir (TS_XP_PORT, TS_XP_PIN, 1);
  
    // X+ goes low, X- goes high
    gpioSetValue(TS_XP_PORT, TS_XP_PIN, 0);   // GND
    gpioSetValue(TS_XM_PORT, TS_XM_PIN, 1);   // 3.3V
  
    // Set pin 0.11 (Y+) to ADC0
    TS_YP_FUNC_ADC;
  
    // Return the ADC results
    return adcRead(TS_YP_ADC_CHANNEL);
  }
  else
  {
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
}

/**************************************************************************/
/*!
    @brief  Reads the current Y position using the ADC
*/
/**************************************************************************/
uint32_t tsReadY(void)
{
  if (!_tsInitialised) tsInit();

  lcdOrientation_t orientation;
  orientation = lcdGetOrientation();

  if (orientation == LCD_ORIENTATION_LANDSCAPE)
  {
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
  else
  {
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
/*!
    @brief  Centers a line of text horizontally
*/
/**************************************************************************/
void tsCalibCenterText(char* text, uint16_t y, uint16_t color)
{
  drawString((lcdGetWidth() - drawGetStringWidth(&dejaVuSans9ptFontInfo, text)) / 2, y, color, &dejaVuSans9ptFontInfo, text);
}

/**************************************************************************/
/*!
    @brief  Renders the calibration screen with an appropriately
            placed test point and waits for a touch event
*/
/**************************************************************************/
void tsRenderCalibrationScreen(uint16_t x, uint16_t y, uint16_t radius)
{

  drawFill(COLOR_WHITE);
  tsCalibCenterText(TS_LINE1, 50, COLOR_DARKGRAY);
  tsCalibCenterText(TS_LINE2, 65, COLOR_DARKGRAY);
  tsCalibCenterText(TS_LINE3, 80, COLOR_DARKGRAY);
  drawCircle(x, y, radius, COLOR_RED);
  drawCircle(x, y, radius + 2, COLOR_MEDIUMGRAY);

  // Wait for touch
  uint32_t z1, z2;
  z1 = z2 = 0;
  while (z2 < _tsThreshhold) 
    tsReadZ(&z1, &z2);
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
  _tsThreshhold = tsGetThreshhold();

  // Load values from EEPROM if touch screen has already been calibrated
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
}

/**************************************************************************/
/*!
    @brief  Starts the screen calibration process.  Each corner will be
            tested, meaning that each boundary (top, left, right and 
            bottom) will be tested twice and the readings averaged.
*/
/**************************************************************************/
void tsCalibrate(void)
{
  lcdOrientation_t orientation;
  uint16_t right2, top2, left2, bottom2;
  bool passed = false;

  // Determine screen orientation before starting calibration
  orientation = lcdGetOrientation();

  /* -------------- Welcome Screen --------------- */
  tsRenderCalibrationScreen(lcdGetWidth() / 2, lcdGetHeight() / 2, 5);

  // Delay to avoid multiple touch events
  systickDelay(250);

  /* -------------- CALIBRATE TOP-LEFT --------------- */
  passed = false;
  while (!passed)
  {
    // Read X/Y depending on screen orientation
    tsRenderCalibrationScreen(3, 3, 5);
    _calibration.offsetLeft = orientation == LCD_ORIENTATION_LANDSCAPE ? tsReadY() : tsReadX();
    _calibration.offsetTop = orientation == LCD_ORIENTATION_LANDSCAPE ? tsReadX() : tsReadY();
  
    // Make sure values are in range
    if (_calibration.offsetLeft < TS_CALIBRATION_OUTOFRANGE && _calibration.offsetTop < TS_CALIBRATION_OUTOFRANGE)
      passed = true;
  }

  // Delay to avoid multiple touch events
  systickDelay(250);

  /* -------------- CALIBRATE BOTTOM-RIGHT  --------------- */
  passed = false;
  while (!passed)
  {
    tsRenderCalibrationScreen(lcdGetWidth() - 4, lcdGetHeight() - 4, 5);
  
    // Read X/Y depending on screen orientation
    _calibration.offsetRight = orientation == LCD_ORIENTATION_LANDSCAPE ? TS_ADC_LIMIT - tsReadY() : TS_ADC_LIMIT - tsReadX();
    _calibration.offsetBottom = orientation == LCD_ORIENTATION_LANDSCAPE ? TS_ADC_LIMIT - tsReadX() : TS_ADC_LIMIT - tsReadY();
  
    // Redo test if value is out of range
    if (_calibration.offsetRight < TS_CALIBRATION_OUTOFRANGE && _calibration.offsetBottom < TS_CALIBRATION_OUTOFRANGE)
      passed = true;
  }

  // Delay to avoid multiple touch events
  systickDelay(250);

  /* -------------- CALIBRATE TOP-RIGHT  --------------- */
  passed = false;
  while (!passed)
  {
    tsRenderCalibrationScreen(lcdGetWidth() - 4, 3, 5);
  
    if (orientation == LCD_ORIENTATION_LANDSCAPE)
    {
      right2 = TS_ADC_LIMIT - tsReadY();
      top2 = tsReadX();
    }
    else
    {
      right2 = tsReadX();
      top2 = TS_ADC_LIMIT - tsReadY();
    }
  
    // Redo test if value is out of range
    if (right2 < TS_CALIBRATION_OUTOFRANGE && top2 < TS_CALIBRATION_OUTOFRANGE)
      passed = true;  
  }

  // Average readings
  _calibration.offsetRight = (_calibration.offsetRight + right2) / 2;
  _calibration.offsetTop = (_calibration.offsetTop + top2) / 2;

  // Delay to avoid multiple touch events
  systickDelay(250);

  /* -------------- CALIBRATE BOTTOM-LEFT --------------- */
  passed = false;
  while (!passed)
  {
    tsRenderCalibrationScreen(3, lcdGetHeight() - 4, 5);
  
    if (orientation == LCD_ORIENTATION_LANDSCAPE)
    {
      left2 = tsReadY();
      bottom2 = TS_ADC_LIMIT - tsReadX();
    }
    else
    {
      left2 = TS_ADC_LIMIT - tsReadX();
      bottom2 = tsReadY();
    }
  
    // Redo test if value is out of range
    if (left2 < TS_CALIBRATION_OUTOFRANGE && bottom2 < TS_CALIBRATION_OUTOFRANGE)
      passed = true;
  }

  // Average readings
  _calibration.offsetLeft = (_calibration.offsetLeft + left2) / 2;
  _calibration.offsetBottom = (_calibration.offsetBottom + bottom2) / 2;

  // Delay to avoid multiple touch events
  systickDelay(250);

  _calibration.divisorX = ((TS_ADC_LIMIT - (_calibration.offsetLeft + _calibration.offsetRight)) * 100) / lcdGetWidth();
  _calibration.divisorY = ((TS_ADC_LIMIT - (_calibration.offsetTop + _calibration.offsetBottom)) * 100) / lcdGetHeight();

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
    tsTouchError_t error;

    // Cause a blocking delay until a touch event occurs or 5s passes
    error = tsWaitForEvent(&data, 5000);

    if (error)
    {
      switch(error)
      {
        case TS_ERROR_TIMEOUT:
          printf("Timeout occurred %s", CFG_PRINTF_NEWLINE);
          break;
        default:
          break;
      }
    }
    else
    {
      // A valid touch event occurred ... parse data
      if (data.x > 100 && data.x < 200)
      {
        // Do something
        printf("Touch Event: X = %d, Y = %d %s", 
            (int)data.x, 
            (int)data.y, 
            CFG_PRINTF_NEWLINE);
      }
    }

    @endcode
*/
/**************************************************************************/
tsTouchError_t tsWaitForEvent(tsTouchData_t* data, uint32_t timeoutMS)
{
  if (!_tsInitialised) tsInit();

  uint32_t z1, z2;
  uint32_t xRaw1, xRaw2, yRaw1, yRaw2;
  z1 = z2 = xRaw1 = xRaw2 = yRaw1 = yRaw2 = 0;

  // Handle timeout if delay > 0 milliseconds
  if (timeoutMS)
  {
    uint32_t startTick = systickGetTicks();
    // Systick rollover may occur while waiting for timeout
    if (startTick > 0xFFFFFFFF - timeoutMS)
    {
      // Wait for timeout or touch event
      while (z2 < _tsThreshhold)
      {
        // Throw alert if timeout delay has been passed
        if ((systickGetTicks() < startTick) && (systickGetTicks() >= (timeoutMS - (0xFFFFFFFF - startTick))))
        {
          return TS_ERROR_TIMEOUT;
        }      
        tsReadZ(&z1, &z2);
      }
    }
    // No systick rollover will occur ... calculate timeout the simple way
    else
    {
      // Wait in infinite loop
      while (z2 < _tsThreshhold)
      {
        // Throw timeout if delay has been passed
        if ((systickGetTicks() - startTick) > timeoutMS)
        {
          return TS_ERROR_TIMEOUT;
        }
        tsReadZ(&z1, &z2);
      }
    }
  }
  // No timeout requested ... wait forever
  else
  {
    while (z2 < _tsThreshhold)
    {
      tsReadZ(&z1, &z2);
    }
  }

  // Keep reading until we get two identical readings
  // Divide values by three to make it a bit less 'fine grain'
  uint8_t attempts = 0;
  while ((xRaw1/3 != xRaw2/3) || (yRaw1/3 != yRaw2/3) || (yRaw1 == 0))
  {
    xRaw1 = tsReadY();    // X and Y are reversed
    xRaw2 = tsReadY();    // X and Y are reversed
    yRaw1 = tsReadX();    // X and Y are reverse
    yRaw2 = tsReadX();    // X and Y are reverse
    // Abort after 100 failed attempts
    if (attempts++ == 100)
    {
      return TS_ERROR_XYMISMATCH;
    }
  }

  // Normalise X
  data->x = ((xRaw1 - _calibration.offsetLeft > TS_ADC_LIMIT ? 0 : xRaw1 - _calibration.offsetLeft) * 100) / _calibration.divisorX;
  if (data->x > lcdGetWidth() - 1) data->x = lcdGetWidth() - 1;

  // Normalise Y
  data->y = ((yRaw1 - _calibration.offsetTop > TS_ADC_LIMIT ? 0 : yRaw1 - _calibration.offsetTop) * 100) / _calibration.divisorY;
  if (data->y > lcdGetHeight() - 1) data->y = lcdGetHeight() - 1;

  // Indicate correct reading
  return TS_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Updates the touch screen threshhold level and saves it
            to EEPROM
*/
/**************************************************************************/
int tsSetThreshhold(uint8_t value)
{
  if ((value < 0) || (value > 254))
  {
    return -1;
  }

  // Update threshhold value
  _tsThreshhold = value;

  // Persist to EEPROM
  eepromWriteU8(CFG_EEPROM_TOUCHSCREEN_THRESHHOLD, value);

  return 0;
}

/**************************************************************************/
/*!
    @brief  Gets the current touch screen threshhold level from EEPROM
            (if present) or returns the default value from projectconfig.h
*/
/**************************************************************************/
uint8_t tsGetThreshhold(void)
{
  // Check if custom threshold has been set in eeprom
  uint8_t thold = eepromReadU8(CFG_EEPROM_TOUCHSCREEN_THRESHHOLD);
  if (thold != 0xFF)
  {
    // Use value from EEPROM
    _tsThreshhold = thold;
  }
  else
  {
    // Use the default value from projectconfig.h
    _tsThreshhold = CFG_TFTLCD_TS_DEFAULTTHRESHOLD;
  }

  return _tsThreshhold;
}
