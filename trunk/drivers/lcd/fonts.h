#ifndef FONTS_H_
#define FONTS_H_

/* Partially based on original code for the KS0108 by Stephane Rey */
/* Current version by Kevin Townsend */
/* Last Updated: 12 May 2009 */

#include "sysdefs.h"

struct FONT_DEF 
{
    uint8_t u8Width;     	/* Character width for storage         */
    uint8_t u8Height;  	/* Character height for storage        */
    uint8_t u8FirstChar;     /* The first character available       */
    uint8_t u8LastChar;      /* The last character available        */
    uint8_t *au8FontTable;   /* Font table start address in memory  */
};

extern const struct FONT_DEF Font_System3x6;
extern const struct FONT_DEF Font_System5x8;
extern const struct FONT_DEF Font_System7x8;
extern const struct FONT_DEF Font_8x8;
extern const struct FONT_DEF Font_8x8thin;

extern const uint8_t au8FontSystem3x6[];
extern const uint8_t au8FontSystem5x8[];
extern const uint8_t au8FontSystem7x8[];
extern const uint8_t au8Font8x8[];
extern const uint8_t au8Font8x8thin[];

#endif
