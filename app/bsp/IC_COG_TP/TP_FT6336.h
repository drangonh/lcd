
#ifndef __TP_FT6336_H__
#define __TP_FT6336_H__

//----------------------------头文件依赖----------------------------//
#include "dev_touch.h"

//----------------------------函数定义----------------------------//
uint8_t ft6336_init(void);
uint8_t ft6336_read_points(tp_dev_t *tp_dev);

#endif //__TP_FT6336_H__
