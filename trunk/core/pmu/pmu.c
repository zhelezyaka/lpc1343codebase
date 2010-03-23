/**************************************************************************/
/*! 
    @file     pmu.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section DESCRIPTION

    Controls the power management features of the LPC1343, allowing you
    to enter sleep/deep-sleep or deep power-down mode.

    For examples of how to enter either mode, see the comments for the
    functions pmuSleep(), pmuDeepSleep() and pmuPowerDown().
	
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

#include "LPC134x.h"

#include "core/gpio/gpio.h"
#include "pmu.h"
#include "nvic13xx.h"

volatile uint32_t pmu_counter = 0;

/**************************************************************************/
/*! 
    Wakeup interrupt handler
*/
/**************************************************************************/
void WAKEUP_IRQHandler(void)
{
  uint32_t regVal;

  /* This handler takes care all the port pins if they
  are configured as wakeup source. */
  regVal = SCB_STARTSRP0;
  if (regVal != 0)
  {
    SCB_STARTRSRP0CLR = regVal;
  }
  regVal = SCB_STARTSRP1;
  if ( regVal != 0 )
  {
    SCB_STARTRSRP1CLR = regVal;
  }

  /* See tracker for bug report. */
  __asm volatile ("NOP");

  return;
}

/**************************************************************************/
/*! 
    @brief Initialises the power management unit

    Initialise the power management unit, and configure pin 0.1 to act as
    a wakeup source from sleep or deep-sleep mode.

    For sleep and deep-sleep modes -- entered via pmuSleep() -- any I/O pin
    can be used as a wakeup source.  For deep power-down mode -- entered
    via pmuPowerDown() -- only a low level on pin 1.4 (WAKEUP) can wake the
    device up.
*/
/**************************************************************************/
void pmuInit( void )
{
  /* Enable all clocks, even those turned off at power up. */
  SCB_PDRUNCFG &= ~(SCB_PDRUNCFG_WDTOSC_MASK | 
                    SCB_PDRUNCFG_SYSOSC_MASK | 
                    SCB_PDRUNCFG_ADC_MASK);

  /* Enable wakeup interrupts (any I/O pin can be used as a wakeup source) */
  //NVIC_EnableIRQ(WAKEUP0_IRQn);    // P0.0
  NVIC_EnableIRQ(WAKEUP1_IRQn);    // P0.1
  //NVIC_EnableIRQ(WAKEUP2_IRQn);    // P0.2
  //NVIC_EnableIRQ(WAKEUP3_IRQn);    // P0.3
  //NVIC_EnableIRQ(WAKEUP4_IRQn);    // P0.4
  //NVIC_EnableIRQ(WAKEUP5_IRQn);    // P0.5
  //NVIC_EnableIRQ(WAKEUP6_IRQn);    // P0.6
  //NVIC_EnableIRQ(WAKEUP7_IRQn);    // P0.7
  //NVIC_EnableIRQ(WAKEUP8_IRQn);    // P0.8
  //NVIC_EnableIRQ(WAKEUP9_IRQn);    // P0.9
  //NVIC_EnableIRQ(WAKEUP10_IRQn);   // P0.10
  //NVIC_EnableIRQ(WAKEUP11_IRQn);   // P0.11
  //NVIC_EnableIRQ(WAKEUP12_IRQn);   // P1.0
  //NVIC_EnableIRQ(WAKEUP13_IRQn);   // P1.1
  //NVIC_EnableIRQ(WAKEUP14_IRQn);   // P1.2
  //NVIC_EnableIRQ(WAKEUP15_IRQn);   // P1.3
  //NVIC_EnableIRQ(WAKEUP16_IRQn);   // P1.4
  //NVIC_EnableIRQ(WAKEUP17_IRQn);   // P1.5
  //NVIC_EnableIRQ(WAKEUP18_IRQn);   // P1.6
  //NVIC_EnableIRQ(WAKEUP19_IRQn);   // P1.7
  //NVIC_EnableIRQ(WAKEUP20_IRQn);   // P1.8
  //NVIC_EnableIRQ(WAKEUP21_IRQn);   // P1.9
  //NVIC_EnableIRQ(WAKEUP22_IRQn);   // P1.10
  //NVIC_EnableIRQ(WAKEUP23_IRQn);   // P1.11
  //NVIC_EnableIRQ(WAKEUP24_IRQn);   // P2.0
  //NVIC_EnableIRQ(WAKEUP25_IRQn);   // P2.1
  //NVIC_EnableIRQ(WAKEUP26_IRQn);   // P2.2
  //NVIC_EnableIRQ(WAKEUP27_IRQn);   // P2.3
  //NVIC_EnableIRQ(WAKEUP28_IRQn);   // P2.4
  //NVIC_EnableIRQ(WAKEUP29_IRQn);   // P2.5
  //NVIC_EnableIRQ(WAKEUP30_IRQn);   // P2.6
  //NVIC_EnableIRQ(WAKEUP31_IRQn);   // P2.7
  //NVIC_EnableIRQ(WAKEUP32_IRQn);   // P2.8
  //NVIC_EnableIRQ(WAKEUP33_IRQn);   // P2.9
  //NVIC_EnableIRQ(WAKEUP34_IRQn);   // P2.10
  //NVIC_EnableIRQ(WAKEUP35_IRQn);   // P2.11
  //NVIC_EnableIRQ(WAKEUP36_IRQn);   // P3.0
  //NVIC_EnableIRQ(WAKEUP37_IRQn);   // P3.1
  //NVIC_EnableIRQ(WAKEUP38_IRQn);   // P3.2
  //NVIC_EnableIRQ(WAKEUP39_IRQn);   // P3.3

  /* Use pin 0.1 as wakeup source and set it as an i/o pin */
  IOCON_PIO0_1 &= ~IOCON_PIO0_1_FUNC_MASK;
  IOCON_PIO0_1 |= (IOCON_PIO0_1_FUNC_GPIO |
                   IOCON_PIO0_1_HYS_ENABLE);

  /* Set pin to input for wakeup */
  gpioSetDir( 0, 1, 0 );	

  /* Only edge trigger. activation polarity on P0.1 is FALLING EDGE. */
  SCB_STARTAPRP0 = ~SCB_STARTAPRP0_MASK;  // 0x00000000

  /* Clear all wakeup source */ 
  SCB_STARTRSRP0CLR = SCB_STARTRSRP0CLR_MASK;
  SCB_STARTRSRP1CLR = SCB_STARTRSRP1CLR_MASK;

  /* Enable Port 0.1 as wakeup source. */
  SCB_STARTERP0 = SCB_STARTERP0_ERPIO0_1;
  return;
}

