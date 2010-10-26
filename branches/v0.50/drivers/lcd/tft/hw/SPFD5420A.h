/**************************************************************************/
/*! 
    @file     SPFD5420A.h
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
#ifndef __SPFD5420A_H__
#define __SPFD5420A_H__

#include "projectconfig.h"

#include "drivers/lcd/tft/lcd.h"
#include "core/gpio/gpio.h"

// Control pins
#define SPFD5420_CS_PORT         1     // CS (LCD Pin 7)
#define SPFD5420_CS_PIN          8
#define SPFD5420_CD_PORT         1     // CS/RS (LCD Pin 8)
#define SPFD5420_CD_PIN          9
#define SPFD5420_WR_PORT         1     // WR (LCD Pin 9)
#define SPFD5420_WR_PIN          10
#define SPFD5420_RD_PORT         1     // RD (LCD Pin 10)
#define SPFD5420_RD_PIN          11

// These combined pin definitions are for optimisation purposes.
// If the pin values above are modified the bit equivalents
// below will also need to be updated
#define SPFD5420_CS_CD_PINS      0x300   // 8 + 9
#define SPFD5420_RD_WR_PINS      0xC00   // 11 + 10
#define SPFD5420_WR_CS_PINS      0x500   // 10 + 8
#define SPFD5420_CD_RD_WR_PINS   0xE00   // 9 + 11 + 10

// Backlight and Reset pins
#define SPFD5420_RES_PORT        3     // LCD Reset  (LCD Pin 31)
#define SPFD5420_RES_PIN         3
#define SPFD5420_BL_PORT         2     // Backlight Enable (LCD Pin 16)
#define SPFD5420_BL_PIN          9

// Data pins
// Note: data pins must be consecutive and on the same port
#define SPFD5420_DATA_PORT       2     // 8-Pin Data Port
#define SPFD5420_DATA_PIN1       1
#define SPFD5420_DATA_PIN2       2
#define SPFD5420_DATA_PIN3       3
#define SPFD5420_DATA_PIN4       4
#define SPFD5420_DATA_PIN5       5
#define SPFD5420_DATA_PIN6       6
#define SPFD5420_DATA_PIN7       7
#define SPFD5420_DATA_PIN8       8
#define SPFD5420_DATA_MASK       0x000001FE
#define SPFD5420_DATA_OFFSET     1     // Offset = PIN1

// These registers allow fast single operation clear+set of bits (see section 8.5.1 of LPC1343 UM)
#define SPFD5420_GPIO2DATA_DATA      (*(pREG32 (GPIO_GPIO2_BASE + (SPFD5420_DATA_MASK << 2))))
#define SPFD5420_GPIO1DATA_WR        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << SPFD5420_WR_PIN) << 2))))
#define SPFD5420_GPIO1DATA_CD        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << SPFD5420_CD_PIN) << 2))))
#define SPFD5420_GPIO1DATA_CS        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << SPFD5420_CS_PIN) << 2))))
#define SPFD5420_GPIO1DATA_RD        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << SPFD5420_RD_PIN) << 2))))
#define SPFD5420_GPIO3DATA_RES       (*(pREG32 (GPIO_GPIO3_BASE + ((1 << SPFD5420_RES_PIN) << 2))))
#define SPFD5420_GPIO1DATA_CS_CD     (*(pREG32 (GPIO_GPIO1_BASE + ((SPFD5420_CS_CD_PINS) << 2))))
#define SPFD5420_GPIO1DATA_RD_WR     (*(pREG32 (GPIO_GPIO1_BASE + ((SPFD5420_RD_WR_PINS) << 2))))
#define SPFD5420_GPIO1DATA_WR_CS     (*(pREG32 (GPIO_GPIO1_BASE + ((SPFD5420_WR_CS_PINS) << 2))))
#define SPFD5420_GPIO1DATA_CD_RD_WR  (*(pREG32 (GPIO_GPIO1_BASE + ((SPFD5420_CD_RD_WR_PINS) << 2))))

// Macros for control line state
#define CLR_CD          SPFD5420_GPIO1DATA_CD = (0)
#define SET_CD          SPFD5420_GPIO1DATA_CD = (1 << SPFD5420_CD_PIN)
#define CLR_CS          SPFD5420_GPIO1DATA_CS = (0)
#define SET_CS          SPFD5420_GPIO1DATA_CS = (1 << SPFD5420_CS_PIN)
#define CLR_WR          SPFD5420_GPIO1DATA_WR = (0)
#define SET_WR          SPFD5420_GPIO1DATA_WR = (1 << SPFD5420_WR_PIN)
#define CLR_RD          SPFD5420_GPIO1DATA_RD = (0)
#define SET_RD          SPFD5420_GPIO1DATA_RD = (1 << SPFD5420_RD_PIN)
#define CLR_RESET       SPFD5420_GPIO3DATA_RES = (0)
#define SET_RESET       SPFD5420_GPIO3DATA_RES = (1 << SPFD5420_RES_PIN)

// These 'combined' macros are defined to improve code performance by
// reducing the number of instructions in heavily used functions
#define CLR_CS_CD       SPFD5420_GPIO1DATA_CS_CD = (0);
#define SET_RD_WR       SPFD5420_GPIO1DATA_RD_WR = (SPFD5420_RD_WR_PINS);
#define SET_WR_CS       SPFD5420_GPIO1DATA_WR_CS = (SPFD5420_WR_CS_PINS);
#define SET_CD_RD_WR    SPFD5420_GPIO1DATA_CD_RD_WR = (SPFD5420_CD_RD_WR_PINS);

#endif
