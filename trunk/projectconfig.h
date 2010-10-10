/**************************************************************************/
/*! 
    @file     projectconfig.h
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

#ifndef _PROJECTCONFIG_H_
#define _PROJECTCONFIG_H_

#include "lpc134x.h"
#include "sysdefs.h"
#include "drivers/chibi/chb_drvr.h"

#define PLACE_IN_SRAM    __attribute__ ((section(".fast")))

/*=========================================================================
    CORE CPU SETTINGS
    -----------------------------------------------------------------------

    CFG_CPU_CCLK    Value is for reference only.  'core/cpu/cpu.c' must
                    be modified to change the clock speed, but the value
                    should be indicated here since CFG_CPU_CCLK is used by
                    other peripherals to determine timing.

    -----------------------------------------------------------------------*/
    #define CFG_CPU_CCLK                (72000000)
/*=========================================================================*/


/*=========================================================================
    SYSTICK TIMER
    -----------------------------------------------------------------------

    CFG_SYSTICK_DELAY_IN_MS   The number of milliseconds between each tick
                              of the systick timer.

    -----------------------------------------------------------------------*/
    #define CFG_SYSTICK_DELAY_IN_MS     (1)
/*=========================================================================*/


/*=========================================================================
    UART
    -----------------------------------------------------------------------

    CFG_UART_BAUDRATE         The default UART speed.  This value is used 
                              when initialising UART, and should be a 
                              standard value like 57600, 9600, etc.
    CFG_UART_BUFSIZE          The length in bytes of the UART RX FIFO. This
                              will determine the maximum number of received
                              characters to store in memory.

    -----------------------------------------------------------------------*/
    #define CFG_UART_BAUDRATE           (115200)
    #define CFG_UART_BUFSIZE            (80)
/*=========================================================================*/


/*=========================================================================
    ON-BOARD LED
    -----------------------------------------------------------------------

    CFG_LED_PORT              The port for the on board LED
    CFG_LED_PIN               The pin for the on board LED
    CFG_LED_ON                The pin state to turn the LED on (0 = low, 1 = high)
    CFG_LED_OFF               The pin state to turn the LED off (0 = low, 1 = high)

    -----------------------------------------------------------------------*/
    #define CFG_LED_PORT                (2)
    #define CFG_LED_PIN                 (10)
    #define CFG_LED_ON                  (0)
    #define CFG_LED_OFF                 (1)
/*=========================================================================*/


/*=========================================================================
    MICRO-SD CARD
    -----------------------------------------------------------------------

    CFG_SDCARD                If this field is defined SD Card and FAT32
                              file system support will be included
    CFG_SDCARD_CDPORT         The card detect port number
    CFG_SDCARD_CDPIN          The card detect pin number
    
    NOTE: CFG_SDCARD =        ~7.2 KB Flash and 0.6 KB SRAM (-Os)
    -----------------------------------------------------------------------*/
    // #define CFG_SDCARD
    #define CFG_SDCARD_CDPORT           (3)
    #define CFG_SDCARD_CDPIN            (0)
/*=========================================================================*/


/*=========================================================================
    USB
    -----------------------------------------------------------------------

    CFG_USBHID                If this field is defined USB HID support will
                              be included.  Currently uses ROM-based USB HID
    CFG_USBCDC                If this field is defined USB CDC support will
                              be included, with the USB Serial Port speed
                              set to 115200 BPS by default
    CFG_USBCDC_BAUDRATE       The default TX/RX speed.  This value is used 
                              when initialising USBCDC, and should be a 
                              standard value like 57600, 9600, etc.
    CFG_USBCDC_BUFSIZE        The size in bytes of the USB CDC transmit
                              FIFO buffer
    CFG_USBCDC_INITTIMEOUT    The maximum delay in milliseconds to wait for
                              USB to connect.  Must be a multiple of 10!

    NOTE: CFG_USBHID =        ~0.5 KB Flash and 36 bytes SRAM (-Os)
    NOTE: CFG_USBCDC =        ~4.0 KB Flash and 272 bytes SRAM (-Os)
    -----------------------------------------------------------------------*/
    // #define CFG_USBHID
    // #define CFG_USBCDC
    #define CFG_USBCDC_BAUDRATE         (115200)
    #define CFG_USBCDC_BUFSIZE          (80)
    #define CFG_USBCDC_INITTIMEOUT      (5000)
