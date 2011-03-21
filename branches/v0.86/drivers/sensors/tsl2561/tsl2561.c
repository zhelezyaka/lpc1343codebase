/**************************************************************************/
/*! 
    @file     tsl2561.c
    @author   K. Townsend (microBuilder.eu)
	
    @brief    Drivers for the TAOS TSL2561 I2C digital luminosity sensor

    @section DESCRIPTION

    The TSL2561 is a 16-bit digital luminosity sensor the approximates
    the human eye's response to light.  It contains one broadband
    photodiode that measures visible plus infrared light (channel 0)
    and one infrared photodiode (channel 1).

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
#include "tsl2561.h"
#include "core/systick/systick.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;

uint32_t i;

static bool _tsl2561Initialised = false;

/**************************************************************************/
/*! 
    @brief  Sends a single command byte over I2C
*/
/**************************************************************************/
tsl2561Error_e tsl2561WriteCmd (uint8_t cmd)
{
  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = TSL2561_ADDRESS;       // I2C device address
  I2CMasterBuffer[1] = cmd;                   // Command register
  i2cEngine();
  return TSL2561_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Writes an 8 bit values over I2C
*/
/**************************************************************************/
tsl2561Error_e tsl2561Write8 (uint8_t reg, uint32_t value)
{
  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = TSL2561_ADDRESS;           // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  I2CMasterBuffer[2] = (value & 0xFF);            // Value to write
  i2cEngine();
  return TSL2561_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
tsl2561Error_e tsl2561Read16(uint8_t reg, uint16_t *value)
{
  // Clear write buffers
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 2;
  I2CMasterBuffer[0] = TSL2561_ADDRESS;             // I2C device address
  I2CMasterBuffer[1] = reg;                       // Command register
  // Append address w/read bit
  I2CMasterBuffer[2] = TSL2561_ADDRESS | TSL2561_READBIT;  
  i2cEngine();

  // Shift values to create properly formed integer (low byte first)
  *value = (I2CSlaveBuffer[0] | (I2CSlaveBuffer[1] << 8));

  return TSL2561_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
tsl2561Error_e tsl2561Init(void)
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return TSL2561_ERROR_I2CINIT;    /* Fatal error */
  }
  
  _tsl2561Initialised = true;

  // Note: by default, the device is in power down mode on bootup

  return TSL2561_ERROR_OK;
}

/**************************************************************************/
/*! 
    @brief  Reads the luminosity on both channels from the TSL2561
*/
/**************************************************************************/
tsl2561Error_e tsl2561GetLuminosity (uint16_t *broadband, uint16_t *ir)
{
  if (!_tsl2561Initialised) tsl2561Init();

  tsl2561Error_e error = TSL2561_ERROR_OK;

  // Enable the device by setting the control bit to 0x03
  error = tsl2561Write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON);
  if (error) return error;  

  // Wait 400ms for ADC to complete
  systickDelay(400);

  // Reads two byte value from channel 0 (visible + infrared)
  error = tsl2561Read16(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN0_LOW, broadband);
  if (error) return error;

  // Reads two byte value from channel 1 (infrared)
  error = tsl2561Read16(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN1_LOW, ir);
  if (error) return error;

  // Turn the device off to save power
  error = tsl2561Write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWEROFF);
  if (error) return error;  

  return error;
}

/**************************************************************************/
/*! 
    @brief  Calculates LUX from the supplied ch0 and ch1 readings
*/
/**************************************************************************/
uint32_t tsl2561CalculateLux(uint16_t ch0, uint16_t ch1)
{
  unsigned long chScale;
  unsigned long channel1;
  unsigned long channel0;

  // Assumes no scaling (integration time = 402ms)
  chScale = (1 << TSL2561_LUX_CHSCALE);

  // Scale for gain (assumes gain = 1x)
  chScale = chScale << 4;

  // scale the channel values
  channel0 = (ch0 * chScale) >> TSL2561_LUX_CHSCALE;
  channel1 = (ch1 * chScale) >> TSL2561_LUX_CHSCALE;

  // find the ratio of the channel values (Channel1/Channel0)
  unsigned long ratio1 = 0;
  if (channel0 != 0) ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE+1)) / channel0;

  // round the ratio value
  unsigned long ratio = (ratio1 + 1) >> 1;

  unsigned int b, m;
  if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1T))
    {b=TSL2561_LUX_B1T; m=TSL2561_LUX_M1T;}
  else if (ratio <= TSL2561_LUX_K2T)
    {b=TSL2561_LUX_B2T; m=TSL2561_LUX_M2T;}
  else if (ratio <= TSL2561_LUX_K3T)
    {b=TSL2561_LUX_B3T; m=TSL2561_LUX_M3T;}
  else if (ratio <= TSL2561_LUX_K4T)
    {b=TSL2561_LUX_B4T; m=TSL2561_LUX_M4T;}
  else if (ratio <= TSL2561_LUX_K5T)
    {b=TSL2561_LUX_B5T; m=TSL2561_LUX_M5T;}
  else if (ratio <= TSL2561_LUX_K6T)
    {b=TSL2561_LUX_B6T; m=TSL2561_LUX_M6T;}
  else if (ratio <= TSL2561_LUX_K7T)
    {b=TSL2561_LUX_B7T; m=TSL2561_LUX_M7T;}
  else if (ratio > TSL2561_LUX_K8T)
    {b=TSL2561_LUX_B8T; m=TSL2561_LUX_M8T;}

  unsigned long temp;
  temp = ((channel0 * b) - (channel1 * m));

  // do not allow negative lux value
  if (temp < 0) temp = 0;

  // round lsb (2^(LUX_SCALE-1))
  temp += (1 << (TSL2561_LUX_LUXSCALE-1));

  // strip off fractional portion
  uint32_t lux = temp >> TSL2561_LUX_LUXSCALE;

  // Signal I2C had no errors
  return lux;
}
