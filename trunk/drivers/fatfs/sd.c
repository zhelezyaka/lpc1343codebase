/**************************************************************************/
/*! 
    @file     sd.c
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

#include "core/ssp/ssp.h"
#include "diskio.h"
#include "sd.h"
#include "sdspi.h"

/* 512 bytes for each sector */
#define SD_SECTOR_SIZE          512

/* token for write operation */
#define TOKEN_SINGLE_BLOCK      0xFE
#define TOKEN_MULTI_BLOCK       0xFC
#define TOKEN_STOP_TRAN         0xFD

/* Local variables */
static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */
static volatile	BYTE    Timer1, Timer2;	/* 100Hz decrement timer */
static uint8_t          CardType;
static SDCFG            SDCfg;

/* Local Function Prototypes */
static bool 	  sdInit            (void);
static uint8_t    sdSendCmd         (uint8_t cmd, uint32_t arg);
static bool 	  sdReadSector      (uint32_t sector, uint8_t *buff, uint32_t count);
static bool 	  sdReadDataBlock   (uint8_t *buff, uint32_t cnt);
static bool 	  sdWriteSector     (uint32_t sector, const uint8_t *buff, uint32_t count);
static bool 	  sdWriteDataBlock  (const uint8_t *buff, uint8_t token);
static bool 	  sdWaitForReady    (void);
static bool       sdReadCfg         (SDCFG *cfg);

/**************************************************************************/
/*! 
    @brief Initialise the disk drive

    @param[in]  drv
                Physical drive number (always 0)
*/
/**************************************************************************/
DSTATUS disk_initialize ( BYTE drv )
{
  if (drv) return STA_NOINIT;               /* Supports only single drive */
  // if (Stat & STA_NODISK) return Stat;    /* No card in the socket */

  if (sdInit() && sdReadCfg(&SDCfg))
      Stat &= ~STA_NOINIT;

  return Stat;
}

/**************************************************************************/
/*! 
    @brief IO control

    @param[in]  drv
                Physical drive number (always 0)
    @param[in]  ctrl
                Control code
    @param[in]  buff
                Buffer to send/receive control data
*/
/**************************************************************************/
#if _USE_IOCTL != 0
DRESULT disk_ioctl ( BYTE drv, BYTE ctrl, void *buff )
{
  DRESULT res;
  BYTE n, *ptr = buff;
  
  if (drv) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  res = RES_ERROR;
  
  switch (ctrl) 
  {
    case CTRL_SYNC :		/* Make sure that no pending write process */
      if (sdWaitForReady() == true)	
        res = RES_OK;
      break;

    case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
      *(DWORD*)buff = SDCfg.sectorcnt;
      res = RES_OK;
      break;

    case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
      *(WORD*)buff = SDCfg.sectorsize;	//512;
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
      *(DWORD*)buff = SDCfg.blocksize;
      res = RES_OK;
      break;

    case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
      *ptr = CardType;
      res = RES_OK;
      break;

    case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
      for (n=0;n<16;n++)
        *(ptr+n) = SDCfg.csd[n]; 
      res = RES_OK;
      break;

    case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
      for (n=0;n<16;n++)
        *(ptr+n) = SDCfg.cid[n];
      res = RES_OK;
      break;

    case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
      for (n=0;n<4;n++)
        *(ptr+n) = SDCfg.ocr[n];
      res = RES_OK;
      break;

    case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
      if (sdSendCmd(SD_STATUS, 0) == 0)   /* SD_STATUS */
      {	
        sdspiRecvByte();
        if (sdReadDataBlock(ptr, 64))
          res = RES_OK;
      }
      break;

    default:
      res = RES_PARERR;
  }

  sdspiRelease ();
  return res;
}
#endif /* _USE_IOCTL != 0 */

