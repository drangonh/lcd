#ifndef __DEV_UART_H__
#define __DEV_UART_H__

//----------------------------ͷ�ļ�����----------------------------//
#include "SWM320.h"

//----------------------------��������----------------------------//
void serial_init(void);
void uart_write(const unsigned char *pbuffer, unsigned numbytes);

#endif //__DEV_UART_H__
