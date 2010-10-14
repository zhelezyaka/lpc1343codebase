/**************************************************************************/
/*! 
    @file     pn532.c
    @author   K. Townsend (microBuilder.eu)

    @section DESCRIPTION

    UART-based driver for NXP's PN532 NFC transceiver
    
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

#include "pn532.h"
#include "core/uart/uart.h"

#ifdef CFG_PRINTF_USBCDC
  #include "core/usbcdc/cdcuser.h"
  char usbcdcBuf [32];
#endif

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// WARNING: This file is experimental and is not intended for production
//          or any other use
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**************************************************************************/
/*! 
    Prints a hexadecimal value in plain characters
*/
/**************************************************************************/
void pn532_print_hex(const byte_t* pbtData, const size_t szBytes)
{
  size_t szPos;
  for (szPos=0; szPos < szBytes; szPos++) {
    printf("%02x ",pbtData[szPos]);
  }
  printf(CFG_PRINTF_NEWLINE);
}

/**************************************************************************/
/*! 
    Polls the user input buffer (USB CDC, etc.) for any incoming text and
	redirects the data to the UART transmit buffer for the PN532 to read
*/
/**************************************************************************/
void pn532PollInput (void)
{
  #if defined CFG_PRINTF_USBCDC
    int  numBytesToRead, numBytesRead, numAvailByte;
    
    CDC_OutBufAvailChar (&numAvailByte);
    if (numAvailByte > 0) 
    {
      numBytesToRead = numAvailByte > 32 ? 32 : numAvailByte; 
      numBytesRead = CDC_RdOutBuf (&usbcdcBuf[0], &numBytesToRead);
      // Retransmit data to uart/PN532
      uartSend(usbcdcBuf, sizeof(numBytesRead));
    }
  #endif
}

/**************************************************************************/
/*! 
    Polls the PN532/UART buffer and retransmits it USB CDC (etc.)
*/
/**************************************************************************/
void pn532PollNFC (void)
{
  byte_t abtRx[256];
  size_t szRxLen;
	
  // Receive response and print to screen
  if (uartRxBufferReadArray(abtRx,&szRxLen))
  {
    printf("Received (%02d): ",szRxLen);
    pn532_print_hex(abtRx,szRxLen);
  }
}

/**************************************************************************/
/*! 
    Initialises the PN532 and send the wakeup sequence if the device.
*/
/**************************************************************************/
void pn532Init (void)
{
  byte_t abtWakeUp[] = { 0x55,0x55,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00 };

  // Configure UART
  // ToDo: Trust CFG_UART_BAUDRATE in projectconfig.h, or
  // force a value here at the risk of projectconfig.h being
  // misleading to someone unfamiliar with the code?
  uartInit(CFG_UART_BAUDRATE);

  // Initialise the UART buffer
  // Removed: This already happens in uartInit above
  // uartRxBufferInit();
  
  // Send wakeup sequence to the PN532
  uartSend(abtWakeUp,sizeof(abtWakeUp));
  
  // Printf goes to the PC (USB CDC) and uartSend + the uart
  // buffer for to the PN532
  printf("PN532 Initialised ... waiting for data");
  printf(CFG_PRINTF_NEWLINE);

  // ToDo: Return a value indicating is the init/wakeup was succesful ?
}

