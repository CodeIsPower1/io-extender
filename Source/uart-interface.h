#ifndef __UART_INTERFACE_H__
#define __UART_INTERFACE_H__

int uart_read (unsigned char *r, unsigned int length);
int uart_write (unsigned char *r, unsigned int length);

#endif //__UART_INTERFACE_H__