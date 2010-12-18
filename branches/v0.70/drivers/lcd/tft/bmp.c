/**************************************************************************/
/*! 
    @file     bmp.c
    @author   K. Townsend (microBuilder.eu)

    @brief    Loads uncomprssed 24-bit windows bitmaps images

    Based on the information available at:
    http://local.wasp.uwa.edu.au/~pbourke/dataformats/bmp/
    and some sample code written by Michael Sweet
	
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
#include <string.h>

#include "bmp.h"

#include "drivers/lcd/tft/drawing.h"
#include "drivers/lcd/tft/lcd.h"

#ifdef CFG_SDCARD
  #include "drivers/fatfs/diskio.h"
  #include "drivers/fatfs/ff.h"
  static FATFS Fatfs[1];

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/
bmp_error_t bmpParseBitmap(uint16_t x, uint16_t y, FIL file)
{
  UINT              bytesRead;
  bmp_header_t      header;
  bmp_infoheader_t  infoHeader;

  // Read the file header
  // ToDo: Optimise this to read buffer once and parse it
  f_read(&file, &header.type, sizeof(header.type), &bytesRead);
  f_read(&file, &header.size, sizeof(header.size), &bytesRead);
  f_read(&file, &header.reserved1, sizeof(header.reserved1), &bytesRead);
  f_read(&file, &header.reserved2, sizeof(header.reserved2), &bytesRead);
  f_read(&file, &header.offset, sizeof(header.offset), &bytesRead);

  // Make sure this is a bitmap (first two bytes = 'BM' or 0x4D42 on little-endian systems)
  if (header.type != 0x4D42) return BMP_ERROR_NOTABITMAP;

  // Read the info header
  // ToDo: Optimise this to read buffer once and parse it
  f_read(&file, &infoHeader.size, sizeof(infoHeader.size), &bytesRead);
  f_read(&file, &infoHeader.width, sizeof(infoHeader.width), &bytesRead);
  f_read(&file, &infoHeader.height, sizeof(infoHeader.height), &bytesRead);
  f_read(&file, &infoHeader.planes, sizeof(infoHeader.planes), &bytesRead);
  f_read(&file, &infoHeader.bits, sizeof(infoHeader.bits), &bytesRead);
  f_read(&file, &infoHeader.compression, sizeof(infoHeader.compression), &bytesRead);
  f_read(&file, &infoHeader.imagesize, sizeof(infoHeader.imagesize), &bytesRead);
  f_read(&file, &infoHeader.xresolution, sizeof(infoHeader.xresolution), &bytesRead);
  f_read(&file, &infoHeader.yresolution, sizeof(infoHeader.yresolution), &bytesRead);
  f_read(&file, &infoHeader.ncolours, sizeof(infoHeader.ncolours), &bytesRead);
  f_read(&file, &infoHeader.importantcolours, sizeof(infoHeader.importantcolours), &bytesRead);

  // Make sure that this is a 24-bit image
  if (infoHeader.bits != 24) 
    return BMP_ERROR_INVALIDBITDEPTH;

  // Check image dimensions
  if ((infoHeader.width > CFG_TFTLCD_WIDTH) | (infoHeader.height > CFG_TFTLCD_HEIGHT))
    return BMP_ERROR_INVALIDDIMENSIONS;

  // Make sure image is not compressed
  if (infoHeader.compression != BMP_COMPRESSION_NONE) 
    return BMP_ERROR_COMPRESSEDDATA;

  // Read 24-bit image data
  uint32_t px, py;
  FRESULT res;
  uint8_t buffer[infoHeader.width * 3];
  for (py = infoHeader.height; py > 0; py--)
  {
    // Read one row at a time
    res = f_read(&file, &buffer, infoHeader.width * 3, &bytesRead);
    if (res || bytesRead == 0)
    {
      // Error or EOF
      return BMP_ERROR_PREMATUREEOF;
    }
    for (px = 0; px < infoHeader.width; px++)
    {
      // Render pixel
      // ToDo: This is a brutally slow way of rendering bitmaps ...
      //        update to pass one row of data at a time
      drawPixel(x + px, y + py - 1, drawRGB24toRGB565(buffer[(px * 3) + 2], buffer[(px * 3) + 1], buffer[(px * 3)]));
    }
  }

  return BMP_ERROR_NONE;
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Loads a 24-bit Windows bitmap image from an SD card and
            renders it

    @section Example

    @code 

    #include "drivers/lcd/tft/bmp.h"

    bmp_error_t error;

    // Draw image.bmp (from the root folder) starting at pixel 0,0
    error = bmpDrawBitmap(0, 0, "/image.bmp");

    // Check 'error' for problems such as BMP_ERROR_FILENOTFOUND

    @endcode
*/
/**************************************************************************/
bmp_error_t bmpDrawBitmap(uint16_t x, uint16_t y, const char* filename)
{
  bmp_error_t error = BMP_ERROR_NONE;
  DSTATUS stat;
  BYTE res;

  stat = disk_initialize(0);

  if ((stat & STA_NOINIT) || (stat & STA_NODISK))
  {
    // Card not initialised or no disk present
    return BMP_ERROR_SDINITFAIL;
  }

  if (stat == 0)
  {
    // Try to mount drive
    res = f_mount(0, &Fatfs[0]);
    if (res != FR_OK) 
    {
      // Failed to mount 0:
      return BMP_ERROR_SDINITFAIL;
    }
    if (res == FR_OK)
    {
      // Try to open the requested file
      FIL imgfile;  
      if(f_open(&imgfile, filename, FA_READ | FA_OPEN_EXISTING) != FR_OK) 
      {  
        // Unable to open the requested file
        return BMP_ERROR_FILENOTFOUND;
      }
      // Try to render the specified image
      error = bmpParseBitmap(x, y, imgfile);
      // Close file
      f_close(&imgfile);
      // Unmount drive
      f_mount(0,0);
       // Return results
      return error;
    }
  }

  // Return OK signal
  return BMP_ERROR_NONE;
}

#endif
