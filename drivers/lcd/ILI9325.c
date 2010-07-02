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

//  lcdTest();
//  lcdDrawString(10, 10, WHITE, "LINE 1 (3X6 SYSTEM)", Font_System3x6);  
//  lcdDrawString(10, 20, WHITE, "Line 2 (5x8 SYSTEM)", Font_System5x8);  
//  lcdDrawString(10, 30, WHITE, "Line 3 (7x8 SYSTEM)", Font_System7x8);  
//  lcdDrawString(10, 50, BLACK, "Line 4 (8x8)", Font_8x8);  
//  lcdDrawString(10, 60, BLACK, "Line 6 (8x8 Thin)", Font_8x8thin);  

  while (1)
  {
    lcdTest();
    // lcdDrawString(200, 310, BLACK, "TEST", Font_System7x8);
    frames++;
    lcdTest2();
    frames++;
  }
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
      else if(i>39)lcdWriteData(BLACK);
      else lcdWriteData(WHITE);
    }
  }
}

/*************************************************/
// Provided for convenience sake ... shouldn't be used
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
  LCD_GPIO2DATA_DATA = (command >> (8 - LCD_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  LCD_GPIO2DATA_DATA = command << LCD_DATA_OFFSET;
  CLR_WR;
  SET_WR_CS;      // Saves 7 commands compared to "SET_WR; SET_CS;"
}

/*************************************************/
void lcdWriteData(uint16_t data)
{
  CLR_CS;
  SET_CD_RD_WR;   // Saves 14 commands compared to "SET_CD; SET_RD; SET_WR"
  LCD_GPIO2DATA_DATA = (data >> (8 - LCD_DATA_OFFSET));
  CLR_WR;
  SET_WR;
  LCD_GPIO2DATA_DATA = data << LCD_DATA_OFFSET;
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
  lcdDelay(10000);
  SET_RESET;
  lcdDelay(500);

  lcdCommand(0x00FF, 0x0001);
  lcdCommand(0x00F3, 0x0008);
  lcdWriteCmd(0x00F3);

  lcdCommand(0x0001, 0x0100);     // Driver Output Control Register (R01h)
  lcdCommand(0x0002, 0x0700);     // LCD Driving Waveform Control (R02h)
  lcdCommand(0x0003, 0x1030);     // Entry Mode (R03h)  
  lcdCommand(0x0008, 0x0302);
  lcdCommand(0x0009, 0x0000);
  lcdCommand(0x0010, 0x0000);     // Power Control 1 (R10h)
  lcdCommand(0x0011, 0x0007);     // Power Control 2 (R11h)  
  lcdCommand(0x0012, 0x0000);     // Power Control 3 (R12h)
  lcdCommand(0x0013, 0x0000);     // Power Control 4 (R13h)
  lcdDelay(1000);  
  lcdCommand(0x0010, 0x14B0);     // Power Control 1 (R10h)  
  lcdDelay(500);  
  lcdCommand(0x0011, 0x0007);     // Power Control 2 (R11h)  
  lcdDelay(500);  
  lcdCommand(0x0012, 0x008E);     // Power Control 3 (R12h)
  lcdCommand(0x0013, 0x0C00);     // Power Control 4 (R13h)
  lcdCommand(0x0029, 0x0015);     // NVM read data 2 (R29h)
  lcdDelay(500);
  lcdCommand(0x0030, 0x0000);     // Gamma Control 1
  lcdCommand(0x0031, 0x0107);     // Gamma Control 2
  lcdCommand(0x0032, 0x0000);     // Gamma Control 3
  lcdCommand(0x0035, 0x0203);     // Gamma Control 6
  lcdCommand(0x0036, 0x0402);     // Gamma Control 7
  lcdCommand(0x0037, 0x0000);     // Gamma Control 8
  lcdCommand(0x0038, 0x0207);     // Gamma Control 9
  lcdCommand(0x0039, 0x0000);     // Gamma Control 10
  lcdCommand(0x003C, 0x0203);     // Gamma Control 13
  lcdCommand(0x003D, 0x0403);     // Gamma Control 14
  lcdCommand(0x0050, 0x0000);     // Window Horizontal RAM Address Start (R50h)
  lcdCommand(0x0051, 0x00EF);     // Window Horizontal RAM Address End (R51h)
  lcdCommand(0x0052, 0X0000);     // Window Vertical RAM Address Start (R52h)
  lcdCommand(0x0053, 0x013F);     // Window Vertical RAM Address End (R53h)
  lcdCommand(0x0060, 0xa700);     // Driver Output Control (R60h)
  lcdCommand(0x0061, 0x0001);     // Driver Output Control (R61h)
  lcdCommand(0x0090, 0X0029);     // Panel Interface Control 1 (R90h)

  // Display On
  lcdCommand(0x0007, 0x0133);     // Display Control (R07h)
  lcdDelay(500);
  lcdWriteCmd(0x0022);
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