/**************************************************************************/
/*! 
    @brief read sector(s)

    @param[in]  drv
                Physical drive number (always 0)
    @param[out] buff
                Pointer to the data buffer to store read data
    @param[in]  sector
                Start sector number (LBA)
    @param[in]  count
                Sector count (1..255)
*/
/**************************************************************************/
DRESULT disk_read (BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
  if (drv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  if (sdReadSector (sector, buff, count) == true)	
    return RES_OK;
  else
    return RES_ERROR;
}

/**************************************************************************/
/*! 
    @brief get disk statius

    @param[in]  drv
                Physical drive number (always 0)
*/
/**************************************************************************/
DSTATUS disk_status (BYTE drv)
{
  if (drv) return STA_NOINIT;		/* Supports only single drive */
  
  return Stat;
}

/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure                                      */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */
void disk_timerproc (void)
{
  BYTE n;
  
  n = Timer1;						/* 100Hz decrement timer */
  if (n) Timer1 = --n;
  n = Timer2;
  if (n) Timer2 = --n;
}

/**************************************************************************/
/*! 
    @brief Write sector(s)

    @param[in]  drv
                Physical drive number (always 0)
    @param[in]  buff
                Pointer to the data to be written
    @param[in]  sector
                Start sector number (LBA)
    @param[in]  count
                Sector count (1..255)
*/
/**************************************************************************/
#if _READONLY == 0
DRESULT disk_write (BYTE drv, const BYTE *buff,	DWORD sector, BYTE count)
{
  if (drv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  // if (Stat & STA_PROTECT) return RES_WRPRT;

  if ( sdWriteSector(sector, buff, count) == true)
    return RES_OK;
  else
    return RES_ERROR;
}
#endif /* _READONLY == 0 */

/**************************************************************************/
/*! 
    @brief Wait until the card is no busy
*/
/**************************************************************************/
static bool sdWaitForReady (void)
{
  uint8_t res;
  /* timeout should be large enough to make sure the write operation can be completed. */
  uint32_t timeout = 400000;
  
  sdspiSendByte(0xFF);
  do 
  {
    res = sdspiRecvByte();
  } while ((res != 0xFF) && timeout--);
  
  return (res == 0xFF ? true : false);
}

/**************************************************************************/
/*! 
    @brief Initialise the SD/MMC card
*/
/**************************************************************************/
static bool sdInit (void) 
{
  uint32_t i, timeout;
  uint8_t cmd, ct, ocr[4];
  bool ret = false;
  
  /* Initialize SPI interface and enable Flash Card SPI mode. */
  sdspiInit ();

  /* At least 74 clock cycles are required prior to starting bus communication */
  for (i = 0; i < 80; i++)    /* 80 dummy clocks */
  {
    sdspiSendByte (0xFF);
  }

  ct = CT_NONE;
  if (sdSendCmd (GO_IDLE_STATE, 0) == 0x1)
  {
    timeout = 50000;
    if (sdSendCmd(CMD8, 0x1AA) == 1)    /* SDHC */
    { 
      /* Get trailing return value of R7 resp */
      for (i = 0; i < 4; i++) ocr[i] = sdspiRecvByte();
      if (ocr[2] == 0x01 && ocr[3] == 0xAA) 
      {
        /* Wait for leaving idle state (ACMD41 with HCS bit) */
        while (timeout-- && sdSendCmd(SD_SEND_OP_COND, 1UL << 30));	
        /* Check CCS bit in the OCR */
        if (timeout && sdSendCmd(READ_OCR, 0) == 0) 
        {
          for (i = 0; i < 4; i++) ocr[i] = sdspiRecvByte();
          ct = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
        }
      } 
      else   /* SDSC or MMC */
      {  
        if (sdSendCmd(SD_SEND_OP_COND, 0) <= 1)
        {
          ct = CT_SD1; cmd = SD_SEND_OP_COND;	/* SDSC */
        } 
        else 
        {
          ct = CT_MMC; cmd = SEND_OP_COND;	/* MMC */
        }
        /* Wait for leaving idle state */
        while (timeout-- && sdSendCmd(cmd, 0));			
        /* Set R/W block length to 512 */
        if (!timeout || sdSendCmd(SET_BLOCKLEN, SD_SECTOR_SIZE) != 0)
        {
          ct = CT_NONE;
        }                                        
      }
    }
  }
  CardType = ct;
  sdspiRelease();

  if (ct)   /* Initialization succeeded */
  {
    ret = true;
    // sdspiSetSpeed(SPI_SPEED_20MHz);
  } 
  else      /* Initialization failed */
  {
    sdspiSelect ();
    sdWaitForReady ();
    // sdspiDeInit();
  }
  
  return ret;
}

/**************************************************************************/
/*! 
    @brief Send a command to the flash card and get a response

    @param[in]  cmd
                Command index
    @param[in]  arg
                Argument for the command
*/
/**************************************************************************/
static uint8_t sdSendCmd (uint8_t cmd, uint32_t arg) 
{
  uint32_t r1, n;
  
  if (cmd & 0x80)                           /* ACMD<n> is the command sequence of CMD55-CMD<n> */
  {	
    cmd &= 0x7F;
    r1 = sdSendCmd(APP_CMD, 0);             /* CMD55 */
    if (r1 > 1) return r1;                  /* cmd send failed */
  }
  
  /* Select the card and wait for ready */
  sdspiDeSelect();
  sdspiSelect();
  if (sdWaitForReady() == false ) return 0xFF;
  
  sdspiSendByte (0xFF);                     /* prepare 8 clocks */ 
  sdspiSendByte (cmd);	 
  sdspiSendByte (arg >> 24);
  sdspiSendByte (arg >> 16);
  sdspiSendByte (arg >> 8);
  sdspiSendByte (arg);

  /* Checksum, should only be valid for the first command.CMD0 */
  n = 0x01;                                 /* Dummy CRC + Stop */
  if (cmd == GO_IDLE_STATE) n = 0x95;       /* Valid CRC for CMD0(0) */
  if (cmd == CMD8) n = 0x87;                /* Valid CRC for CMD8(0x1AA) */
  sdspiSendByte(n); 
  
  if (cmd == STOP_TRAN) sdspiRecvByte ();   /* Skip a stuff byte when stop reading */
  
  n = 10;                                   /* Wait for a valid response in timeout of 10 attempts */
  do 
  {
    r1 = sdspiRecvByte ();
  } while ((r1 & 0x80) && --n);
  
  return (r1);                              /* Return with the response value */
}

/**************************************************************************/
/*! 
    @brief Reads "count" sector(s) starting from sector index "sector"

    @param[in]  sector
                Starting sector
    @param[out] buff
                Pointer to bufer to hold read data
    @param[in]  count
                number of sectors to read
*/
/**************************************************************************/
static bool sdReadSector (uint32_t sector, uint8_t *buff, uint32_t count)
{
  /* Convert to byte address if needed */
  if (!(CardType & CT_BLOCK)) sector *= SD_SECTOR_SIZE;	
  
  if (count == 1)  /* Single block read */
  {
    if ((sdSendCmd(READ_BLOCK, sector) == 0)
      && sdReadDataBlock(buff, SD_SECTOR_SIZE))
      count = 0;
  } 
  else            /* Multiple block read */
  {   
    if (sdSendCmd(READ_MULT_BLOCK, sector) == 0) 
    {
      do 
      {
        if (!sdReadDataBlock(buff, SD_SECTOR_SIZE)) break;
        buff += SD_SECTOR_SIZE;
      } while (--count);

      sdSendCmd(STOP_TRAN, 0);	/* STOP_TRANSMISSION */
    }
  }
  sdspiRelease();
  
  return count ? false : true;	
}

/**************************************************************************/
/*! 
    @brief Read data into the supplied buffer

    @param[out] buff
                Point to the data buffer to store data into 
    @param[in]  cnt
                Byte count (must be multiple of 4, normally 512)
*/
/**************************************************************************/
static bool sdReadDataBlock ( uint8_t *buff, uint32_t cnt)
{
  uint8_t token;
  uint32_t timeout;
  
  timeout = 20000;

  /* Wait for data packet in timeout of 100ms */
  do
  {
    token = sdspiRecvByte();
  } while ((token == 0xFF) && timeout--);

  /* If not valid data token, return with error */
  if(token != 0xFE) return false;
  
  /* Receive the data block into buffer */
  do
  {
    *buff++ = sdspiRecvByte ();
    *buff++ = sdspiRecvByte ();
    *buff++ = sdspiRecvByte ();
    *buff++ = sdspiRecvByte ();
  } while (cnt -= 4);
  
  sdspiRecvByte ();                     /* Discard CRC */
  sdspiRecvByte ();
  
  /* Return with success */
  return true;
}

#if _READONLY == 0
/**************************************************************************/
/*! 
    @brief Write "count" sector(s) starting from sector index "sector"

    @param[in]  sector
                Starting sector
    @param[in]  buff
                Pointer to the buffer containing the data to write
    @param[in]  count
                Number of sector to write
*/
/**************************************************************************/
static bool sdWriteSector (uint32_t sector, const uint8_t *buff, uint32_t count) 
{
  /* Convert to byte address if needed */	
  if (!(CardType & CT_BLOCK)) sector *= 512;
  
  if (count == 1) /* Single block write */
  {
    if ((sdSendCmd(WRITE_BLOCK, sector) == 0)
      && sdWriteDataBlock(buff, TOKEN_SINGLE_BLOCK))
      count = 0;
  } 
  else /* Multiple block write */
  {
    if (CardType & CT_SDC) sdSendCmd(SET_WR_BLK_ERASE_COUNT, count);
    if (sdSendCmd(WRITE_MULT_BLOCK, sector) == 0)
    {
      do 
      {
        if (!sdWriteDataBlock(buff, TOKEN_MULTI_BLOCK)) break;
        buff += 512;
      } while (--count);
      #if 1
        if (!sdWriteDataBlock(0, TOKEN_STOP_TRAN))	/* STOP_TRAN token */
                count = 1;
      #else
        sdspiSendByte(TOKEN_STOP_TRAN);
      #endif
    }
  }
  sdspiRelease(); 
  return count ? false : true;
}

/**************************************************************************/
/*! 
    @brief Write 512 bytes

    @param[in]  buff
                512 byte data block to be transmitted
    @param[in]  token
                0xFE -> single block
                0xFC -> multi block
                0xFD -> stop
*/
/**************************************************************************/
static bool sdWriteDataBlock (const uint8_t *buff, uint8_t token)
{
  uint8_t resp;
  uint8_t i = 0; // avoid warning
  
  sdspiSendByte (token);                  /* send data token first*/
  
  if (token != TOKEN_STOP_TRAN) 
  {
  /* Send data. */
  for (i = 512/4; i ; i--) 
  { 
    sdspiSendByte (*buff++);
    sdspiSendByte (*buff++);
    sdspiSendByte (*buff++);
    sdspiSendByte (*buff++);
  }
    sdspiSendByte(0xFF);                  /* 16-bit CRC (Dummy) */
    sdspiSendByte(0xFF);
  
    resp = sdspiRecvByte();               /* Receive data response */
    if ((resp & 0x1F) != 0x05)            /* If not accepted, return with error */
      return false;
  
    if ( sdWaitForReady() == false)       /* Wait while Flash Card is busy. */
      return false;
  }
  
  return true;
}
#endif /* _READONLY */

/**************************************************************************/
/*! 
    @brief Read MMC/SD card device configuration

    @param[in]  cfg
                Pointer to SDCFG
*/
/**************************************************************************/
static bool sdReadCfg (SDCFG *cfg) 
{	   
	uint8_t i;	
	uint16_t csize;
	uint8_t n, csd[16];
	bool retv = false;
	
	/* Read the OCR - Operations Condition Register. */
	if (sdSendCmd (READ_OCR, 0) != 0x00) goto x;
	for (i = 0; i < 4; i++)	cfg->ocr[i] = sdspiRecvByte ();
  	
	/* Read the CID - Card Identification. */
	if ((sdSendCmd (SEND_CID, 0) != 0x00) || 
		(sdReadDataBlock (cfg->cid, 16) == false))
		goto x;

    /* Read the CSD - Card Specific Data. */
	if ((sdSendCmd (SEND_CSD, 0) != 0x00) || 
		(sdReadDataBlock (cfg->csd, 16) == false))
		goto x;

	cfg -> sectorsize = SD_SECTOR_SIZE;

	/* Get number of sectors on the disk (DWORD) */
	if ((cfg->csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
		csize = cfg->csd[9] + ((uint16_t)cfg->csd[8] << 8) + 1;
		cfg -> sectorcnt = (uint32_t)csize << 10;
	} else {					/* SDC ver 1.XX or MMC*/
		n = (cfg->csd[5] & 15) + ((cfg->csd[10] & 128) >> 7) + ((cfg->csd[9] & 3) << 1) + 2;  // 19
		csize = (cfg->csd[8] >> 6) + ((uint16_t)cfg->csd[7] << 2) + ((uint16_t)(cfg->csd[6] & 3) << 10) + 1; // 3752
		cfg -> sectorcnt = (uint32_t)csize << (n - 9); // 3842048
	}

	cfg->size = cfg -> sectorcnt * cfg -> sectorsize; // 512*3842048=1967128576Byte (1.83GB)

	/* Get erase block size in unit of sector (DWORD) */
	if (CardType & CT_SD2) {			/* SDC ver 2.00 */
		if (sdSendCmd(SD_STATUS /*ACMD13*/, 0) == 0) {		/* Read SD status */
			sdspiRecvByte();
			if (sdReadDataBlock(csd, 16)) {				/* Read partial block */
				for (n = 64 - 16; n; n--) sdspiRecvByte();	/* Purge trailing data */
				cfg->blocksize = 16UL << (csd[10] >> 4);
				retv = true;
			}
		}
	} else {					/* SDC ver 1.XX or MMC */
		if ((sdSendCmd(SEND_CSD, 0) == 0) && sdReadDataBlock(csd, 16)) {	/* Read CSD */
			if (CardType & CT_SD1) {			/* SDC ver 1.XX */
				cfg->blocksize = (((csd[10] & 63) << 1) + ((uint16_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
			} else {					/* MMC */
				// cfg->blocksize = ((uint16_t)((buf[10] & 124) >> 2) + 1) * (((buf[11] & 3) << 3) + ((buf[11] & 224) >> 5) + 1);
				cfg->blocksize = ((uint16_t)((cfg->csd[10] & 124) >> 2) + 1) * (((cfg->csd[10] & 3) << 3) + ((cfg->csd[11] & 224) >> 5) + 1);
			}
			retv = true;
		}
	}

x:	sdspiRelease();
    return (retv);
}

