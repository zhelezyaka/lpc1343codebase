#include "lcd.h"

#include <string.h>

uint32_t frames;

/*************************************************/
void lcdInit(void)
{
  // Set control line pins to output
  gpioSetDir(LCD_CS_PORT, LCD_CS_PIN, 1);
  gpioSetDir(LCD_CD_PORT, LCD_CD_PIN, 1);
  gpioSetDir(LCD_WR_PORT, LCD_WR_PIN, 1);
  gpioSetDir(LCD_RD_PORT, LCD_RD_PIN, 1);
  
  // Set data port pins to output
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN1, 1);
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN2, 1);
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN3, 1);
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN4, 1);
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN5, 1);
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN6, 1);
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN7, 1);
  gpioSetDir(LCD_DATA_PORT, LCD_DATA_PIN8, 1);
    
  // Set backlight
  gpioSetDir(LCD_BL_PORT, LCD_BL_PIN, 1);      // set to output
  gpioSetValue(LCD_BL_PORT, LCD_BL_PIN, 0);    // turn on

  // Initialize the display
  lcdInitDisplay();

  frames = 0;

  lcdTest();
  lcdDrawString(10, 10, WHITE, "LINE 1 (3X6 SYSTEM)", Font_System3x6);  
  lcdDrawString(10, 20, WHITE, "Line 2 (5x8 SYSTEM)", Font_System5x8);  
  lcdDrawString(10, 30, WHITE, "Line 3 (7x8 SYSTEM)", Font_System7x8);  
  lcdDrawString(10, 50, BLACK, "Line 4 (8x8)", Font_8x8);  
  lcdDrawString(10, 60, BLACK, "Line 6 (8x8 Thin)", Font_8x8thin);  

  
//  while (1)
//  {
//    lcdTest();
//    frames++;
//    lcdTest2();
//    frames++;
//  }
}

/*************************************************/
void lcdTest(void)
{
  uint32_t i,j;
  lcdHome();
  
  for(i=0;i<320;i++)
  {
    for(j=0;j<240;j++)
    {
      if(i>279)lcdWriteData(WHITE);
      else if(i>239)lcdWriteData(BLUE);
      else if(i>199)lcdWriteData(GREEN);
      else if(i>159)lcdWriteData(CYAN);
      else if(i>119)lcdWriteData(RED);
      else if(i>79)lcdWriteData(MAGENTA);
      else if(i>39)lcdWriteData(YELLOW);
      else lcdWriteData(BLACK);
    }
  }
}

/*************************************************/
void lcdTest2(void)
{
  uint32_t i,j;
  lcdHome();
  
  for(i=0;i<320;i++)
  {
    for(j=0;j<240;j++)
    {
      if(i>279)lcdWriteData(YELLOW);
      else if(i>239)lcdWriteData(MAGENTA);
      else if(i>199)lcdWriteData(RED);
      else if(i>159)lcdWriteData(CYAN);
      else if(i>119)lcdWriteData(GREEN);
      else if(i>79)lcdWriteData(BLUE);
      else if(i>39)lcdWriteData(WHITE);
      else lcdWriteData(BLACK);
    }
  }
}

/*************************************************/
// Provide for convenience sake ... shouldn't be used
// in critical sections since it adds an extra
// branch, etc.
void lcdCommand(uint16_t command, uint16_t data)
{
  lcdWriteCmd(command);
  lcdWriteData(data);
}

/*************************************************/
void lcdWriteCmd(uint16_t command)
{
  CLR_CS_CD;      // Saves 7 commands compared to "CLR_CS; CLR_CD;"
  SET_RD_WR;      // Saves 7 commands compared to "SET_RD; SET_WR;"
  GPIO_GPIO2DATA &= ~LCD_DATA_MASK;             // clear
  GPIO_GPIO2DATA |= command >> LCD_DATA_OFFSET; // set
  CLR_WR;
  SET_WR;
  GPIO_GPIO2DATA &= ~LCD_DATA_MASK;             // clear
  GPIO_GPIO2DATA |= (command & LCD_DATA_MASK);  // set
  CLR_WR;
  SET_WR_CS;      // Saves 7 commands compared to "SET_WR; SET_CS;"
}

/*************************************************/
void lcdWriteData(uint16_t data)
{
  CLR_CS;
  SET_CD_RD_WR;   // Saves 14 commands compared to "SET_CD; SET_RD; SET_WR"
  GPIO_GPIO2DATA &= ~LCD_DATA_MASK;             // clear
  GPIO_GPIO2DATA |= data >> LCD_DATA_OFFSET; // set
  CLR_WR;
  SET_WR;
  GPIO_GPIO2DATA &= ~LCD_DATA_MASK;             // clear
  GPIO_GPIO2DATA |= (data & LCD_DATA_MASK);  // set
  CLR_WR;
  SET_WR_CS;      // Saves 7 commands compared to "SET_WR, SET_CS;"
}

