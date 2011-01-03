/**************************************************************************/
/*! 
    @file     cmd_tbl.h
    @author   K. Townsend (microBuilder.eu)

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

#ifndef __CMD_TBL_H__ 
#define __CMD_TBL_H__

#define CMD_COUNT (sizeof(cmd_tbl)/sizeof(cmd_t))

#include <stdio.h>

#ifdef CFG_INTERFACE_UART
#include "core/uart/uart.h"
#endif

// Function prototypes for the command table
void cmd_help(uint8_t argc, char **argv);         // handled by core/cmd/cmd.c
void cmd_hello(uint8_t argc, char **argv);
void cmd_sysinfo(uint8_t argc, char **argv);

#ifdef CFG_TFTLCD
void cmd_button(uint8_t argc, char **argv);
void cmd_circle(uint8_t argc, char **argv);
void cmd_clear(uint8_t argc, char **argv);
void cmd_clear24(uint8_t argc, char **argv);
void cmd_line(uint8_t argc, char **argv);
void cmd_pixel(uint8_t argc, char **argv);
void cmd_progress(uint8_t argc, char **argv);
void cmd_getpixel(uint8_t argc, char **argv);
void cmd_gettext(uint8_t argc, char **argv);
void cmd_calibrate(uint8_t argc, char **argv);
void cmd_text(uint8_t argc, char **argv);
void cmd_textw(uint8_t argc, char **argv);
#ifdef CFG_SDCARD
void cmd_bmp(uint8_t argc, char **argv);
#endif
#endif

#ifdef CFG_CHIBI
void cmd_chibi_addr(uint8_t argc, char **argv);
void cmd_chibi_ieeeaddr(uint8_t argc, char **argv);
void cmd_chibi_tx(uint8_t argc, char **argv);
#endif

#ifdef CFG_I2CEEPROM
void cmd_i2ceeprom_read(uint8_t argc, char **argv);
void cmd_i2ceeprom_write(uint8_t argc, char **argv);
#endif

#ifdef CFG_LM75B
void cmd_lm75b_gettemp(uint8_t argc, char **argv);
#endif

#ifdef CFG_SDCARD
void cmd_sd_dir(uint8_t argc, char **argv);
#endif

void cmd_deepsleep(uint8_t argc, char **argv);

/**************************************************************************/
/*! 
    Command list for the command-line interpreter and the name of the
    corresponding method that handles the command.

    Note that a trailing ',' is required on the last entry, which will
    cause a NULL entry to be appended to the end of the table.
*/
/**************************************************************************/
cmd_t cmd_tbl[] = 
{
  // command name, min args, max args, hidden, function name, command description, syntax description
  { "help",           0,  0,  0, cmd_help              , "Displays a list of all available commands"           , "'help' has no parameters" },
  { "hello",          0,  1,  0, cmd_hello             , "Displays \'Hello World!\'"                           , "'hello [<name>]'" },
  { "sysinfo",        0,  0,  0, cmd_sysinfo           , "Displays current system configuration settings"      , "'sysinfo' has no parameters" },

  #ifdef CFG_TFTLCD
  { "btn",            5,  99, 0, cmd_button            , "Draws a button"                                      , "'btn <x> <y> <w> <h> <enabled> [<text>]'" },
  { "calibrate",      0,  0,  0, cmd_calibrate         , "Calibrates the touch screen"                         , "'calibrate' has no parameters'" },
  { "circle",         4,  4,  0, cmd_circle            , "Draws a circle"                                      , "'circle <x> <y> <radius> <color>'" },
  { "clr",            0,  1,  0, cmd_clear             , "Fills the screen with a 16-bit (RGB565) color"       , "'clr [<color>]'" },
  { "clr24",          3,  3,  0, cmd_clear24           , "Fills the screen with a 24-bit (RGB) color"          , "'clr24 <r> <g> <b>'" },
  { "gettext",        0,  0,  0, cmd_gettext           , "Displays an alpha-numeric input dialogue"            , "'gettext' has no parameters" },
  { "gp",             2,  2,  0, cmd_getpixel          , "Reads a single pixel from the LCD"                   , "'gp <x> <y>'" },
  { "line",           5,  5,  0, cmd_line              , "Draws a line"                                        , "'line <x1> <y1> <x2> <y2> <color>'" },
  { "p",              3,  3,  0, cmd_pixel             , "Draws a single pixel"                                , "'p <x> <y> <color>'" },
  { "progress",       7,  7,  0, cmd_progress          , "Draws a progress bar"                                , "'progress <x> <y> <w> <h> <percent> <bordercolor> <fillcolor>'" },
  { "text",           5, 99,  0, cmd_text              , "Renders text on the LCD"                             , "'text <x> <y> <color> <fontnumber> <message>'" },
  { "textw",          2, 99,  0, cmd_textw             , "Gets the width in pixels of the supplied text"       , "'textw <fontnumber> <message>'" },
  #ifdef CFG_SDCARD
  { "bmp",            3,  3,  0, cmd_bmp               , "Loads a bitmap image from the SD card"               , "'bmp <x> <y> <filename>'" },
  #endif
  #endif

  #ifdef CFG_CHIBI
  { "chb-addr",       0,  1,  0, cmd_chibi_addr        , "Gets/sets the 16-bit node address"                   , "'chb-addr [<1-65534>|<OxFFFE>]'" },
  { "chb-send",       2, 99,  0, cmd_chibi_tx          , "Transmits the supplied text/value"                   , "'chb-send <destaddr> <message>'" },
  #endif

  #ifdef CFG_I2CEEPROM
  { "eeprom-read",    1,  1,  0, cmd_i2ceeprom_read    , "Reads one byte from EEPROM"                          , "'eeprom-read <addr>'" },
  { "eeprom-write",   2,  2,  0, cmd_i2ceeprom_write   , "Writes one byte to EEPROM"                           , "'eeprom-write <addr> <value>'" },
  #endif

  #ifdef CFG_LM75B
  { "lm75b-read",     0,  0,  0, cmd_lm75b_gettemp     , "Gets the temp. in degrees celsius"                   , "'lm75b-read' has no parameters" },
  #endif

  #ifdef CFG_SDCARD
  { "sd-dir",         0,  1,  0,  cmd_sd_dir           , "List all files in the specified directory"           , "'sd-dir [<path>]'" },
  #endif

  { "sleep",          0,  0,  0,  cmd_deepsleep        , "Put the device into deep sleep for ~10 seconds"      , "'sleep' has no parameters" },
};

#endif