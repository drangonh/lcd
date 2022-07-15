#ifndef __DEV_TOUCH_H__
#define __DEV_TOUCH_H__

//----------------------------头文件依赖----------------------------//
#include "main.h"

//----------------------------宏定义----------------------------//
//端口配置
#define TP_GPIO_RST 		GPIOM
#define TP_PIN_RST 			PIN20

#define TP_GPIO_INT 		GPIOB
#define TP_PIN_INT 			PIN7

#define TP_PORT_SCL 		PORTA
#define TP_PIN_SCL 			PIN10
#define TP_PIN_SCL_FUN 		FUNMUX0_I2C0_SCL

#define TP_PORT_SDA 		PORTA
#define TP_PIN_SDA 			PIN9
#define TP_PIN_SDA_FUN 		FUNMUX1_I2C0_SDA


#define TP_PRES_DOWN 		(0x80) //触屏被按下
#define TP_CATH_PRES 		(0x40) //有按键按下
#define TP_MAX_TOUCH 		(5)    //电容屏支持的点数,固定为5点

//----------------------------类型声明----------------------------//
//触摸屏控制器
typedef struct
{
    uint16_t x[TP_MAX_TOUCH]; //当前坐标
    uint16_t y[TP_MAX_TOUCH]; //电容屏有最多5组坐标
    uint8_t status;           //笔的状态
                              //b7:按下1/松开0;
                              //b6~b5:保留
                              //b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
} tp_dev_t;

//----------------------------外部变量----------------------------//
extern tp_dev_t TP_Dev;

//----------------------------函数定义----------------------------//
void touch_ic_init(void);
void touch_read_points(void);

#endif //__DEV_TOUCH_H__
