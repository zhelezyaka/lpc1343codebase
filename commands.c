/**************************************************************************/
/*! 
    @file     commands.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @brief    Entry point for all commands in the 'core/cmd' command-line
              interpretter.  Every menu item defined in cmd_tbl.h points
              to a method that should be located here for convenience
              sake.  (The only exception is the 'help', which exists in
              any project and is handled directly by core/cmd/cmd.c). All
              methods have exactly the same signature (argc + argv).

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

#include <stdio.h>
#include <string.h>   // memset
#include <stdlib.h>   // strtol
#include <ctype.h>    // isdigit, isspace, etc.

#include "core/cmd/cmd.h"
#include "core/gpio/gpio.h"

#ifdef CFG_CHIBI
  #include "drivers/chibi/chb.h"
  #include "drivers/chibi/chb_drvr.h"
#endif

/**************************************************************************/
/*! 
    Determines whether the supplied string consists solely of decimal
    characters (0-9)

    @param[in]  s
                Input string

    @section Example

    @code
    char *test = "1234567890";

    // Check if the value contains only decimal characters
    if (isDecimalString(test))
    {
      // String contains only decimal characters
      ...
    }
    else
    {
      // Supplied string contains non-decimal characters
    }

    @endcode
*/
/**************************************************************************/
static int isDecimalString (char *s)
{
  while (*s)
    if (!isdigit (*s++))
      return 0;

  return 1;
}

/**************************************************************************/
/*!
    @brief  Attempts to convert the supplied decimal or hexadecimal
          string to the matching 32-bit value.  All hexadecimal values
          must be preceded by either '0x' or '0X' to be properly parsed.

    @param[in]  s
                Input string
    @param[out] result
                Signed 32-bit integer to hold the conversion results

    @section Example

    @code
    char *hex = "0xABCD";
    char *dec = "1234";

    // Convert supplied values to integers
    int32_t hexValue, decValue;
    getNumber (hex, &hexValue);
    getNumber (dec, &decValue);

    @endcode
*/
/**************************************************************************/
static int getNumber (char *s, int32_t *result)
{
  int32_t value;
  uint32_t mustBeHex = FALSE;
  uint32_t sgn = 1;
  const unsigned char hexToDec [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255, 255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15};

  if (!s)
    return 0;

  // Check if this is a hexadecimal value
  if ((strlen (s) > 2) && (!strncmp (s, "0x", 2) || !strncmp (s, "0X", 2)))
  {
    mustBeHex = TRUE;
    s += 2;
  }

  // Check for negative sign
  if (!mustBeHex && *s && (*s == '-'))
  {
    sgn = -1;
    s++;
  }

  // Try to convert value
  for (value = 0; *s; s++)
  {
    if (mustBeHex && isxdigit (*s))
      value = (value << 4) | hexToDec [toupper (*s) - '0'];
    else if (isdigit (*s))
      value = (value * 10) + (*s - '0');
    else
    {
      printf ("Malformed number. Must be decimal number, or hex value preceeded by '0x'%s", CFG_INTERFACE_NEWLINE);
      return 0;
    }
  }

  // Set number to negative value if required
  if (!mustBeHex)
    value *= sgn;

  *result = value;

  return 1;
}

/**************************************************************************/
/*! 
    'hello' command handler
*/
/**************************************************************************/
void cmd_hello(uint8_t argc, char **argv)
{
  if (argc > 0)
  {
    printf("Hello %s%s", argv[0],CFG_INTERFACE_NEWLINE);
  }
  else
  {
    printf("Hello World!%s", CFG_INTERFACE_NEWLINE);
  }
}

/**************************************************************************/
/*! 
    'sysinfo' command handler
*/
/**************************************************************************/
void cmd_sysinfo(uint8_t argc, char **argv)
{
  printf("%-30s : %d Hz %s", "Core System Clock", CFG_CPU_CCLK, CFG_INTERFACE_NEWLINE);
  printf("%-30s : %d mS %s", "Systick Timer Delay", CFG_SYSTICK_DELAY_IN_MS, CFG_INTERFACE_NEWLINE);
  printf("%-30s : %d BPS %s", "UART Baud Rate", CFG_UART_BAUDRATE, CFG_INTERFACE_NEWLINE);

  #ifdef CFG_CHIBI
    chb_pcb_t *pcb = chb_get_pcb();
    printf("%-30s : %s%s", "Wireless Frequency", "868 MHz", CFG_INTERFACE_NEWLINE);
    printf("%-30s : %04X%s", "Wireless Node Address", pcb->src_addr, CFG_INTERFACE_NEWLINE);
  #endif

  // printf("%-30s : %s", "<Property Name>", CFG_INTERFACE_NEWLINE);
}

#ifdef CFG_CHIBI

