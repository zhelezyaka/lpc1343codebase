#ifndef __LCD__
#define __LCD__

#include "projectconfig.h"
#include "core/gpio/gpio.h"
#include "fonts.h"

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

// These pin definitions are for optimisation purposes only
// If the pins values above are modified the bit equivalents
// below will also need to be updated
#define LCD_CS_CD_PINS      0x300   // 8 + 9
#define LCD_RD_WR_PINS      0xC00   // 11 + 10
#define LCD_WR_CS_PINS      0x500   // 10 + 8
#define LCD_CD_RD_WR_PINS   0xE00   // 9 + 11 + 10

// Data pins
// Note: data pins must be consecutive and on the same port
#define LCD_DATA_PORT   2     // 8-Pin Data Port
#define LCD_DATA_PIN1   1
#define LCD_DATA_PIN2   2
#define LCD_DATA_PIN3   3
#define LCD_DATA_PIN4   4
#define LCD_DATA_PIN5   5
#define LCD_DATA_PIN6   6
#define LCD_DATA_PIN7   7
#define LCD_DATA_PIN8   8
#define LCD_DATA_MASK   0x000001FE
#define LCD_DATA_OFFSET 1     // Offset = PIN1

// For single operation bit clear/set (see 1343 UM 8.5.1)
#define LCD_GPIO2DATA_DATA      (*(pREG32 (GPIO_GPIO2_BASE + (LCD_DATA_MASK << 2))))
#define LCD_GPIO1DATA_WR        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << LCD_WR_PIN) << 2))))

#define LCD_GPIO1DATA_CD        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << LCD_CD_PIN) << 2))))
#define LCD_GPIO1DATA_CS        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << LCD_CS_PIN) << 2))))
#define LCD_GPIO1DATA_WR        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << LCD_WR_PIN) << 2))))
#define LCD_GPIO1DATA_RD        (*(pREG32 (GPIO_GPIO1_BASE + ((1 << LCD_RD_PIN) << 2))))
#define LCD_GPIO3DATA_RES       (*(pREG32 (GPIO_GPIO3_BASE + ((1 << LCD_RES_PIN) << 2))))
#define LCD_GPIO1DATA_CS_CD     (*(pREG32 (GPIO_GPIO1_BASE + ((LCD_CS_CD_PINS) << 2))))
#define LCD_GPIO1DATA_RD_WR     (*(pREG32 (GPIO_GPIO1_BASE + ((LCD_RD_WR_PINS) << 2))))
#define LCD_GPIO1DATA_WR_CS     (*(pREG32 (GPIO_GPIO1_BASE + ((LCD_WR_CS_PINS) << 2))))
#define LCD_GPIO1DATA_CD_RD_WR  (*(pREG32 (GPIO_GPIO1_BASE + ((LCD_CD_RD_WR_PINS) << 2))))

// Macros for control lines
#define CLR_CD          LCD_GPIO1DATA_CD = (0)
#define SET_CD          LCD_GPIO1DATA_CD = (1 << LCD_CD_PIN)
#define CLR_CS          LCD_GPIO1DATA_CS = (0)
#define SET_CS          LCD_GPIO1DATA_CS = (1 << LCD_CS_PIN)
#define CLR_WR          LCD_GPIO1DATA_WR = (0)
#define SET_WR          LCD_GPIO1DATA_WR = (1 << LCD_WR_PIN)
#define CLR_RD          LCD_GPIO1DATA_RD = (0)
#define SET_RD          LCD_GPIO1DATA_RD = (1 << LCD_RD_PIN)
#define CLR_RESET       LCD_GPIO3DATA_RES = (0)
#define SET_RESET       LCD_GPIO3DATA_RES = (1 << LCD_RES_PIN)

#define CLR_CS_CD       LCD_GPIO1DATA_CS_CD = (0);
#define SET_RD_WR       LCD_GPIO1DATA_RD_WR = (LCD_RD_WR_PINS);
#define SET_WR_CS       LCD_GPIO1DATA_WR_CS = (LCD_WR_CS_PINS);
#define SET_CD_RD_WR    LCD_GPIO1DATA_CD_RD_WR = (LCD_CD_RD_WR_PINS);

// The following macros are defined to improve code optimization by
// reducing the number of instructions in heavily used functions
//#define CLR_CS_CD       GPIO_GPIO1DATA &= ~((1 << LCD_CD_PIN) | (1 << LCD_CS_PIN)); 
//#define SET_RD_WR       GPIO_GPIO1DATA |= ((1 << LCD_RD_PIN) | (1 << LCD_WR_PIN)); 
//#define SET_WR_CS       GPIO_GPIO1DATA |= ((1 << LCD_WR_PIN) | (1 << LCD_CS_PIN)); 
//#define SET_CD_RD_WR    GPIO_GPIO1DATA |= ((1 << LCD_CD_PIN) | (1 << LCD_RD_PIN) | (1 << LCD_WR_PIN)); 

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
