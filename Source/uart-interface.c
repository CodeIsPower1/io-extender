/*-----------------------------------------------------------------------------
 * Name:    stdin_USART.c
 * Purpose: STDIN USART Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/
 
/* Copyright (c) 2013 - 2015 ARM LIMITED
 
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
#include "stm32f10x.h"
#include "Driver_USART.h"
#include "rtx_os.h"
 
//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
 
// <h>STDIN USART Interface
 
//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for USART interface
#define USART_DRV_NUM           4
 
//   <o>Baudrate
#define USART_BAUDRATE          115200
 
// </h>
 
 
#define _USART_Driver_(n)  Driver_USART##n
#define  USART_Driver_(n) _USART_Driver_(n)
 
extern ARM_DRIVER_USART  USART_Driver_(USART_DRV_NUM);
#define ptrUSART       (&USART_Driver_(USART_DRV_NUM))
 


static unsigned char recv_buff[100];
static unsigned char send_buff[100];


__WEAK void USART4_SignalEvent_Callback (uint32_t event) {
	switch (event) {
		case ARM_USART_EVENT_SEND_COMPLETE:
		{
		
		}break;
		
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
		{
		
		}break;
	}
}

/**
  Initialize stdin
 
  \return          0 on success, or -1 on error.
*/
int uart_init (void) {
  int32_t status;
 
  status = ptrUSART->Initialize(USART4_SignalEvent_Callback);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->PowerControl(ARM_POWER_FULL);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->Control(ARM_USART_MODE_ASYNCHRONOUS |
                             ARM_USART_DATA_BITS_8       |
                             ARM_USART_PARITY_NONE       |
                             ARM_USART_STOP_BITS_1       |
                             ARM_USART_FLOW_CONTROL_NONE,
                             USART_BAUDRATE);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->Control(ARM_USART_CONTROL_RX, 1);
  if (status != ARM_DRIVER_OK) return (-1);
	
	status = ptrUSART->Control(ARM_USART_CONTROL_TX, 1);
  if (status != ARM_DRIVER_OK) return (-1);
 
  return (0);
}
 
 
/**
  Get a character from stdin
 
  \return     The next character from the input, or -1 on read error.
*/
int uart_getchar (void) {
	unsigned char r[1];
  if (ptrUSART->Receive(r, 1) != ARM_DRIVER_OK) {
    return (-1);
  }
  while (ptrUSART->GetRxCount() != 1);
  return (r[0]);
}

int uart_read (unsigned char *r, unsigned int length) {
  uint32_t start_tick,cur_tick;
 
  start_tick = osKernelGetTickCount();

  if (ptrUSART->Receive(r, length) != ARM_DRIVER_OK) {
    return (-1);
  }
	do {
		if ((ptrUSART->GetRxCount() == length)) {
			break;
		}
		cur_tick = osKernelGetTickCount();
		if (start_tick+ osKernelGetTickFreq()*20/1000 < cur_tick) {
			break;
		}
	}while(1);
  return (ptrUSART->GetRxCount());
}

int uart_write (unsigned char *r, unsigned int length) {
  uint32_t start_tick,cur_tick;
 
  start_tick = osKernelGetTickCount();

  if (ptrUSART->Send(r, length) != ARM_DRIVER_OK) {
    return (-1);
  }
	do {
		if ((ptrUSART->GetTxCount() == length)) {
			break;
		}
		cur_tick = osKernelGetTickCount();
		if (start_tick+ osKernelGetTickFreq()*20/1000 < cur_tick) {
			break;
		}
	}while(1);
  return (ptrUSART->GetTxCount());
}