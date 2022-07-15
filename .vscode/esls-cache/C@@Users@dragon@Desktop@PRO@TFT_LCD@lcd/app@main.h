#ifndef __MAIN_H__
#define __MAIN_H__

//----------------------------头文件依�?----------------------------//
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "SWM320.h"

#include "version.h"

#include "ff.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
//#include "lv_port_fs.h"
#include "lv_conf.h"

#include "dev_systick.h"
#include "dev_uart.h"
#include "dev_rtc.h"
#include "dev_sdio.h"
#include "dev_sdram.h"
#include "dev_spiflash.h"
#include "dev_lcd.h"
#include "dev_touch.h"

#if (0 != __RESOLUTION_TARGET__ && 4 != __RESOLUTION_TARGET__)
#include "load_file.h"
#include "ui_page.h"
#endif //(0 != __RESOLUTION_TARGET__ && 4 != __RESOLUTION_TARGET__)

//----------------------------全局�?----------------------------//
/* 简单打印调试信�?, 后续考虑加上 log.c/h */
#define USE_DEBUG 
#ifdef  USE_DEBUG
	#define DEBUG(...)           printf( __VA_ARGS__)
	#define DEBUG_WARN(...)      printf("Warning--" __VA_ARGS__)
	#define DEBUG_ERROR(...)     printf("ERROR--" __VA_ARGS__)
	
#else
    #define DEBUG(...)          ;
    #define DEBUG_WARN(...)     ;
    #define DEBUG_ERROR(...)    ;
	
#endif //USE_DEBUG

//----------------------------外部全局变量----------------------------//
extern volatile uint32_t Tick_Time;
extern volatile uint32_t TIMR0_Time;

#endif //__MAIN_H__
