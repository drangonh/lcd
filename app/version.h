/**************************************************************************/ /**
 * @file     version.h
 * @brief    版本/目标控制
 * @version  V2.0
 * @date     2022.03.09
 ******************************************************************************/
#ifndef __VERSION_H__
#define __VERSION_H__

/* __RESOLUTION_TARGET__ 在 Keil Option for Target => C/C++ => Preprocessor Symbols => Define 设置
 * enum : 编译目标选择
 * 0 => LVGL 官方Demo, 其UI演示的分辨率自适应, 例子选择在 lv_ex_conf.h 中宏配置, 分辨率则在 version.h 内设置
 * 1 => SYNWIT 应用例程 - 480  *  272 晶力泰
 * 2 => SYNWIT 应用例程 - 480  *  480 方形屏(TL040WVS03) 圆形屏(TL021WC02_Round) 
 * 3 => SYNWIT 应用例程 - 800  *  480 晶力泰 
 * 4 => SimplLCD_RGB 例程在 LVGL 下实现 - 不依赖于任何 UI 素材, 单纯显示颜色/边界测试
 */

#if 		(0 == __RESOLUTION_TARGET__)

#define LCD_HDOT		(480)		// 水平点数
#define LCD_VDOT		(272)		// 垂直点数

#define LCD_TYPE			0//仅针对 480*480 屏有效
/* enum : TFT-LCD 型号
 * 480  *  480 方形屏(TL040WVS03) 圆形屏(TL021WC02_Round) 
 * 0 => TL040WVS03
 * 1 => TL021WC02_Round
 */

#elif 		(1 == __RESOLUTION_TARGET__)

#define LCD_HDOT		(480)	// 水平点数
#define LCD_VDOT		(272)	// 垂直点数


#elif 		(2 == __RESOLUTION_TARGET__)

#define LCD_HDOT		(480)	// 水平点数
#define LCD_VDOT		(480)	// 垂直点数

#define LCD_TYPE			0
/* enum : TFT-LCD 型号
 * 0 => TL040WVS03
 * 1 => TL021WC02_Round
 */
 
#elif 		(3 == __RESOLUTION_TARGET__)

#define LCD_HDOT		(800)	// 水平点数
#define LCD_VDOT		(480)	// 垂直点数


#elif 		(4 == __RESOLUTION_TARGET__)

#define LCD_HDOT		(480)		// 水平点数
#define LCD_VDOT		(272)		// 垂直点数

#define LCD_TYPE			0//仅针对 480*480 屏有效
/* enum : TFT-LCD 型号
 * 480  *  480 方形屏(TL040WVS03) 圆形屏(TL021WC02_Round) 
 * 0 => TL040WVS03
 * 1 => TL021WC02_Round
 */

#else 


#endif //__RESOLUTION_TARGET__

/* #define GUI_VERSION			612 //GUI版本直接分开工程以示区别
 * enum : LVGL 版本控制
 * 612 => lvgl-V6.1.2
 * 711 => lvgl-V7.11.0
 * 820 => lvgl-V8.2.0
 */

#endif //__VERSION_H__