/*=========================================================================*/


/*=========================================================================
    PRINTF REDIRECTION
    -----------------------------------------------------------------------

    CFG_PRINTF_UART           Will cause all printf statements to be 
                              redirected to UART
    CFG_PRINTF_USBCDC         Will cause all printf statements to be
                              redirect to USB Serial
    CFG_PRINTF_CWDEBUG        Will cause all printf statements to be
                              redirected to the Crossworks
                              debug_printf statement (Crossworks only)
    CFG_PRINTF_NEWLINE        This should be either "\r\n" for Windows or
                              "\n" for *nix

    Note: If no printf redirection definitions are present, all printf
    output will be ignored, though this will also save ~350 bytes flash.

    NOTE: PRINTF Support =    ~350 bytes Flash (-Os)
    -----------------------------------------------------------------------*/
    // #define CFG_PRINTF_UART
    // #define CFG_PRINTF_USBCDC
    // #define CFG_PRINTF_CWDEBUG
    #define CFG_PRINTF_NEWLINE          "\r\n"
/*=========================================================================*/


/*=========================================================================
    COMMAND LINE INTERFACE
    -----------------------------------------------------------------------

    CFG_INTERFACE             If this field is defined the UART or USBCDC
                              based command-line interface will be included
    CFG_INTERFACE_MAXMSGSIZE  The maximum number of bytes to accept for an
                              incoming command
    CFG_INTERFACE_PROMPT      The command prompt to display at the start
                              of every new data entry line

    NOTE:                     The command-line interface will use either
                              USB-CDC or UART depending on whether
                              CFG_PRINTF_UART or CFG_PRINTF_USBCDC are 
                              selected.

    NOTE: CFG_INTERFACE =     ~6.0 KB Flash and 240 bytes SRAM (-Os), but
                              this varies with the number of commands
                              present
    -----------------------------------------------------------------------*/
    // #define CFG_INTERFACE
    #define CFG_INTERFACE_MAXMSGSIZE    (80)
    #define CFG_INTERFACE_PROMPT        "LPC1343 >> "
/*=========================================================================*/


/*=========================================================================
    EEPROM
    -----------------------------------------------------------------------

    CFG_I2CEEPROM             If defined, drivers for the onboard EEPROM
                              will be included during build

    -----------------------------------------------------------------------*/
    // #define CFG_I2CEEPROM
/*=========================================================================*/


/*=========================================================================
    LM75B TEMPERATURE SENSOR
    -----------------------------------------------------------------------

    CFG_LM75B                 If defined, drivers for an optional LM75B
                              temperature sensor will be included during
                              build (requires external HW)

    -----------------------------------------------------------------------*/
    // #define CFG_LM75B
/*=========================================================================*/


/*=========================================================================
    CHIBI WIRELESS STACK
    -----------------------------------------------------------------------

    CFG_CHIBI                   If defined, the CHIBI wireless stack will be
                                included during build.  Requires external HW.
    CFG_CHIBI_MODE              The mode to use when receiving and transmitting
                                with Chibi.  See chb_drvr.h for possible values
    CFG_CHIBI_POWER             The power level to use when transmitting.  See
                                chb_drvr.h for possible values
    CFG_CHIBI_BUFFERSIZE        The size of the message buffer in bytes.
    CFG_CHIBI_EEPROM_IEEEADDR   Start location in EEPROM for the full IEEE
                                address of this node
    CFG_CHIBI_EEPROM_SHORTADDR  Start location in EEPROM for the short (16-bit)
                                address of this node

    NOTE: CFG_CHIBI =           ~4.0 KB Flash and 184 bytes SRAM (-Os)
    -----------------------------------------------------------------------*/
    // #define CFG_CHIBI
    #define CFG_CHIBI_MODE              (BPSK20_868MHZ)     // See chb_drvr.h for possible values
    #define CFG_CHIBI_POWER             (CHB_PWR_EU2_5DBM)  // See chb_drvr.h for possible values
    #define CFG_CHIBI_BUFFERSIZE        (128)
    #define CFG_CHIBI_EEPROM_IEEEADDR   (uint16_t)(0x0000)
    #define CFG_CHIBI_EEPROM_SHORTADDR  (uint16_t)(0x0009)
