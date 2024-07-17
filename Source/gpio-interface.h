#ifndef __GPIO_INTERFACE_H__
#define __GPIO_INTERFACE_H__

#include "GPIO_STM32F10x.h"

typedef struct GPIO_CONFIG {
	GPIO_PIN_ID pin;
	GPIO_CONF   conf;
	GPIO_MODE   mode;
}GPIO_CONFIG;

enum {
	IN0=0,
	IN1,
	IN2,
	IN3,
	IN4,
	IN5,
	IN6,
	IN7,
	IN8,
	IN9,
	IN10,
	IN11,
	IN12,
	IN13,
	IN14,
	IN15,
	OUT3,
	OUT4,
	OUT5,
	OUT6,
	OUT7,
	OUT8,
	OUT9,
	OUT10,
	ARM_IO_OUT0,
	ARM_IO_OUT1,
	OUT1,
	OUT2,
	IO_QTY,
};


int32_t IO_Initialize (void);
/**
  \fn          int32_t IO_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t IO_Uninitialize (void);
/**
  \fn          int32_t IO_High (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t IO_High (uint32_t num);
/**
  \fn          int32_t IO_Low (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t IO_Low (uint32_t num);

int32_t IO_Get (uint32_t num);
/**
  \fn          uint32_t IO_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t IO_GetCount (void);


#endif