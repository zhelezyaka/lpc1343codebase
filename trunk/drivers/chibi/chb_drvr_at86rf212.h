/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
#ifndef CHIBI_DRVR_H
#define CHIBI_DRVR_H

#include "lpc134x.h"
#include "sysdefs.h"
#include "core/gpio/gpio.h"

#define CHB_CHINA                   0
#define CHB_EEPROM_IEEE_ADDR        0x00
#define CHB_EEPROM_SHORT_ADDR       0x09
#define CHB_AT86RF212_VER_NUM       0x01
#define CHB_AT86RF212_PART_NUM      0x07

#define CHB_SPI_CMD_RW              0xC0  /**<  Register Write (short mode). */
#define CHB_SPI_CMD_RR              0x80  /**<  Register Read (short mode). */
#define CHB_SPI_CMD_FW              0x60  /**<  Frame Transmit Mode (long mode). */
#define CHB_SPI_CMD_FR              0x20  /**<  Frame Receive Mode (long mode). */
#define CHB_SPI_CMD_SW              0x40  /**<  SRAM Write. */
#define CHB_SPI_CMD_SR              0x00  /**<  SRAM Read. */
#define CHB_SPI_CMD_RADDRM          0x7F  /**<  Register Address Mask. */

#define CHB_IRQ_BAT_LOW_MASK        0x80  /**< Mask for the BAT_LOW interrupt. */
#define CHB_IRQ_TRX_UR_MASK         0x40  /**< Mask for the TRX_UR interrupt. */
#define CHB_IRQ_TRX_END_MASK        0x08  /**< Mask for the TRX_END interrupt. */
#define CHB_IRQ_RX_START_MASK       0x04  /**< Mask for the RX_START interrupt. */
#define CHB_IRQ_PLL_UNLOCK_MASK     0x02  /**< Mask for the PLL_UNLOCK interrupt. */
#define CHB_IRQ_PLL_LOCK_MASK       0x01  /**< Mask for the PLL_LOCK interrupt. */

#define CHB_EINTPORT                (1)
#define CHB_EINTPIN                 (8)             /** Interrupt on GPIO1.8 **/
#define CHB_EINTPIN_IOCONREG        (IOCON_PIO1_8)  /** Required to disable internal pull-up/down resistor **/
#define CHB_RSTPORT                 (2)
#define CHB_RSTPIN                  (9)             /** Sleep Pin **/
#define CHB_RSTPIN_IOCONREG         (IOCON_PIO2_9)  /** Required to disable internal pull-up/down resistor **/
#define CHB_SLPTRPORT               (1)
#define CHB_SLPTRPIN                (9)             /** Reset Pin **/
#define CHB_SLPTRPIN_IOCONREG       (IOCON_PIO1_9)  /** Required to disable internal pull-up/down resistor **/
    
#define CHB_ENTER_CRIT()            __disable_irq() // Requires CMSIS (core_cm3.h)
#define CHB_LEAVE_CRIT()            __enable_irq()  // Requires CMSIS (core_cm3.h)
#define CHB_RST_ENABLE()            do {gpioSetValue(CHB_RSTPORT, CHB_RSTPIN, 0); timer32DelayUS(0, 500);} while (0)
#define CHB_RST_DISABLE()           do {gpioSetValue(CHB_RSTPORT, CHB_RSTPIN, 1); timer32DelayUS(0, 500);} while (0)
#define CHB_SLPTR_ENABLE()          do {gpioSetValue(CHB_SLPTRPORT, CHB_SLPTRPIN, 1); timer32DelayUS(0, 500);} while (0)
#define CHB_SLPTR_DISABLE()         do {gpioSetValue(CHB_SLPTRPORT, CHB_SLPTRPIN, 0); timer32DelayUS(0, 500);} while (0)

#define CHB_INIT_MAXRETRIES         (50)  /**< Maximum number of attempts to get RX_AACK_ON status in Init */

// CCA constants
enum
{
    CCA_ED                    = 1,    /**< Use energy detection above threshold mode. */
    CCA_CARRIER_SENSE         = 2,    /**< Use carrier sense mode. */
    CCA_CARRIER_SENSE_WITH_ED = 3     /**< Use a combination of both energy detection and carrier sense. */
};

