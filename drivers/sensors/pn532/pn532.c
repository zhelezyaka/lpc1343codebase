/**************************************************************************/
/*! 
    @file     pn532.c
*/
/**************************************************************************/
#include <string.h>

#include "pn532.h"
#include "core/systick/systick.h"

#ifdef PN532_INTERFACE_UART
#include "core/uart/uart.h"
#endif

static pn532_pcb_t pcb;     // PN532 Peripheral control block
static byte_t abtRx[256];   // Receive buffer

/* ======================================================================
   PRIVATE FUNCTIONS
   ====================================================================== */

/**************************************************************************/
/*! 
    Prints a hexadecimal value in plain characters
*/
/**************************************************************************/
void print_hex(const byte_t* pbtData, const size_t szBytes)
{
  size_t szPos;
  for (szPos=0; szPos < szBytes; szPos++) {
    printf("%02x ",pbtData[szPos]);
  }
  printf(CFG_PRINTF_NEWLINE);
}

/**************************************************************************/
/*! 
    Builds a standard PN532 frame
*/
/**************************************************************************/
void pn532BuildFrame(byte_t * pbtFrame, size_t * pszFrame, const byte_t * pbtData, const size_t szData)
{
  if (szData <= PN532_NORMAL_FRAME__DATA_MAX_LEN) {
    // LEN - Packet length = data length (len) + checksum (1) + end of stream marker (1)
    pbtFrame[3] = szData + 1;
    // LCS - Packet length checksum
    pbtFrame[4] = 256 - (szData + 1);
    // TFI
    pbtFrame[5] = 0xD4;
    // DATA - Copy the PN53X command into the packet buffer
    memcpy (pbtFrame + 6, pbtData, szData);

    // DCS - Calculate data payload checksum
    byte_t btDCS = (256 - 0xD4);
	size_t szPos;
    for (szPos = 0; szPos < szData; szPos++) {
      btDCS -= pbtData[szPos];
    }
    pbtFrame[6 + szData] = btDCS;

    // 0x00 - End of stream marker
    pbtFrame[szData + 7] = 0x00;

    (*pszFrame) = szData + PN532_NORMAL_FRAME__OVERHEAD;
  } else {
    // FIXME: Build extended frame
    // abort();
  }
}

/**************************************************************************/
/*! 

*/
/**************************************************************************/
bool pn532Send(const byte_t * pbtData, const size_t szData)
{
  // Every packet must start with "00 00 ff"
  byte_t  abtFrame[PN532_BUFFER_LEN] = { 0x00, 0x00, 0xff };
  size_t szFrame = 0;

  pn532BuildFrame (abtFrame, &szFrame, pbtData, szData);

  printf("Sending  (%02d): ", szFrame);
  print_hex(abtFrame, szFrame);

  #ifdef PN532_INTERFACE_UART
  uartSend (abtFrame, szFrame);
  #endif

  // Wait for ACK
  byte_t abtRxBuf[6];
  uart_pcb_t *pcb = uartGetPCB();
  // FIXME: How long should we wait for ACK?
  systickDelay(10);
  if (pcb->rxfifo.len < 6) {
    PN532_DEBUG ("%s", "Unable to read ACK\r\n");
    return false;
  }

  // Read ACK ... this will also remove it from the buffer
  const byte_t abtAck[6] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };
  abtRxBuf[0] = uartRxBufferRead();
  abtRxBuf[1] = uartRxBufferRead();
  abtRxBuf[2] = uartRxBufferRead();
  abtRxBuf[3] = uartRxBufferRead();
  abtRxBuf[4] = uartRxBufferRead();
  abtRxBuf[5] = uartRxBufferRead();
  if (0 != (memcmp (abtRxBuf, abtAck, 6))) {
    PN532_DEBUG ("%s", "Invalid ACK\r\n");
    return false;
  }  

  return true;
}

