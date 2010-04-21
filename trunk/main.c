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
#include "core/timer32/timer32.h"
#include "core/pmu/pmu.h"

#ifdef CFG_USBHID
#include "core/usbhid-rom/usbhid.h"
#endif

#ifdef CFG_I2CEEPROM
#include "drivers/eeprom/mcp24aa/mcp24aa.h"
#endif

#ifdef CFG_LM75B
#include "drivers/sensors/lm75b/lm75b.h"
#endif

#ifdef CFG_CHIBI
#include "drivers/chibi/chb.h"
#include "drivers/chibi/chb_drvr.h"
static chb_rx_data_t rx_data;
#endif


int main (void)
{
  cpuInit();
  systickInit(CFG_SYSTICK_DELAY_MS);
  uartInit(CFG_UART_BAUDRATE);

  printf("CPU Initialised (72MHz).......\r\n");

  // Set LED as output and turn off
  gpioSetDir(CFG_LED_PORT, CFG_LED_PIN, 1);
  gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, 1);

  // Initialise 32-bit timer 0 with default delay 
  printf("Initialising 32-bit Timer 0 with 100 uS delay...\r\n");
  timer32Init(0, TIMER32_DEFAULTINTERVAL);
  timer32Enable(0);

  // Initialise USB HID
  #ifdef CFG_USBHID
  printf("Initialising USB (HID)........\r\n");
  usbHIDInit();
  #endif

  #ifdef CFG_I2CEEPROM
  printf("Initialising I2C EEPROM.......\r\n");
  mcp24aaInit();
  uint8_t buffer[1] = { 0x00 };
  mcp24aaWriteByte(0x0000, 0xEE);
  mcp24aaReadByte(0x0000, buffer);
  #endif

  #ifdef CFG_LM75B
  int32_t temperature = 0;
  lm75bInit();
  #endif

  #ifdef CFG_CHIBI
  printf("Initialising Chibi (868MHz)...\r\n");
  pcb_t *pcb = chb_get_pcb();
  chb_init();
  uint32_t i = 0;
  char buf[11];
  //  // Set address if necessary
  //  uint8_t addr_ieee[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
  //  chb_set_ieee_addr(addr_ieee);
  //  uint16_t addr_short = 0x1234;
  //  chb_set_short_addr(addr_short);
  #endif

//  // Wait 10 seconds before entering deep sleep
//  timer32Delay(0, TIMER32_DELAY_1S * 10);
//  pmuInit();
//  // Enter deep sleep and wakeup after 5 seconds
//  uint32_t pmuRegVal = SCB_PDSLEEPCFG_IRCOUT_PD |
//                SCB_PDSLEEPCFG_IRC_PD |
//                SCB_PDSLEEPCFG_FLASH_PD |
//                SCB_PDSLEEPCFG_USBPLL_PD |
//                SCB_PDSLEEPCFG_SYSPLL_PD |
//                SCB_PDSLEEPCFG_SYSOSC_PD |
//                SCB_PDSLEEPCFG_ADC_PD |
//                SCB_PDSLEEPCFG_BOD_PD;
//  pmuDeepSleep(pmuRegVal, 5);

  while (1)
  {
    #ifdef CFG_CHIBI
      #ifdef CFG_CHIBI_TRANSMITTER
        i++;
        sprintf(buf,"%ld",i);
        gpioSetValue (2, 10, 0);
        chb_write(0xFFFF, (uint8_t *)buf, 11);
        gpioSetValue (2, 10, 1);
        timer32Delay(0, TIMER32_DELAY_1S);
      #endif
      #ifdef CFG_CHIBI_RECEIVER
        if (pcb->data_rcv)
        {
          // Enable LED (set low)
          gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, 0);
          // get the length of the data
          rx_data.len = chb_read(&rx_data);
          // Output message to UART
          printf("Message received from node %02X: %s, len=%d, rssi=%02X.\n", rx_data.src_addr, rx_data.data, rx_data.len, pcb->ed);
          // Disable LED (set high)
          gpioSetValue (CFG_LED_PORT, CFG_LED_PIN, 1);
        }
      #endif
    #else
      // Blink LED every second
      timer32Delay(0, TIMER32_DELAY_1S * 1);
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
      #ifdef CFG_LM75B
        lm75bGetTemperature(&temperature);
        // Multiply value by 125 for fixed-point math (0.125°C per unit)
        temperature *= 125;
        // Use modulus operator to display decimal value
        printf("Current Temperature: %ld.%ld C\r\n", temperature / 1000, temperature % 1000);
      #endif
    #endif
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
