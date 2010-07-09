/**************************************************************************/
/*! 
    @file     sysinit.c
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

#include "sysinit.h"

#include "core/cpu/cpu.h"
#include "core/pmu/pmu.h"

#ifdef CFG_PRINTF_UART
  #include "core/uart/uart.h"
#endif

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

#ifdef CFG_CHIBI
  #include "drivers/chibi/chb.h"
  static chb_rx_data_t rx_data;
#endif

#ifdef CFG_USBHID
  #include "core/usbhid-rom/usbhid.h"
#endif

#ifdef CFG_USBCDC
  #include "core/usbcdc/usb.h"
  #include "core/usbcdc/usbcore.h"
  #include "core/usbcdc/usbhw.h"
  #include "core/usbcdc/cdcuser.h"
#endif

#ifdef CFG_LCD
  #include "drivers/lcd/lcd.h"
  #include "drivers/lcd/drawing.h"
  #include "drivers/lcd/fonts/consolas9.h"
  #include "drivers/lcd/fonts/consolas11.h"
  #include "drivers/lcd/fonts/consolas16.h"
  #include "drivers/lcd/fonts/smallfonts.h"
#endif

#ifdef CFG_I2CEEPROM
  #include "drivers/eeprom/mcp24aa/mcp24aa.h"
#endif

#ifdef CFG_SDCARD
  #include "drivers/fatfs/diskio.h"
  #include "drivers/fatfs/sdspi.h"
#endif

/**************************************************************************/
/*! 
    Configures the core system clock and sets up any mandatory
    peripherals like the systick timer, UART for printf, etc.

    This function should set the HW to the default state you wish to be
    in coming out of reset/startup, such as disabling or enabling LEDs,
    setting specific pin states, etc.
*/
/**************************************************************************/
void systemInit()
{
  // Setup the cpu and core clock
  cpuInit();

  // Initialise the systick timer (delay set in projectconfig.h)
  systickInit(CFG_SYSTICK_DELAY_IN_MS);

  // Initialise GPIO
  gpioInit();

  #ifdef CFG_PRINTF_UART
    // Initialise UART with the default baud rate (set in projectconfig.h)
    uartInit(CFG_UART_BAUDRATE);
  #endif

  // Initialise power management unit
  pmuInit();

  // Set LED pin as output and turn LED off
  gpioSetDir(CFG_LED_PORT, CFG_LED_PIN, 1);
  gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF);

  // Initialise USB HID
  #ifdef CFG_USBHID
    usbHIDInit();
  #endif

  // Initialise USB CDC
  #ifdef CFG_USBCDC
    CDC_Init();                   // Initialise VCOM
    USB_Init();                   // USB Initialization
    USB_Connect(TRUE);            // USB Connect
    while (!USB_Configuration);   // wait until USB is configured
  #endif

  // Printf can now be used with either UART or USBCDC

  // Initialise EEPROM
  #ifdef CFG_I2CEEPROM
    mcp24aaInit();
  #endif

  // Initialise LCD Display
  #ifdef CFG_LCD
    lcdInit();

    // Get 16-bit equivalent of 24-bit color
    uint16_t gray = drawRGB24toRGB565(0x33, 0x33, 0x33);

    // Fill the screen with a test pattern
    // drawTestPattern();
    drawFill(gray);

    #if defined CFG_LCD_INCLUDESMALLFONTS & CFG_LCD_INCLUDESMALLFONTS == 1
      drawStringSmall(1, 210, WHITE, "5x8 System (Max 40 Characters)", Font_System5x8);
      drawStringSmall(1, 220, WHITE, "7x8 System (Max 30 Characters)", Font_System7x8);
    #endif
 
    drawString(1,   90,   GREEN,    &consolas9ptFontInfo,   "Consolas 9 (38 chars wide)");
    drawString(1,   105,  GREEN,    &consolas9ptFontInfo,   "12345678901234567890123456789012345678");
    drawString(1,   130,  YELLOW,   &consolas11ptFontInfo,  "Consolas 11 (33 chars wide)");
    drawString(1,   145,  YELLOW,   &consolas11ptFontInfo,  "123456789012345678901234567890123");
    drawString(1,   163,  BLACK,    &consolas16ptFontInfo,  "Consolas 16 (22 chars)");
    drawString(1,   183,  BLACK,    &consolas16ptFontInfo,  "1234567890123456789012");
    drawString(1,   10,   BLACK,    &consolas11ptFontInfo,  "Address");
    drawString(210, 10,   BLACK,    &consolas11ptFontInfo,  "Time");
    drawString(1,   22,   WHITE,    &consolas16ptFontInfo,  "0xAA01");
    drawString(190, 22,   WHITE,    &consolas16ptFontInfo,  "04:02");

    // Draw some primitive shapes
    drawCircle(120, 160, 100, WHITE);
    drawLine(10, 10, 70, 300, WHITE);
    drawRectangle (100, 100, 80, 80, WHITE);
    drawRectangleFilled (98, 98, 82, 82, gray);

    // Draw some compound shapes
    drawProgressBar(10, 150, 100, 10, WHITE, BLACK, WHITE, GREEN, 75);
    drawProgressBar(10, 165, 100, 10, WHITE, BLACK, WHITE, YELLOW, 23);
    drawProgressBar(10, 180, 100, 10, WHITE, BLACK, WHITE, RED, 64);
    drawProgressBar(10, 195, 100, 20, WHITE, BLACK, WHITE, BLUE, 90);

    uint16_t getc;
    getc = lcdGetPixel(0, 0);
    // drawFill(getc);
  #endif

  // Initialise Chibi
  #ifdef CFG_CHIBI
    // Write addresses to EEPROM for the first time if necessary
    // uint16_t addr_short = 0x0001;
    // uint64_t addr_ieee =  0x0000000000000001;
    // mcp24aaWriteBuffer(CFG_CHIBI_EEPROM_SHORTADDR, (uint8_t *)&addr_short, 2);
    // mcp24aaWriteBuffer(CFG_CHIBI_EEPROM_IEEEADDR, (uint8_t *)&addr_ieee, 8);
    chb_init();
    chb_pcb_t *pcb = chb_get_pcb();
    printf("%-40s : 0x%04X%s", "Chibi Initialised", pcb->src_addr, CFG_INTERFACE_NEWLINE);
  #endif

  #ifdef CFG_SDCARD
    sdspiInit();
    DSTATUS stat;
    stat = disk_initialize(0);
  #endif

  // Start the command line interface (if requested)
  #ifdef CFG_INTERFACE
    printf("%sType 'help' for a list of available commands%s", CFG_INTERFACE_NEWLINE, CFG_INTERFACE_NEWLINE);
    cmdInit();
  #endif
}

/**************************************************************************/
/*! 
    @brief Sends a single byte to a pre-determined peripheral (UART, etc.).

    @param[in]  byte
                Byte value to send
*/
/**************************************************************************/
void __putchar(const char c) 
{
  #ifdef CFG_PRINTF_UART
    // Send output to UART
    uartSendByte(c);
  #endif

  #ifdef CFG_PRINTF_USBCDC
    usbcdcSendByte(c);
  #endif

  #if defined CFG_PRINTF_NONE
    // Ignore output
  #endif
}

/**************************************************************************/
/*! 
    @brief Sends a string to a pre-determined end point (UART, etc.).

    @param[in]  str
                Text to send
*/
/**************************************************************************/
int puts(const char * str)
{
  while(*str) __putchar(*str++);
  return 0;
}

