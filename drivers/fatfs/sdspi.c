/**************************************************************************/
/*! 
    @file     sdspi.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section DESCRIPTION

    A wrapper for SSP/SPI access
        
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

#include "core/ssp/ssp.h"

#include "sdspi.h"

/* Local functions */
static uint8_t sdspiSendRecvByte (uint8_t byte_s);

/**************************************************************************/
/*! 
    @brief Initialises SPI0 at default speed (4MHz)
*/
/**************************************************************************/
void sdspiInit (void) 
{
  // Clock low between frames and transition on leading edge
  sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge);

  // Does speed need to be set to 400KHz?  Currently 4MHz by default
  // sdspiSetSpeed (SPI_SPEED_400kHz);
}

/**************************************************************************/
/*! 
    @brief Sets the SSEL pin low (select)
*/
/**************************************************************************/
void sdspiSelect ()
{
  // Macro defined in ssp.h
  ssp0Select();
}

/**************************************************************************/
/*! 
    @brief Sets the SSEL pin high (deselect)
*/
/**************************************************************************/
void sdspiDeSelect ()
{
  // Macro defined in ssp.h
  ssp0Deselect();
}

/**************************************************************************/
/*! 
    @brief Send one byte then receive one response byte

    @param[in]  byte_s
                The byte to send
*/
/**************************************************************************/
static uint8_t sdspiSendRecvByte (uint8_t byte_s)
{
  uint8_t r[1];

  sdspiSelect();
  sspSend(0, (uint8_t *)&byte_s, 1);
  sspReceive(0, (uint8_t *)&r, 1); 
  sdspiDeSelect();

  /* Return received value */
  return r[0];
}

/**************************************************************************/
/*! 
    @brief Send one byte
*/
/**************************************************************************/
void sdspiSendByte (uint8_t data)
{
  sdspiSendRecvByte (data);	
}


/**************************************************************************/
/*! 
    @brief Received one byte
*/
/**************************************************************************/
uint8_t sdspiRecvByte ()
{
  return sdspiSendRecvByte (0xFF);	
}

/**************************************************************************/
/*! 
    @brief Frees SPI for other devices to use it
*/
/**************************************************************************/
void sdspiRelease (void)
{
  sdspiDeSelect ();
}