// configuration parameters
enum
{
    CHB_CHANNEL             = 1,
    CHB_PAN_ID              = 0x1234,
    CHB_TX_PWR              = 0x0,
    CHB_SHORT_ADDR          = 0x0,
    CHB_IEEE_ADDR           = 0x0,
    CHB_MAX_FRAME_RETRIES   = 3,
    CHB_MAX_CSMA_RETRIES    = 4,
    CHB_CCA_MODE            = CCA_ED,
    CHB_MIN_BE              = 3,
    CHB_MAX_BE              = 5,
    CHB_CCA_ED_THRES        = 0x7,
    CHB_CSMA_SEED0          = 0,
    CHB_CSMA_SEED1          = 0,
    CHB_FRM_VER             = 1         // accept 802.15.4 ver 0 or 1 frames
};

enum
{
    TRX_STATUS      = 0x01,
    TRX_STATE       = 0x02,
    TRX_CTRL_0      = 0x03,
    TRX_CTRL_1      = 0x04,
    PHY_TX_PWR      = 0x05,
    PHY_RSSI        = 0x06,
    PHY_ED_LEVEL    = 0x07,
    PHY_CC_CCA      = 0x08,
    CCA_THRES       = 0x09,
    RX_CTRL         = 0x0a,
    SFD_VALUE       = 0x0b,
    TRX_CTRL_2      = 0x0c,
    ANT_DIV         = 0x0d,
    IRQ_MASK        = 0x0e,
    IRQ_STATUS      = 0x0f,
    VREG_CTRL       = 0x10,
    BATMON          = 0x11,
    XOSC_CTRL       = 0x12,
    CC_CTRL_0       = 0x13,
    CC_CTRL_1       = 0x14,
    RX_SYN          = 0x15,
    RF_CTRL_0       = 0x16,
    XAH_CTRL_1      = 0x17,
    FTN_CTRL        = 0x18,
    RF_CTRL_1       = 0x19,
    PLL_CF          = 0x1a,
    PLL_DCU         = 0x1b,
    PART_NUM        = 0x1c,
    VERSION_NUM     = 0x1d,
    MAN_ID_0        = 0x1e,
    MAN_ID_1        = 0x1f,
    SHORT_ADDR_0    = 0x20,
    SHORT_ADDR_1    = 0x21,
    PAN_ID_0        = 0x22,
    PAN_ID_1        = 0x23,
    IEEE_ADDR_0     = 0x24,
    IEEE_ADDR_1     = 0x25,
    IEEE_ADDR_2     = 0x26,
    IEEE_ADDR_3     = 0x27,
    IEEE_ADDR_4     = 0x28,
    IEEE_ADDR_5     = 0x29,
    IEEE_ADDR_6     = 0x2a,
    IEEE_ADDR_7     = 0x2b,
    XAH_CTRL_0      = 0x2c,
    CSMA_SEED_0     = 0x2d,
    CSMA_SEED_1     = 0x2e,
    CSMA_BE         = 0x2f
};

// random defines
enum
{
    CHB_MAX_FRAME_RETRIES_POS   = 4,
    CHB_MAX_CSMA_RETIRES_POS    = 1,
    CHB_CSMA_SEED1_POS          = 0,
    CHB_CCA_MODE_POS            = 5,
    CHB_AUTO_CRC_POS            = 5,
    CHB_TRX_END_POS             = 3,
    CHB_TRAC_STATUS_POS         = 5,
    CHB_FVN_POS                 = 6,
    CHB_OQPSK_TX_OFFSET         = 2,
    CHB_BPSK_TX_OFFSET          = 3,
    CHB_MIN_FRAME_LENGTH        = 3,
    CHB_MAX_FRAME_LENGTH        = 0x7f,
};