/**************************************************************************/
/*! 
    @brief Puts select peripherals in sleep mode.

    This function will put the device into sleep mode.  Most gpio pins
    can be used to wake the device up, but the pins must first be
    configured for this in pmuInit.

    @section Example

    @code 
    // Configure wakeup sources before going into sleep/deep-sleep.
    // By default, pin 0.1 is configured as wakeup source (falling edge)
    pmuInit();
  
    // Enter sleep mode
    pmuSleep();
    @endcode
*/
/**************************************************************************/
void pmuSleep()
{
  SCB_PDAWAKECFG = SCB_PDRUNCFG;
  __asm volatile ("WFI");
  return;
}

/**************************************************************************/
/*! 
    @brief Puts select peripherals in deep-sleep mode.

    This function will put the device into deep-sleep mode.  Any gpio
    pins can be used to wake the device up, but the pins must first be
    configured for this in pmuInit.

    The sleepCtrl parameter is used to indicate which peripherals should
    be put in sleep mode (see the SCB_PDSLEEPCFG register for details).  
    
    @param[in]  sleepCtrl  
                The bits to set in the SCB_PDSLEEPCFG register.  This
                controls which peripherals will be put in sleep mode.

    @section Example

    @code 
    uint32_t pmuRegVal;
  
    // Configure wakeup sources before going into sleep/deep-sleep.
    // By default, pin 0.1 is configured as wakeup source (falling edge)
    pmuInit();
  
    // Put peripherals into sleep mode
    pmuRegVal = SCB_PDSLEEPCFG_USBPLL_PD |
              SCB_PDSLEEPCFG_SYSPLL_PD |
              SCB_PDSLEEPCFG_WDTOSC_PD |
              SCB_PDSLEEPCFG_ADC_PD |
              SCB_PDSLEEPCFG_BOD_PD;

    // Enter deep sleep mode
    pmuDeepSleep(pmuRegVal);
    @endcode
*/
/**************************************************************************/
void pmuDeepSleep(uint32_t sleepCtrl)
{
  SCB_PDAWAKECFG = SCB_PDRUNCFG;
  // According to the datasheet, bits 9 and 11 must be set to 1 for DS
  sleepCtrl |= (1 << 9) | (1 << 11);
  SCB_PDSLEEPCFG = sleepCtrl;
  SCB_SCR |= SCB_SCR_SLEEPDEEP;
  __asm volatile ("WFI");
  return;
}

/**************************************************************************/
/*! 
    @brief Puts the device in deep power-down mode.

    This function will configure the PMU control register and enter
    deep power-down mode.  Pre-determined values are stored in the four
    general-purpose registers (PMU_GPREG0..3), which can be used to persist
    any essential system settings while the device is in deep power-down
    mode, so long as 3.3V is still available.

    @warning    The only way to wake a device up from deep power-down mode
                is to set a low-level on P1.4.  If 3.3V power is lost, the
                values stored in the four general-purpose registers will
                also be lost.

    @section Example

    @code 
    #include "core/cpu/cpu.h"
    #include "core/pmu/pmu.h"

    int main(void)
    {
      cpuInit();
      pmuInit();

      // Enter power-down mode
      pmuPowerDown();

      while(1)
      {
        // Device was woken up by WAKEUP pin
      }
    }
    @endcode
*/
/**************************************************************************/
void pmuPowerDown( void )
{
  uint32_t regVal;

  if ( (PMU_PMUCTRL & ((0x1<<8) | (PMU_PMUCTRL_DPDFLAG))) != 0x0 )
  {
    /* Check sleep and deep power down bits. If sleep and/or
       deep power down mode are entered, clear the PCON bits. */
    regVal = PMU_PMUCTRL;
    regVal |= ((0x1<<8) | 
               (PMU_PMUCTRL_DPDEN_SLEEP) |
               (PMU_PMUCTRL_DPDFLAG));
    PMU_PMUCTRL = regVal;

    if ( (PMU_GPREG0 != 0x12345678)||(PMU_GPREG1 != 0x87654321)
       ||(PMU_GPREG2 != 0x56781234)||(PMU_GPREG3 != 0x43218765) )
    {
      while (1);
    }
  }
  else
  {
    /* If in neither sleep nor deep-sleep mode, enter deep power down mode. */
    PMU_GPREG0 = 0x12345678;
    PMU_GPREG1 = 0x87654321;
    PMU_GPREG2 = 0x56781234;
    PMU_GPREG3 = 0x43218765;
    SCB_SCR |= SCB_SCR_SLEEPDEEP;
    PMU_PMUCTRL = PMU_PMUCTRL_DPDEN_DEEPPOWERDOWN;
    __asm volatile ("WFI");
  }
  return;
}