/**************************************************************************/
/*! 

*/
/**************************************************************************/
bool pn532ReadResponse(void)
{
  size_t szRxLen;

  #ifdef PN532_INTERFACE_UART
  // Read response and print to screen
  if (!uartRxBufferReadArray(abtRx, &szRxLen)) {
    return false;
  }
  printf("Received (%02d): ",szRxLen);
  print_hex(abtRx,szRxLen);

  // Check preamble
  const byte_t pn53x_preamble[3] = { 0x00, 0x00, 0xff };
  if (0 != (memcmp (abtRx, pn53x_preamble, 3))) {
    PN532_DEBUG("%s", "Frame preamble+start code mismatch\r\n");
    return false;
  }

  if ((0x01 == abtRx[3]) && (0xff == abtRx[4])) {
    // Error frame
    PN532_DEBUG("%s", "Application level error detected\r\n");
    return false;
  } else if ((0xff == abtRx[3]) && (0xff == abtRx[4])) {
    // Extended frame
    // FIXME: Code this
    PN532_DEBUG("%s", "Extended frames currently unsupported\r\n");
    return false;
  } else {
    // Normal frame
    if (256 != (abtRx[3] + abtRx[4])) {
      // TODO: Retry
      PN532_DEBUG("%s", "Length checksum mismatch\r\n");
      return false;
    }
    // size_t szPayloadLen = abtRx[3] - 2;
  }

  return true;
  #endif
}

/* ======================================================================
   PUBLIC FUNCTIONS
   ====================================================================== */

/**************************************************************************/
/*! 
    @brief      Gets a reference to the PN532 peripheral control block,
                which can be used to determine that state of the PN532
                IC, buffers, etc.
*/
/**************************************************************************/
pn532_pcb_t *pn532GetPCB()
{
  return &pcb;
}

/**************************************************************************/
/*! 
    @brief      Initialises the appropriate serial bus (UART, etc.),and
                sets up any buffers or peripherals required by the PN532.
*/
/**************************************************************************/
void pn532Init(void)
{
  // Clear protocol control blocks
  memset(&pcb, 0, sizeof(pn532_pcb_t));

  #ifdef PN532_INTERFACE_UART
  PN532_DEBUG("Initialising UART (%d)\r\n", PN532_UART_BAUDRATE);
  uartInit(PN532_UART_BAUDRATE);
  uartRxBufferInit();
  #endif

  pcb.initialised = TRUE;
  pcb.awake = FALSE;
}

/**************************************************************************/
/*! 
    @brief      Sends the wakeup sequence to the PN532.
*/
/**************************************************************************/
void pn532Wakeup(void)
{
  byte_t abtWakeUp[] = { 0x55,0x55,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00 };

  PN532_DEBUG("Sending Wakeup Sequence\r\n");

  #ifdef PN532_INTERFACE_UART
  uartSend(abtWakeUp,sizeof(abtWakeUp));
  systickDelay(100);    // ToDo: Check IRQ for response rather than using an ugly, naive delay
  pn532ReadResponse();
  #endif

  pcb.awake = TRUE;
}

/**************************************************************************/
/*! 
    @brief      Sends a byte array of command and parameter data to the
                PN532, starting with the command byte.  The frame's
                preamble, checksums, postamble and frame identifier (0xD4)
                will all be automatically added.

    @param[in]  abtCommand
                The byte array containg the command and any
                optional paramaters
    @param[in]  szLen
                The number of bytes in abtCommand

    @section Example

    @code

    #include "drivers/sensors/pn532/pn532.h"
    ...
    pn532Init();
    pn532Wakeup();

    // Try to initialise one ISO14443A target at 106kbps 
    byte_t abtCommand[] = { PN532_COMMAND_INLISTPASSIVETARGET, 0x01, 
                            PN532_MODULATION_ISO14443A };
    pn532SendCommand(abtCommand, sizeof(abtCommand));

    @endcode
*/
/**************************************************************************/
void pn532SendCommand(byte_t * abtCommand, size_t szLen)
{
  if (pcb.awake == FALSE) pn532Wakeup();

  pn532Send(abtCommand, szLen);
  systickDelay(100);
  pn532ReadResponse();
}