/*=========================================================================*/


/*=========================================================================
    TFT LCD
    -----------------------------------------------------------------------

    CFG_TFTLCD                  If defined, this will cause drivers for
                                a pre-determined LCD screen to be included
                                during build.  Only one LCD driver can be 
                                included during the build process (for ex.
                                'drivers/lcd/hw/ILI9325.c')
    CFG_TFTLCD_INCLUDESMALLFONTS If set to 1, smallfont support will be
                                included for 3x6, 5x8, 7x8 and 8x8 fonts.
                                This should only be enabled if these small
                                fonts are required since there is already
                                support for larger fonts generated using
                                Dot Factory 
                                http://www.pavius.net/downloads/tools/53-the-dot-factory
    CFG_TFTLCD_WIDTH            The width in pixels of the LCD screen
    CFG_TFTLCD_HEIGHT           The height in pixels of the LCD screen

    NOTE: CFG_TFTLCD (ILI9325)  ~4.9 KB Flash (-Os, no small fonts, 
                                consolas9 used)
    -----------------------------------------------------------------------*/
    // #define CFG_TFTLCD
    #define CFG_TFTLCD_INCLUDESMALLFONTS   (0)
    #define CFG_TFTLCD_WIDTH               (240)
    #define CFG_TFTLCD_HEIGHT              (320)
/*=========================================================================*/


/*=========================================================================
    ST7565 128x64 Graphic LCD
    -----------------------------------------------------------------------

    CFG_ST7565                  If defined, this will cause drivers for
                                the 128x64 pixel ST7565 LCD to be included

    -----------------------------------------------------------------------*/
    #define CFG_ST7565
/*=========================================================================*/




/*=========================================================================
  CONFIG FILE VALIDATION
  -------------------------------------------------------------------------
  Basic error checking to make sure that incompatible defines are not 
  enabled at the same time, etc.

  =========================================================================*/

#if !defined CFG_PRINTF_NONE
  #if defined CFG_PRINTF_USBCDC && defined CFG_PRINTF_UART
    #error "CFG_PRINTF_UART or CFG_PRINTF_USBCDC cannot both be defined at once"
  #endif
  #if defined CFG_PRINTF_USBCDC && !defined CFG_USBCDC
    #error "CFG_PRINTF_CDC requires CFG_USBCDC to be defined as well"
  #endif
#endif
#if defined CFG_PRINTF_NONE && defined CFG_PRINTF_UART
  #error "CFG_PRINTF_NONE and CFG_PRINTF_UART can not be defined at the same time"
#endif
#if defined CFG_PRINTF_NONE && defined CFG_PRINTF_USBCDC
  #error "CFG_PRINTF_NONE and CFG_PRINTF_USBCDC can not be defined at the same time"
#endif

#if defined CFG_USBCDC && defined CFG_USBHID
  #error "Only one USB class can be defined at a time (CFG_USBCDC or CFG_USBHID)"
#endif

#ifdef CFG_INTERFACE
  #if defined CFG_PRINTF_NONE
    #error "CFG_INTERFACE can not be defined with CFG_PRINTF_NONE"
  #endif
  #if !defined CFG_PRINTF_UART && !defined CFG_PRINTF_USBCDC
    #error "CFG_PRINTF_UART or CFG_PRINTF_USBCDC must be defined for for CFG_INTERFACE Input/Output"
  #endif
#endif

#ifdef CFG_CHIBI
  #if !defined CFG_I2CEEPROM
    #error "CFG_CHIBI requires CFG_I2CEEPROM to store and retrieve addresses"
  #endif
  #ifdef CFG_SDCARD
    #error "CFG_CHIBI and CFG_SDCARD can not be defined at the same time. Only one SPI block is available on the LPC1343."
  #endif
#endif

#ifdef CFG_TFTLCD
  #ifdef CFG_ST7565
    #error "CFG_TFTLCD and CFG_ST7565 can not be defined at the same time."
  #endif
#endif

#endif
