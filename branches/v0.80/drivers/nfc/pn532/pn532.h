#ifndef _PN532_H_
#define _PN532_H_

#include "projectconfig.h"

void pn532Init (void);
void pn532PollInput (void);
void pn532PollNFC (void);

#endif