// transceiver timing
enum
{
    TIME_RST_PULSE_WIDTH        = 1, 
    TIME_P_ON_TO_CLKM_AVAIL     = 380,
    TIME_SLEEP_TO_TRX_OFF       = 240,
    TIME_TRX_OFF_TO_SLEEP       = 35,
    TIME_PLL_ON_TRX_OFF         = 1,
    TIME_TRX_OFF_RX_ON          = 110,
    TIME_RX_ON_TRX_OFF          = 1,
    TIME_PLL_ON_RX_ON           = 1,
    TIME_RX_ON_PLL_ON           = 1,
    TIME_PLL_LOCK_TIME          = 110,
    TIME_BUSY_TX_PLL_ON         = 32,
    TIME_ALL_STATES_TRX_OFF     = 1,
    TIME_RESET_TRX_OFF          = 26,
    TIME_TRX_IRQ_DELAY          = 9,
    TIME_TRX_OFF_PLL_ON         = 110,
    TIME_IRQ_PROCESSING_DLY     = 32
};

// trac status
enum
{
    TRAC_SUCCESS               = 0,
    TRAC_SUCCESS_DATA_PENDING  = 1,
    TRAC_WAIT_FOR_ACK          = 2,
    TRAC_CHANNEL_ACCESS_FAIL   = 3,
    TRAC_NO_ACK                = 5,
    TRAC_INVALID               = 7
};

// radio statuses
enum
{
    RADIO_SUCCESS = 0x40,                       /**< The requested service was performed successfully. */
    RADIO_UNSUPPORTED_DEVICE,                   /**< The connected device is not an Atmel AT86RF212. */
    RADIO_INVALID_ARGUMENT,                     /**< One or more of the supplied function arguments are invalid. */
    RADIO_TIMED_OUT,                            /**< The requested service timed out. */
    RADIO_WRONG_STATE,                          /**< The end-user tried to do an invalid state transition. */
    RADIO_BUSY_STATE,                           /**< The radio transceiver is busy receiving or transmitting. */
    RADIO_STATE_TRANSITION_FAILED,              /**< The requested state transition could not be completed. */
    RADIO_CCA_IDLE,                             /**< Channel is clear, available to transmit a new frame. */
    RADIO_CCA_BUSY,                             /**< Channel busy. */
    RADIO_TRX_BUSY,                             /**< Transceiver is busy receiving or transmitting data. */
    RADIO_BAT_LOW,                              /**< Measured battery voltage is lower than voltage threshold. */
    RADIO_BAT_OK,                               /**< Measured battery voltage is above the voltage threshold. */
    RADIO_CRC_FAILED,                           /**< The CRC failed for the actual frame. */
    RADIO_CHANNEL_ACCESS_FAILURE,               /**< The channel access failed during the auto mode. */
    RADIO_NO_ACK,                               /**< No acknowledge frame was received. */
};

// transceiver commands
enum
{
    CMD_NOP                  = 0x00,
    CMD_TX_START             = 0x02,
    CMD_FORCE_TRX_OFF        = 0x03,
    CMD_FORCE_PLL_ON         = 0x04,
    CMD_RX_ON                = 0x06,
    CMD_TRX_OFF              = 0x08,
    CMD_PLL_ON               = 0x09,
    CMD_RX_AACK_ON           = 0x16, // 22
    CMD_TX_ARET_ON           = 0x19  // 25
};

// transceiver states
enum
{
    P_ON                = 0x00,
    BUSY_RX             = 0x01,
    BUSY_TX             = 0x02,
    RX_ON               = 0x06,
    TRX_OFF             = 0x08,
    PLL_ON              = 0x09,
    SLEEP               = 0x0F, // 15
    BUSY_RX_AACK        = 0x11, // 17
    BUSY_TX_ARET        = 0x12, // 18
    RX_AACK_ON          = 0x16, // 22
    TX_ARET_ON          = 0x19, // 25
    RX_ON_NOCLK         = 0x1C, // 28
    RX_AACK_ON_NOCLK    = 0x1D, // 29
    BUSY_RX_AACK_NOCLK  = 0x1E, // 30
    TRANS_IN_PROG       = 31
};

