/**************************************************************************/
/*! 
    @file     sd.h
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
#ifndef _SD_H__ 
#define _SD_H__ 

#include <stdint.h>
#include <stdbool.h>

/* SD/MMC Commands */
#define GO_IDLE_STATE               (0x40 + 0)    // CMD0
#define SEND_OP_COND                (0x40 + 1)
#define CMD8                        (0x40 + 8)    // CMD8
#define SEND_CSD                    (0x40 + 9)
#define SEND_CID                    (0x40 + 10)   // CMD10
#define STOP_TRAN                   (0x40 + 12)   // CMD12
#define SET_BLOCKLEN                (0x40 + 16)   // CMD16
#define READ_BLOCK                  (0x40 + 17)
#define READ_MULT_BLOCK             (0x40 + 18)
#define WRITE_BLOCK                 (0x40 + 24)
#define WRITE_MULT_BLOCK            (0x40 + 25)
#define APP_CMD                     (0x40 + 55)   // CMD55
#define READ_OCR                    (0x40 + 58)   // CMD58
#define CRC_ON_OFF                  (0x40 + 59)
#define SD_SEND_OP_COND             (0xC0 + 41)   // ACMD41
#define SD_STATUS                   (0xC0 + 13)   // ACMD13, SD_STATUS (SDC) 
#define SET_WR_BLK_ERASE_COUNT      (0xC0 + 23)   // ACMD23 (SDC) 

/* Card type flags (CardType) */
#define CT_NONE                     0x00
#define CT_MMC                      0x01
#define CT_SD1                      0x02
#define CT_SD2                      0x04
#define CT_SDC                      (CT_SD1|CT_SD2)
#define CT_BLOCK                    0x08

/* MMC device configuration */
typedef struct tagSDCFG
{
  uint32_t sernum;        // serial number
  uint32_t size;          // size=sectorsize*sectorcnt
  uint32_t sectorcnt;
  uint32_t sectorsize;    // 512
  uint32_t blocksize;     // erase block size
  uint8_t ocr[4];         // OCR
  uint8_t cid[16];        // CID
  uint8_t csd[16];        // CSD
} SDCFG;

#endif
