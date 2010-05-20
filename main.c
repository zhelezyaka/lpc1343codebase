/**************************************************************************/
/*! 
    @file     main.c
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

#include <stdio.h>
#include <stdlib.h>

#include "projectconfig.h"

#include "core/cpu/cpu.h"
#include "core/gpio/gpio.h"
#include "core/uart/uart.h"
#include "core/systick/systick.h"

#ifdef CFG_USBHID
#include "core/usbhid-rom/usbhid.h"
#endif

#ifdef CFG_I2CEEPROM
#include "drivers/eeprom/mcp24aa/mcp24aa.h"
#endif

int main (void)
{
  // Setup the cpu and core clock
  cpuInit();

  // Initialise the systick timer (delay set in projectconfig.h))
  systickInit(CFG_SYSTICK_DELAY_IN_MS);

  // Initialise UART with the default baud rate (set in projectconfig.h)
  // (Required since printf is redirected to UART)
  uartInit(CFG_UART_BAUDRATE);

  printf("CPU Initialised (72MHz).......\r\n");

  // Set LED pin as output and turn off
  gpioSetDir(CFG_LED_PORT, CFG_LED_PIN, 1);
  gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, 1);

  // Initialise USB HID
  #ifdef CFG_USBHID
  printf("Initialising USB (HID)........\r\n");
  usbHIDInit();
  #endif

  // Initialise EEPROM
  #ifdef CFG_I2CEEPROM
  printf("Initialising I2C EEPROM.......\r\n");
  mcp24aaInit();
  // uint8_t buffer[1] = { 0x00 };
  // mcp24aaWriteByte(0x0000, 0xEE);
  // mcp24aaReadByte(0x0000, buffer);
  #endif

  while (1)
  {
    // Blink LED every second
    systickDelay(1000 / CFG_SYSTICK_DELAY_IN_MS);
    if (gpioGetValue(CFG_LED_PORT, CFG_LED_PIN))
    {
      // Enable LED (set low)
      gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, 0);
    }
    else
    {
      // Disable LED (set high)
      gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, 1);
    }
  }
}

/**************************************************************************/
/*! 
    Redirect printf output to UART0
*/
/**************************************************************************/
void __putchar(char c) 
{
  uartSendByte(c);
}

int puts ( const char * str )
{
  while(*str++) __putchar(*str);
  return 0;
}