/*************************************************/
void lcdDelay(unsigned int t)
{
  unsigned char t1;
  while(t--)
  for ( t1=10; t1 > 0; t1-- )
  {
    __asm("nop");
  }
}

/*************************************************/
void lcdInitDisplay(void)
{
  uint16_t i = 0x0000;

  // Clear data line
  GPIO_GPIO2DATA &= ~LCD_DATA_MASK;
    
  SET_RD;
  SET_WR;
  SET_CS;
  SET_CD;

  // Reset display
  CLR_RESET;
  lcdDelay(50);
  SET_RESET;
  lcdDelay(50);
  
  lcdCommand(0x0001, 0x0000);     // Driver Output Control Register (R01h)
  lcdCommand(0x0002, 0x0700);     // LCD Driving Waveform Control (R02h)
  
  // Entry Mode (R03h)
  lcdCommand(0x0003, 0x0000 | 1<<14			// DFM
                            | 1<<12			// BGR
                            | 0<<7			// ORG
                            | 1<<5			// ID/1
                            | 1<<4			// ID/0
                            | 0<<3 );			// AM
  
  lcdCommand(0x0004, 0x0000);     // Scaling Control register (R04h)  
  lcdCommand(0x0008, 0x0207);     // Display Control 2 (R08h)
  lcdCommand(0x0009, 0x0000);     // Display Control 3 (R09h)  
  lcdCommand(0x000A, 0x0000);     // Frame Cycle Control (R0Ah)
  lcdCommand(0x000C, 0x0000);     // External Display Interface Control 1 (R0Ch)
  lcdCommand(0x000D, 0x0000);     // Frame Maker Position (R0Dh)
  lcdCommand(0x000F, 0x0000);     // External Display Interface Control 2 (R0Fh)

  lcdCommand(0x0010, 0x0000);     // Power Control 1 (R10h)
  lcdCommand(0x0011, 0x0007);     // Power Control 2 (R11h)  
  lcdCommand(0x0012, 0x0000);     // Power Control 3 (R12h)
  lcdCommand(0x0013, 0x0000);     // Power Control 4 (R13h)
  lcdDelay(200);  

  lcdCommand(0x0007, 0x0101);     // Display Control (R07h)
  lcdCommand(0x0010, 0x12B0);     // Power Control 1 (R10h)  
  lcdCommand(0x0011, 0x0007);     // Power Control 2 (R11h)  
  lcdCommand(0x0012, 0x01BB);     // Power Control 3 (R12h)
  lcdDelay(50);  
  lcdCommand(0x0013, 0x1300);     // Power Control 4 (R13h)

  lcdCommand(0x0029, 0x0010);     // NVM read data 2 (R29h)
  lcdDelay(50);
  
  lcdCommand(0x0030, 0x000A);     // Gamma Control 1
  lcdCommand(0x0031, 0x1326);     // Gamma Control 2
  lcdCommand(0x0032, 0x0A29);     // Gamma Control 3
  lcdCommand(0x0033, 0x290A);     // Gamma Control 4
  lcdCommand(0x0034, 0x2613);     // Gamma Control 5
  lcdCommand(0x0035, 0x0A0A);     // Gamma Control 6
  lcdCommand(0x0036, 0x1E03);     // Gamma Control 7
  lcdCommand(0x0037, 0x031E);     // Gamma Control 8
  lcdCommand(0x0038, 0x0706);     // Gamma Control 9
  lcdCommand(0x0039, 0x0303) ;    // Gamma Control 10
  lcdCommand(0x003A, 0x0E04);     // Gamma Control 11
  lcdCommand(0x003B, 0x0E01);     // Gamma Control 12
  lcdCommand(0x003C, 0x010E);     // Gamma Control 13
  lcdCommand(0x003D, 0x040E);     // Gamma Control 14
  lcdCommand(0x003E, 0x0303);     // Gamma Control 15
  lcdCommand(0x003F, 0x0607);     // Gamma Control 16  

  lcdCommand(0x0050, 0x0000);     // Window Horizontal RAM Address Start (R50h)
  lcdCommand(0x0051, 0x00EF);     // Window Horizontal RAM Address End (R51h)
  lcdCommand(0x0052, 0X0000);     // Window Vertical RAM Address Start (R52h)
  lcdCommand(0x0053, 0x013F);     // Window Vertical RAM Address End (R53h)
  
  lcdCommand(0x0060, 0x2700);     // Driver Output Control (R60h)
  lcdCommand(0x0061, 0x0001);     // Driver Output Control (R61h)
  
  lcdCommand(0x006A, 0X0000);     // Vertical Scroll Control (R6Ah)
  
  lcdCommand(0x0080, 0X0000);     // Display Position - Partial Display 1 (R80h)
  lcdCommand(0x0081, 0X0000);     // RAM Address Start - Partial Display 1 (R81h)
  lcdCommand(0x0082, 0X0000);     // RAM Address End - Partial Display 1 (R82h)
  lcdCommand(0x0083, 0X0000);     // Display Position - Partial Display 2 (R83h)
  lcdCommand(0x0084, 0X0000);     // RAM Address Start - Partial Display 2 (R84h)
  lcdCommand(0x0085, 0X0000);     // RAM Address End - Partial Display 2 (R85h)
  
  lcdCommand(0x0090, 0X0010);     // Panel Interface Control 1 (R90h)
  lcdCommand(0x0092, 0X0000);     // Panel Interface Control 2 (R92h)
  lcdCommand(0x0093, 0X0103);     // Panel Interface control 3 (R93h)
  lcdCommand(0x0095, 0X0210);     // Panel Interface control 4 (R95h)
  lcdCommand(0x0097, 0X0000);     // Panel Interface Control 5 (R97h)
  lcdCommand(0x0098, 0X0000);     // Panel Interface Control 6 (R98h)

  // Display On
  lcdCommand(0x0007, 0x0173);     // Display Control (R07h)
}

