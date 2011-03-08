/**************************************************************************/
/*! 
    @file     pn532.h
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2011, microBuilder SARL
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

#ifndef __PN532_H__
#define __PN532_H__

#include "projectconfig.h"

#define PN532_INTERFACE_UART

#define PN532_DEBUG(fmt, args...)             printf(fmt, ##args) 

#define PN532_NORMAL_FRAME__DATA_MAX_LEN      (254)
#define PN532_NORMAL_FRAME__OVERHEAD          (8)
#define PN532_EXTENDED_FRAME__DATA_MAX_LEN    (264)
#define PN532_EXTENDED_FRAME__OVERHEAD        (11)
#define PN532_BUFFER_LEN                      (PN532_EXTENDED_FRAME__DATA_MAX_LEN + PN532_EXTENDED_FRAME__OVERHEAD)
#define PN532_UART_BAUDRATE                   (115200)

// PN532 Protocol control block
typedef struct _pn532_pcb_t
{
  BOOL initialised;
  BOOL awake;
} pn532_pcb_t;

enum
{
  PN532_MODULATION_ISO14443A          = 0x00,
  PN532_MODULATION_FELICA_212KBPS     = 0x01,
  PN532_MODULATION_FELICA_424KBPS     = 0x02,
  PN532_MODULATION_ISO14443B          = 0x03,
  PN532_MODULATION_JEWEL              = 0x04
};

enum
{
  PN532_COMMAND_DIAGNOSE              = 0x00,
  PN532_COMMAND_GETFIRMWAREVERSION    = 0x02,
  PN532_COMMAND_GETGENERALSTATUS      = 0x04,
  PN532_COMMAND_READREGISTER          = 0x06,
  PN532_COMMAND_WRITEREGISTER         = 0x08,
  PN532_COMMAND_READGPIO              = 0x0C,
  PN532_COMMAND_WRITEGPIO             = 0x0E,
  PN532_COMMAND_SETSERIALBAUDRATE     = 0x10,
  PN532_COMMAND_SETPARAMETERS         = 0x12,
  PN532_COMMAND_SAMCONFIGURATION      = 0x14,
  PN532_COMMAND_POWERDOWN             = 0x16,
  PN532_COMMAND_RFCONFIGURATION       = 0x32,
  PN532_COMMAND_RFREGULATIONTEST      = 0x58,
  PN532_COMMAND_INJUMPFORDEP          = 0x56,
  PN532_COMMAND_INJUMPFORPSL          = 0x46,
  PN532_COMMAND_INLISTPASSIVETARGET   = 0x4A,
  PN532_COMMAND_INATR                 = 0x50,
  PN532_COMMAND_INPSL                 = 0x4E,
  PN532_COMMAND_INDATAEXCHANGE        = 0x40,
  PN532_COMMAND_INCOMMUNICATETHRU     = 0x42,
  PN532_COMMAND_INDESELECT            = 0x44,
  PN532_COMMAND_INRELEASE             = 0x52,
  PN532_COMMAND_INSELECT              = 0x54,
  PN532_COMMAND_INAUTOPOLL            = 0x60,
  PN532_COMMAND_TGINITASTARGET        = 0x8C,
  PN532_COMMAND_TGSETGENERALBYTES     = 0x92,
  PN532_COMMAND_TGGETDATA             = 0x86,
  PN532_COMMAND_TGSETDATA             = 0x8E,
  PN532_COMMAND_TGSETMETADATA         = 0x94,
  PN532_COMMAND_TGGETINITIATORCOMMAND = 0x88,
  PN532_COMMAND_TGRESPONSETOINITIATOR = 0x90,
  PN532_COMMAND_TGGETTARGETSTATUS     = 0x8A
};

pn532_pcb_t *pn532GetPCB();
void pn532Init();
void pn532Wakeup(void);
void pn532SendCommand(byte_t * abtCommand, size_t szLen);

#endif
