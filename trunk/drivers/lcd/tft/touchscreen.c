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

static bool _tsInitialised = FALSE;

/**************************************************************************/
/*!
    @brief  Reads the current Z/pressure level using the ADC
*/
/**************************************************************************/
static void tsReadZ(uint32_t* z1, uint32_t* z2)
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
    @brief  Causes a blocking delay until a valid touch event occurs

    @note   Thanks to 'rossum' and limor for this nifty little tidbit on
            debouncing the signals via pressure sensitivity (using Z)
*/
/**************************************************************************/
void tsWaitForEvent(tsTouchData_t* data)
{
  if (!_tsInitialised) tsInit();

  // Read Z for pressure
  uint32_t z1, z2;
  z1 = z2 = 0;

  // Wait for touch
  while (z2 < 8)
  {
    tsReadZ(&z1, &z2);
  }

  // Set results
  data->x = tsReadX();
  data->y = tsReadY();

  // Calculate pressure level
  int32_t t = z2 * data->x / z1;
  t-=64;
  if (t < 0)
  {
    data->pressure = 0;
  }
  else if (t > 255)
  {
    data->pressure = 255;
  }
  else
  {
    data->pressure = t;
  }
  
  // Display results
  printf("Touch Event: X = %d, Y = %d %s", (int)data->x, (int)data->y, CFG_PRINTF_NEWLINE);
}