/*************************************************/
void lcdHome(void)
{
  lcdCommand(0x0020, 0X0000);     // GRAM Address Set (Horizontal Address) (R20h)
  lcdCommand(0x0021, 0X0000);     // GRAM Address Set (Vertical Address) (R21h)
  lcdWriteCmd(0x0022);            // Write Data to GRAM (R22h)
}

/*************************************************/
void lcdSetWindow(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1)
{
  lcdCommand(0x0050, x-1);       // Window Horizontal RAM Address Start (R50h)
  lcdCommand(0x0051, x1-1);      // Window Horizontal RAM Address End (R51h)
  lcdCommand(0x0052, y-1);       // Window Vertical RAM Address Start (R52h) )
  lcdCommand(0x0053, y1-1);      // Window Vertical RAM Address End (R53h)
}

/*************************************************/
void lcdSetCursor(uint16_t x, uint16_t y)
{
  lcdCommand(0x0020, x-1);       // GRAM Address Set (Horizontal Address) (R20h)
  lcdCommand(0x0021, y-1);       // GRAM Address Set (Vertical Address) (R21h)
}

/*************************************************/
void lcdFillRGB(uint16_t data)
{
  unsigned int i;
  lcdHome();
  
  uint32_t pixels = 320*240;
  for ( i=0; i < pixels; i++ )
  {
    lcdWriteData(data);
  } 
}

/*************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  lcdWriteCmd(0x0020); // GRAM Address Set (Horizontal Address) (R20h)
  lcdWriteData(x-1);
  lcdWriteCmd(0x0021); // GRAM Address Set (Vertical Address) (R21h)
  lcdWriteData(y-1);
  lcdWriteCmd(0x0022);  // Write Data to GRAM (R22h)
  lcdWriteData(color);
}

/*************************************************/
void lcdDrawChar(uint16_t x, uint16_t y, uint16_t color, uint8_t c, struct FONT_DEF font)
{
  uint8_t col, column[font.u8Width];

  // Check if the requested character is available
  if ((c >= font.u8FirstChar) && (c <= font.u8LastChar))
  {
    // Retrieve appropriate columns from font data
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = font.au8FontTable[((c - 32) * font.u8Width) + col];    // Get first column of appropriate character
    }
  }
  else
  {    
    // Requested character is not available in this font ... send a space instead
    for (col = 0; col < font.u8Width; col++)
    {
      column[col] = 0xFF;    // Send solid space
    }
  }

  // Render each column
  uint16_t xoffset, yoffset;
  for (xoffset = 0; xoffset < font.u8Width; xoffset++)
  {
    for (yoffset = 0; yoffset < (font.u8Height + 1); yoffset++)
    {
      uint8_t bit = 0x00;
      bit = (column[xoffset] << (8 - (yoffset + 1)));     // Shift current row bit left
      bit = (bit >> 7);                     // Shift current row but right (results in 0x01 for black, and 0x00 for white)
      if (bit)
      {
        lcdDrawPixel(x + xoffset, y + yoffset, color);
      }
    }
  }
}

/*************************************************/
void lcdDrawString(uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font)
{
  for (U8 l = 0; l < strlen(text); l++)
  {
    lcdDrawChar(x + (l * (font.u8Width + 1)), y, color, text[l], font);
  }
}
