
#ifndef __TP_CST826_H__
#define __TP_CST826_H__

//----------------------------头文件依赖----------------------------//
#include "dev_touch.h"

//----------------------------函数定义----------------------------//
uint8_t cst826_init(void);
uint8_t cst826_read_points(tp_dev_t *tp_dev);

#endif //__TP_CST826_H__
