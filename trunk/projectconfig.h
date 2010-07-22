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

/* Core CPU Settings (reference only) */
#define CFG_CPU_CCLK                (72000000)    // Ref. only.  Clock speed actually set in "core/cpu/cpu.c"

/* Systick Timer Settings */
#define CFG_SYSTICK_DELAY_IN_MS     (1)           // The number of milliseconds between each tick of the systick timer

/* UART Settings */
#define CFG_UART_BAUDRATE           (57600)       // Default UART speed
#define CFG_UART_BUFSIZE            (80)          // RX FIFO buffer size (the maximum number of received chars to store)

/* Test LED Pin Settings */
#define CFG_LED_PORT                (2)
#define CFG_LED_PIN                 (10)
#define CFG_LED_ON                  (0)           // The pin state to turn the LED on (0 = low, 1 = High)
#define CFG_LED_OFF                 (1)           // The pin state to turn the LED off (0 = low, 1 = High)

// #define CFG_SDCARD
#define CFG_SDCARD_CDPORT           (3)
#define CFG_SDCARD_CDPIN            (0)

/* USB Configuration */
#define CFG_USBHID
// #define CFG_USBCDC                             // Defaults to 115200 8N1
// #define CFG_USBCDC_BUFSIZE          (80)       // Transmit FIFO buffer size (max number of outgoing characters to store)

/* Printf Redirection */
// #define CFG_PRINTF_NONE                        // Ignore all printf output
#define CFG_PRINTF_UART                           // Use UART for printf output
// #define CFG_PRINTF_USBCDC                      // Use USB CDC for printf output

/* CLI Interface Settings */
#define CFG_INTERFACE
#define CFG_INTERFACE_MAXMSGSIZE    (80)          // The maximum number of bytes to accept for a command
#define CFG_INTERFACE_NEWLINE       "\r\n"        // This should be either \r\n (Windows-style) or \n (Unix-style)
#define CFG_INTERFACE_PROMPT        "LPC1343 >> " // The command-prompt text to display before each command

/* On-board EEPROM Settings */
#define CFG_I2CEEPROM

/* LM75B Temperature Settings (requires external HW) */
// #define CFG_LM75B

/* Chibi Wireless Stack Settings (requires external HW) */
// #define CFG_CHIBI
#define CFG_CHIBI_MODE              (BPSK20_868MHZ)     // See chb_drvr.h for possible values
#define CFG_CHIBI_POWER             (CHB_PWR_EU2_5DBM)  // See chb_drvr.h for possible values
#define CFG_CHIBI_EEPROM_IEEEADDR   (uint16_t)(0x0000)  // Start location in EEPROM for the full IEEE address
#define CFG_CHIBI_EEPROM_SHORTADDR  (uint16_t)(0x0009)  // Start location in EEPROM for the short (16-bit) address

// #define CFG_LCD
#define CFG_LCD_INCLUDESMALLFONTS   (0)           // 1 to include 'smallfont' support
#define CFG_LCD_WIDTH               (240)         // LCD width in pixels
#define CFG_LCD_HEIGHT              (320)         // LCD height in pixels

// #define CFG_TESTBED

// #####################
// Config error-checking
// #####################
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
#endif

#endif
