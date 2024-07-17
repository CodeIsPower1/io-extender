

#include <stdio.h>
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "led-interface.h"
#include "gpio-interface.h"
#include "mbtable.h"
#include "modbus-rtu-slave.h"

int uart_init (void);
int uart_getchar (void);


typedef struct gpio_ctl{
	uint8_t state; /* 0:init; 1:doing; 2:over  */
	uint8_t mode_bk;/* 0:init;  */
	uint8_t mode_up; /* 0:init; 1:updated */
	uint8_t mode;/* 0:init;  */
	uint16_t count;
}gpio_ctl;
 
gpio_ctl gpio_ctl_val[IO_QTY];

void cfg_gpio_ctl (uint32_t io, uint32_t mode) {
	if (io < IO_QTY) {
		gpio_ctl_val[io].mode = mode;
		gpio_ctl_val[io].mode_up = 1;
	}
}

void io_thread (void *argument) {
  (void)argument;
	// ...
	int loopidx=0;
	int32_t r;
	uint16_t max_count;
	IO_Initialize();
	while(1) {
		for (loopidx=0; loopidx< IO_QTY; loopidx++) {
			if( (r = IO_Get(IN0+loopidx)) >= 0) {
				mbtable_set(IN_STATE_IN0+loopidx, r);
			}
			if (loopidx >= OUT3) {
				if((gpio_ctl_val[loopidx].state == 0 || gpio_ctl_val[loopidx].state == 2) && (gpio_ctl_val[loopidx].mode_up != 0)) {
					gpio_ctl_val[loopidx].mode_bk = gpio_ctl_val[loopidx].mode;
					gpio_ctl_val[loopidx].mode_up = 0;
					gpio_ctl_val[loopidx].state = 0;
					gpio_ctl_val[loopidx].count = 0;
				}

#define OUT_PULSE  				                                        \
				if (gpio_ctl_val[loopidx].state == 0) {                 \
					IO_High(loopidx);                					\
					gpio_ctl_val[loopidx].state = 1;                    \
				}                                                       \
				if (gpio_ctl_val[loopidx].state == 1) {                 \
					if (gpio_ctl_val[loopidx].count++ >= max_count) {   \
						IO_Low(loopidx);                                \
						gpio_ctl_val[loopidx].state = 2;                \
					}                                                   \
				}

				switch(gpio_ctl_val[loopidx].mode_bk) {
					case OUT_LOW:{
						if (gpio_ctl_val[loopidx].state == 0) {
							IO_Low(loopidx);
							gpio_ctl_val[loopidx].state = 2;
						}
					}break;
					case OUT_HIGH:{
						if (gpio_ctl_val[loopidx].state == 0) {
							IO_High(loopidx);
							gpio_ctl_val[loopidx].state = 2;
						}
					}break;
					case OUT_HIGH_PULSE_100MS:{
						max_count = 10;
                        OUT_PULSE;
					}break;
					case OUT_HIGH_PULSE_200MS:{
						max_count = 20;
                        OUT_PULSE;
					}break;
					case OUT_HIGH_PULSE_500MS:{
						max_count = 50;
                        OUT_PULSE;
					}break;
					case OUT_HIGH_PULSE_800MS:{
						max_count = 80;
                        OUT_PULSE;
					}break;
					case OUT_HIGH_PULSE_1S:{
						max_count = 100;
                        OUT_PULSE;
					}break;
					case OUT_HIGH_PULSE_1500MS:{
						max_count = 150;
                        OUT_PULSE;
					}break;
					case OUT_HIGH_PULSE_2S:{
						max_count = 200;
                        OUT_PULSE;
					}break;
					case OUT_HIGH_PULSE_4S:{
						max_count = 400;
                        OUT_PULSE;
					}break;
				}

			}
			
		}
		osDelay(10);
	}

}
 
 
 void uart_thread (void *argument) {
	(void)argument;
	// ...
	uint32_t start_tick,cur_tick;
 
	start_tick = osKernelGetTickCount();


	uart_init();
	for (;;) {
		LocalSlaveProcess();

		cur_tick = osKernelGetTickCount();
		if (start_tick+ osKernelGetTickFreq()*10000/1000 < cur_tick) {
			start_tick = cur_tick;
			cfg_gpio_ctl(OUT3, OUT_HIGH_PULSE_100MS);
			cfg_gpio_ctl(OUT4, OUT_HIGH_PULSE_200MS);
			cfg_gpio_ctl(OUT5, OUT_HIGH_PULSE_500MS);
			cfg_gpio_ctl(OUT6, OUT_HIGH_PULSE_800MS);
            cfg_gpio_ctl(OUT7, OUT_HIGH_PULSE_1S);
			cfg_gpio_ctl(OUT8, OUT_HIGH_PULSE_1500MS);
			cfg_gpio_ctl(OUT9, OUT_HIGH_PULSE_2S);
			cfg_gpio_ctl(OUT10, OUT_HIGH_PULSE_4S);
		}
	}
}
 
 
 

void app_main (void *argument) {
	(void)argument;
	osThreadNew(io_thread, NULL, NULL);    // Create application main thread
	osThreadNew(uart_thread, NULL, NULL);    // Create application main thread
	int qty,loopidx=0;
	LED_Initialize();
	
	qty = LED_GetCount();
  for (;;) {
		for (int idx=0; idx < qty;idx++) {
			if(loopidx%2) {
				LED_On(idx);
			} else {
				LED_Off(idx);
			}
		}
		if(loopidx%2) {
			osDelay(200);
		} else {
			osDelay(20);
		}
		loopidx++;
	}
}