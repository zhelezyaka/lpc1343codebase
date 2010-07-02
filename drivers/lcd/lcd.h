#ifndef __LCD__
#define __LCD__

#include "projectconfig.h"
#include "core/gpio/gpio.h"
#include "fonts.h"

#define GPIO2DATA_MASKED                       (*(pREG32 (0x50020000 + 0x3c0)))

// Control pins
#define LCD_CS_PORT     1     // CS (LCD Pin 7)
#define LCD_CS_PIN      8
#define LCD_CD_PORT     1     // CS/RS (LCD Pin 8)
#define LCD_CD_PIN      9
#define LCD_WR_PORT     1     // WR (LCD Pin 9)
#define LCD_WR_PIN      10
#define LCD_RD_PORT     1     // RD (LCD Pin 10)
#define LCD_RD_PIN      11
#define LCD_RES_PORT    3     // LCD Reset  (LCD Pin 31)
#define LCD_RES_PIN     3
#define LCD_BL_PORT     0     // Backlight Enable (LCD Pin 16)
#define LCD_BL_PIN      8

// Data pins
// Note: data pins must be consecutive and on the same port
#define LCD_DATA_PORT   2     // 8-Pin Data Port
#define LCD_DATA_PIN1   0
#define LCD_DATA_PIN2   1
#define LCD_DATA_PIN3   2
#define LCD_DATA_PIN4   3
#define LCD_DATA_PIN5   4
#define LCD_DATA_PIN6   5
#define LCD_DATA_PIN7   6
#define LCD_DATA_PIN8   7
#define LCD_DATA_MASK   0x000000FF
#define LCD_DATA_OFFSET 8     // PIN1 + 8

// Macros for control lines
#define CLR_CD          GPIO_GPIO1DATA &= ~(1 << LCD_CD_PIN); 
#define SET_CD          GPIO_GPIO1DATA |= (1 << LCD_CD_PIN); 
#define CLR_CS          GPIO_GPIO1DATA &= ~(1 << LCD_CS_PIN); 
#define SET_CS          GPIO_GPIO1DATA |= (1 << LCD_CS_PIN); 
#define CLR_RESET       GPIO_GPIO1DATA &= ~(1 << LCD_RES_PIN);  
#define SET_RESET       GPIO_GPIO1DATA |= (1 << LCD_RES_PIN); 
#define CLR_WR          GPIO_GPIO1DATA &= ~(1 << LCD_WR_PIN); 
#define SET_WR          GPIO_GPIO1DATA |= (1 << LCD_WR_PIN); 
#define CLR_RD          GPIO_GPIO1DATA &= ~(1 << LCD_RD_PIN); 
#define SET_RD          GPIO_GPIO1DATA |= (1 << LCD_RD_PIN); 

// The following macros are defined to improve code optimization by
// reducing the number of instructions in heavily used functions
#define CLR_CS_CD       GPIO_GPIO1DATA &= ~((1 << LCD_CD_PIN) | (1 << LCD_CS_PIN)); 
#define SET_RD_WR       GPIO_GPIO1DATA |= ((1 << LCD_RD_PIN) | (1 << LCD_WR_PIN)); 
#define SET_WR_CS       GPIO_GPIO1DATA |= ((1 << LCD_WR_PIN) | (1 << LCD_CS_PIN)); 
#define SET_CD_RD_WR    GPIO_GPIO1DATA |= ((1 << LCD_CD_PIN) | (1 << LCD_RD_PIN) | (1 << LCD_WR_PIN)); 

// Color definitions
#define	BLACK		0x0000
#define	BLUE		0x001F
#define	RED		0xF800
#define	GREEN		0x07E0
#define CYAN		0x07FF
#define MAGENTA		0xF81F
#define YELLOW		0xFFE0
#define WHITE		0xFFFF

// Method prototypes
void lcdInit(void);
void lcdTest(void);
void lcdTest2(void);
void lcdCommand(uint16_t command, uint16_t data);
void lcdWriteCmd(uint16_t command);
void lcdWriteData(uint16_t data);
void lcdDelay(unsigned int t);
void lcdInitDisplay(void);
void lcdHome(void);
void lcdSetWindow(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1);
void lcdSetCursor(uint16_t x, uint16_t y);
void lcdFillRGB(uint16_t data);
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);
void lcdDrawChar(uint16_t x, uint16_t y, uint16_t color, uint8_t c, struct FONT_DEF font);
void lcdDrawString(uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font);

#endif
