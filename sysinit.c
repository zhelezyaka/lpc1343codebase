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
  #include "core/ssp/ssp.h"
  #include "drivers/fatfs/diskio.h"
  #include "drivers/fatfs/ff.h"
  static FILINFO Finfo;
  static FATFS Fatfs[1];
  static uint8_t buf[64];
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
    uint16_t darkGray = drawRGB24toRGB565(0x33, 0x33, 0x33);
    uint16_t lightGray = drawRGB24toRGB565(0xCC, 0xCC, 0xCC);

    // Draw background
    drawFill(lightGray);
    drawRectangleFilled(1, 1, 240, 20, darkGray);
    drawRectangleFilled(1, 240, 240, 320, darkGray);

    // Render some text
    #if defined CFG_LCD_INCLUDESMALLFONTS & CFG_LCD_INCLUDESMALLFONTS == 1
      drawStringSmall(1, 210, WHITE, "5x8 System (Max 40 Characters)", Font_System5x8);
      drawStringSmall(1, 220, WHITE, "7x8 System (Max 30 Characters)", Font_System7x8);
    #endif
    drawString(5,   8,    WHITE,    &consolas9ptFontInfo,   "LPC1343 Demo Code v0.25");

    // Draw some primitive shapes
    drawCircle(15, 300, 10, WHITE);
    drawLine(100, 280, 200, 310, WHITE);
    drawRectangle (220, 5, 230, 15, WHITE);
    drawRectangleFilled (222, 7, 228, 13, lightGray);

    // Draw some compound shapes
    drawString(10,   150,    BLACK,    &consolas9ptFontInfo,   "Green Progress");
    drawProgressBar(100, 150, 130, 9, WHITE, BLACK, lightGray, GREEN, 75);
    drawString(10,   165,    BLACK,    &consolas9ptFontInfo,   "Yellow Progress");
    drawProgressBar(100, 165, 130, 9, WHITE, BLACK, lightGray, YELLOW, 23);
    drawString(10,   180,    RED,    &consolas9ptFontInfo,   "Red Progress");
    drawProgressBar(100, 180, 130, 9, WHITE, BLACK, lightGray, RED, 64);
    drawString(10,   200,    BLACK,    &consolas9ptFontInfo,   "Battery");
    drawProgressBar(100, 195, 130, 15, WHITE, BLACK, lightGray, BLUE, 90);

    // uint16_t getc;
    // getc = lcdGetPixel(0, 0);
    // drawFill(getc);

    // drawImageFromFile(1, 1, "mur.pic");
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
    printf("%-40s : 0x%04X%s", "Chibi Initialised", pcb->src_addr, CFG_PRINTF_NEWLINE);
  #endif

  #ifdef CFG_SDCARD
    // Init SSP w/clock low between frames and transition on leading edge
    sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge);
    DSTATUS stat;
    stat = disk_initialize(0);
    if (stat & STA_NOINIT) 
    {
      printf("%-40s : %s%s", "MMC", "Not Initialised", CFG_PRINTF_NEWLINE);
    }
    if (stat & STA_NODISK) 
    {
      printf("%-40s : %s%s", "MMC", "No Disk", CFG_PRINTF_NEWLINE);
    }
    if (stat == 0)
    {
      DSTATUS stat;
      DWORD p2;
      WORD w1;
      BYTE res, b1;
      DIR dir;

      // SD Card Initialised
      printf("%-40s : %s%s", "MMC", "Initialised", CFG_PRINTF_NEWLINE);
      // Drive size
      if (disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK) 
      {
        printf("%-40s : %d%s", "MMC Drive Size", p2, CFG_PRINTF_NEWLINE);
      }
      // Sector Size
      if (disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK) 
      {
        printf("%-40s : %d%s", "MMC Sector Size", w1, CFG_PRINTF_NEWLINE);
      }
      // Card Type
      if (disk_ioctl(0, MMC_GET_TYPE, &b1) == RES_OK) 
      {
        printf("%-40s : %d%s", "MMC Card Type", b1, CFG_PRINTF_NEWLINE);
      }
      // Try to mount drive
      res = f_mount(0, &Fatfs[0]);
      if (res != FR_OK) 
      {
        printf("%-40s : %d%s", "MMC - Failed to mount 0:", res, CFG_PRINTF_NEWLINE);
      }
      if (res == FR_OK)
      {
        res = f_opendir(&dir, "/");
        if (res) 
        {
            printf("%-40s : %d%s", "MMC - Failed to open /:", res, CFG_PRINTF_NEWLINE);
            return;
        }
        // Read dir
        for(;;) 
        {
            res = f_readdir(&dir, &Finfo);
            if ((res != FR_OK) || !Finfo.fname[0]) break;
            #if _USE_LFN == 0
              printf("%-25s %s", (char *)&Finfo.fname[0], CFG_PRINTF_NEWLINE);
            #else
              printf("%-75s %s", (char *)&Finfo.lfname[0], CFG_PRINTF_NEWLINE);
            #endif
        }
        // Create a file
        //FIL logFile;  
        //if(f_open(&logFile, "/log.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) 
        //{  
        //  // Flag error  
        //  printf ("Unabled to create log.txt%s", CFG_PRINTF_NEWLINE); 
        //}  
        //unsigned int bytesWritten;  
        //f_write(&logFile, "New log opened!\n", 16, &bytesWritten);  
        //// Flush the write buffer (required?)
        //// f_sync(&logFile);  
        //// Close and unmount.   
        //f_close(&logFile);  
        //f_mount(0,0); 
        //printf("Wrote data to log.txt", CFG_PRINTF_NEWLINE);
      }
    }
  #endif

  // Start the command line interface (if requested)
  #ifdef CFG_INTERFACE
    printf("%sType 'help' for a list of available commands%s", CFG_PRINTF_NEWLINE, CFG_PRINTF_NEWLINE);
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

