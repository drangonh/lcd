#ifndef __DEV_UART_H__
#define __DEV_UART_H__

//----------------------------头文件依赖----------------------------//
#include "SWM320.h"

//----------------------------函数声明----------------------------//
void serial_init(void);
void uart_write(const unsigned char *pbuffer, unsigned numbytes);

#endif //__DEV_UART_H__