// transceiver interrupt register
enum 
{
    IRQ_PLL_LOCK                = 0,
    IRQ_PLL_UNLOCK              = 1,
    IRQ_RX_START                = 2,
    IRQ_TRX_END                 = 3,
    IRQ_CCA_ED_READY            = 4,
    IRQ_AMI                     = 5,
    IRQ_TRX_UR                  = 6,
    IRQ_BAT_LOW                 = 7 
}; 

// transceiver modes
enum
{
    OQPSK_868MHZ    = 0,
    OQPSK_915MHZ    = 1,
    OQPSK_780MHZ    = 2,
    BPSK20_915MHZ   = 3
};

typedef enum
{
  CHB_ERROR_NOERROR     = 0,
  CHB_ERROR_INITTIMEOUT = 1     // Init timed out waiting for AACK
} chbError_t;

// See Table 7-15 for details
typedef enum
{
  CHB_PWR_EU1_2DBM   = 0x63,    // EU (868MHz) Linearized PA mode
  CHB_PWR_EU1_1DBM   = 0x64,    // Note: BPSK 20kbit/s only!
  CHB_PWR_EU1_0DBM   = 0x65,
  CHB_PWR_EU2_5DBM   = 0xE7,    // EU (868MHz) Boost mode (but > supply current)
  CHB_PWR_EU2_4DBM   = 0xE8,    // 4-5dBM BPSK 20 kbit/s only!
  CHB_PWR_EU2_3DBM   = 0xE9,    // 0-3dBM O-QPSK 100/200/400 kbit/s or BPSK
  CHB_PWR_EU2_2DBM   = 0xEA,
  CHB_PWR_EU2_1DBM   = 0xCB,
  CHB_PWR_EU2_0DBM   = 0xAB,
  CHB_PWR_NA_10DBM   = 0xC0,    // North America (915MHz)
  CHB_PWR_NA_9DBM    = 0xA1,
  CHB_PWR_NA_8DBM    = 0x81,
  CHB_PWR_NA_7DBM    = 0x82,
  CHB_PWR_NA_6DBM    = 0x83,
  CHB_PWR_NA_5DBM    = 0x60,
  CHB_PWR_NA_4DBM    = 0x61,
  CHB_PWR_NA_3DBM    = 0x41,
  CHB_PWR_NA_2DBM    = 0x42,
  CHB_PWR_NA_1DBM    = 0x22,
  CHB_PWR_NA_0DBM    = 0x23,
  CHB_PWR_CHINA_5DBM = 0xE7,    // China (780MHz)
  CHB_PWR_CHINA_4DBM = 0xE8,
  CHB_PWR_CHINA_3DBM = 0xE9,
  CHB_PWR_CHINA_2DBM = 0xEA,
  CHB_PWR_CHINA_1DBM = 0xCA,
  CHB_PWR_CHINA_0DBM = 0xAA
} chbPower_t;

// init 
chbError_t chb_drvr_init();

// data access
uint8_t chb_reg_read(uint8_t addr);
uint16_t chb_reg_read16(uint8_t addr);
void chb_reg_write(uint8_t addr, uint8_t val);
void chb_reg_write16(uint8_t addr, uint16_t val);
void chb_reg_write64(uint8_t addr, uint8_t *val);
void chb_reg_read_mod_write(uint8_t addr, uint8_t val, uint8_t mask);
void chb_frame_write(uint8_t *hdr, uint8_t hdr_len, uint8_t *data, uint8_t data_len);

// general configuration
void chb_set_mode(uint8_t mode);
uint8_t chb_set_channel(uint8_t channel);
void chb_set_pwr(uint8_t val);
void chb_set_ieee_addr(uint8_t *addr);
void chb_get_ieee_addr(uint8_t *addr);
void chb_set_short_addr(uint16_t addr);
uint16_t chb_get_short_addr();
uint8_t chb_set_state(uint8_t state);

// data transmit
uint8_t chb_tx(uint8_t *hdr, uint8_t *data, uint8_t len);

#ifdef CHB_DEBUG
// sram access
void chb_sram_read(uint8_t addr, uint8_t len, uint8_t *data);
void chb_sram_write(uint8_t addr, uint8_t len, uint8_t *data);
#endif

void chb_ISR_Handler (void);
#endif

