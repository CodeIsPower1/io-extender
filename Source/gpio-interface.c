/*-----------------------------------------------------------------------------
 * Name:    IO_MCBSTM32C.c
 * Purpose: LED interface for MCBSTM32C evaluation board
 * Rev.:    1.0.0
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2015 - 2017 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "led-interface.h"
#include "GPIO_STM32F10x.h"
#include "gpio-interface.h"

const GPIO_CONFIG Pins[] = {
  [IN0]={{ GPIOC, 0}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN1]={{ GPIOC, 1}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN2]={{ GPIOC, 2}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN3]={{ GPIOC, 3}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN4]={{ GPIOA, 1}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN5]={{ GPIOA, 2}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN6]={{ GPIOA, 3}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN7]={{ GPIOA, 4}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN8]={{ GPIOA, 5}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN9]={{ GPIOA, 6}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN10]={{ GPIOA, 7}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN11]={{ GPIOC, 4}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN12]={{ GPIOC, 5}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN13]={{ GPIOB, 0}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN14]={{ GPIOB, 1}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[IN15]={{ GPIOB, 10}, GPIO_IN_FLOATING, GPIO_MODE_INPUT},
	[OUT3]={{ GPIOB, 12}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT4]={{ GPIOB, 13}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT5]={{ GPIOB, 14}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT6]={{ GPIOB, 15}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT7]={{ GPIOC, 6}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT8]={{ GPIOC, 7}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT9]={{ GPIOC, 8}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT10]={{ GPIOC, 9}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[ARM_IO_OUT0]={{ GPIOA, 8}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[ARM_IO_OUT1]={{ GPIOA, 9}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT1]={{ GPIOA, 10}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
	[OUT2]={{ GPIOA, 11}, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ},
};

#define IO_COUNT (sizeof(Pins)/sizeof(GPIO_CONFIG))


/**
  \fn          int32_t IO_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t IO_Initialize (void) {
  uint32_t n;

  /* Configure pins:  */
  for (n = 0; n < IO_COUNT; n++) {
    GPIO_PortClock   (Pins[n].pin.port, true);
    GPIO_PinWrite    (Pins[n].pin.port, Pins[n].pin.num, 0);
    GPIO_PinConfigure(Pins[n].pin.port, Pins[n].pin.num,
                      Pins[n].conf,
                      Pins[n].mode);
  }

  return 0;
}

/**
  \fn          int32_t IO_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t IO_Uninitialize (void) {
  uint32_t n;

  /* Configure pins: Input mode, without Pull-up/down resistors */
  for (n = 0; n < IO_COUNT; n++) {
    GPIO_PinConfigure(Pins[n].pin.port, Pins[n].pin.num,
                      GPIO_IN_FLOATING,
                      GPIO_MODE_INPUT);
  }

  return 0;
}

/**
  \fn          int32_t IO_High (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t IO_High (uint32_t num) {
  int32_t retCode = 0;

  if (num < IO_COUNT) {
    GPIO_PinWrite(Pins[num].pin.port, Pins[num].pin.num, 1);
  }
  else {
    retCode = -1;
  }

  return retCode;
}

/**
  \fn          int32_t IO_Low (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t IO_Low (uint32_t num) {
  int32_t retCode = 0;

  if (num < IO_COUNT) {
    GPIO_PinWrite(Pins[num].pin.port, Pins[num].pin.num, 0);
  }
  else {
    retCode = -1;
  }

  return retCode;
}



int32_t IO_Get (uint32_t num) {

  if (num < IO_COUNT) {
    return GPIO_PinRead(Pins[num].pin.port, Pins[num].pin.num);
  }
  return -1;
}


/**
  \fn          uint32_t IO_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t IO_GetCount (void) {

  return IO_COUNT;
}