/**************************************************************************/
/*! 
    Gets or sets the 16-bit sensor node address.  This value can be 
    anything between 1-65534, and in decimal or hexadecimal notation.
    All hexadecimal values must be preceded by '0x' or '0X' and use all
    four hexadecimal characters (ex. 0x0001) to be properly interpreted.
*/
/**************************************************************************/
void cmd_chibi_shortaddr(uint8_t argc, char **argv)
{
  if (argc > 0)
  {
    // Check for decimal input
    if (isDecimalString(argv[1]))
    {
      // Make sure decimal value is 16-bit or less
      uint32_t test = strtol(argv[1], NULL, 32);
      if (test >= 0xFFFF || test == 0)
      {
        printf("Invalid Address: Valid 16-bit value required (1-65534).%s", CFG_INTERFACE_NEWLINE);
        return;
      }
    }

    // Make sure hexadecimal values are 16-bit or less
    if ((strlen (argv[1]) > 2) && (!strncmp (argv[1], "0x", 2) || !strncmp (argv[1], "0X", 2)) && (strlen (argv[1]) != 6))
    {
      printf("Invalid Address: Full 16-bit hexadecimal value required (ex. '0x12EF').%s", CFG_INTERFACE_NEWLINE);
      return;
    }

    // Try to convert supplied value to an integer
    int32_t addr;
    getNumber (argv[1], &addr);
    
    // Check for invalid values (getNumber may complain about this as well)
    if (addr <= 0 || addr > 0xFFFF)
    {
      printf("Invalid Address: Value from 1-65534 or 0x0001-0xFFFE required.%s", CFG_INTERFACE_NEWLINE);
      return;
    }
    if (addr == 0xFFFF)
    {
      printf("Invalid Address: 0xFFFF is reserved for global transmissions.%s", CFG_INTERFACE_NEWLINE);
      return;
    }

    // Update address
    chb_set_short_addr((uint16_t)addr);
    chb_pcb_t *pcb = chb_get_pcb();
    printf("Address set to: 0x%4X%s", pcb->src_addr, CFG_INTERFACE_NEWLINE);
  }
  else
  {
    chb_pcb_t *pcb = chb_get_pcb();
    printf("0x%4X%s", pcb->src_addr, CFG_INTERFACE_NEWLINE);
  }
}

void cmd_chibi_ieeeaddr(uint8_t argc, char **argv)
{
  // ToDo: Validate input!
  uint8_t  i, addr[8];

  if (argc > 0)
  {
    memset(addr, 0, 8);
    for (i=0; i<argc-1; i++)
    {
        addr[i] = strtol(argv[i+1], NULL, 16);
    }
    chb_set_ieee_addr(addr);
    printf ("IEEE address set to: 0x");
    for (i=8; i>0; i--)
    {
      printf("%02X", addr[i-1]);
    }
    printf("%s", CFG_INTERFACE_NEWLINE);
  }
  else
  {
    chb_get_ieee_addr(addr);
    
    printf("0x");
    for (i=8; i>0; i--)
    {
      printf("%02X", addr[i-1]);
    }
    printf("%s", CFG_INTERFACE_NEWLINE);
  }
}

void cmd_chibi_tx(uint8_t argc, char **argv)
{
  // ToDo: Validate input!
  uint8_t i, len, *data_ptr, data[50];
  uint16_t addr;

  addr = strtol(argv[1], NULL, 16);

  data_ptr = data;
  for (i=0; i<argc-2; i++)
  {
    len = strlen(argv[i+2]);
    strcpy((char *)data_ptr, (char *)argv[i+2]);
    data_ptr += len;
    *data_ptr++ = ' ';
  }
  *data_ptr++ = '\0';

  chb_write(addr, data, data_ptr - data);
}

void cmd_chibi_mode(uint8_t argc, char **argv)
{
  if (argc > 0)
  {
    // ToDo: Validate input!
    uint8_t mode = strtol(argv[1], NULL, 10);
  
    chb_set_state(TRX_OFF);
  
    chb_set_mode(mode);
    chb_set_channel(1);
  
    chb_set_state(RX_AACK_ON);
  }
  else
  {
    printf("ToDo%s", CFG_INTERFACE_NEWLINE);
  }
}

void cmd_chibi_power(uint8_t argc, char **argv)
{
  if (argc > 0)
  {
    // ToDo: Validate input!
    uint8_t pwr = strtol(argv[1], NULL, 10);
    uint8_t val;
  
    switch (pwr)
    {
      case 10: val = 0xc0; break;
      case 9: val = 0xa1; break;
      case 8: val = 0x81; break;
      case 7: val = 0x82; break;
      case 6: val = 0x83; break;
      case 5: val = 0x60; break;
      case 4: val = 0x61; break;
      case 3: val = 0x41; break;
      case 2: val = 0x42; break;
      case 1: val = 0x22; break;
      case 0: val = 0x23; break;
      default: return;
    }
  
    chb_set_pwr(val);
  }
  else
  {
    printf("ToDo%s", CFG_INTERFACE_NEWLINE);
  }
}

#endif

#ifdef CFG_I2CEEPROM

void cmd_i2ceeprom_read(uint8_t argc, char **argv)
{
}

void cmd_i2ceeprom_write(uint8_t argc, char **argv)
{
}

#endif

#ifdef CFG_LM75B

void cmd_lm75b_gettemp(uint8_t argc, char **argv)
{
}

#endif
