//----------------------------头文件依赖----------------------------//
#include "dev_lcd.h"

//----------------------------函数定义----------------------------//
void lcd_init(void)
{
    LCD_InitStructure LCD_initStruct;
	
	/* 复位
    GPIO_Init(GPIOM, PIN20, 1, 0, 0); 
    GPIO_ClrBit(GPIOM, PIN20);
    swm_delay(1);
    GPIO_SetBit(GPIOM, PIN20);
	*/
	
	//背光控制
    GPIO_Init(GPIOB, PIN12, 1, 0, 0); 
    GPIO_SetBit(GPIOB, PIN12);

    PORT->PORTN_SEL0 = 0xAAAAAAAA; //GPION.0~15  LCD_DATA0~15
    PORT->PORTN_SEL1 = 0xAA;

    // 根据 屏幕的 DataSheet 适配
#if (1 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 272 == LCD_VDOT) ) // 480*272
    LCD_initStruct.Hfp = 5;
    LCD_initStruct.Hbp = 43;
    LCD_initStruct.Vfp = 8;
    LCD_initStruct.Vbp = 8;
    LCD_initStruct.ClkDiv = LCD_CLKDIV_10; // 8 ~ 12 MHz (type: 9 MHz)
    LCD_initStruct.SamplEdge = LCD_SAMPLEDGE_FALL;
    LCD_initStruct.HsyncWidth = LCD_HSYNC_1DOTCLK;
	
#elif (2 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 480 == LCD_VDOT) ) // 480*480

	#if (0 == LCD_TYPE)
	#include "LCD_TL040WVS3.h"
	LCD_SPI_Init_TL040WVS3();// SPI初始化屏
	
	#elif (1 == LCD_TYPE)
	#include "LCD_TL021WC02.h"
	LCD_SPI_Init_TL021WC02();// SPI初始化屏
	
	#endif //LCD_TYPE
    
    LCD_initStruct.Hfp = 10;
    LCD_initStruct.Hbp = 30;
    LCD_initStruct.Vfp = 20;
    LCD_initStruct.Vbp = 20;
    LCD_initStruct.ClkDiv = LCD_CLKDIV_6;
    LCD_initStruct.SamplEdge = LCD_SAMPLEDGE_RISE;//LCD_SAMPLEDGE_FALL;
    LCD_initStruct.HsyncWidth = LCD_HSYNC_1DOTCLK;
	
#elif (3 == __RESOLUTION_TARGET__ || (800 == LCD_HDOT && 480 == LCD_VDOT) ) // 800*480
    LCD_initStruct.Hfp = 10;
    LCD_initStruct.Hbp = 80;
    LCD_initStruct.Vfp = 16;
    LCD_initStruct.Vbp = 16;
    LCD_initStruct.ClkDiv = LCD_CLKDIV_6;// 30 MHz
    LCD_initStruct.SamplEdge = LCD_SAMPLEDGE_FALL;
    LCD_initStruct.HsyncWidth = LCD_HSYNC_1DOTCLK;
	
#endif //__RESOLUTION_TARGET__
	
    LCD_initStruct.HnPixel = LV_HOR_RES_MAX;
    LCD_initStruct.VnPixel = LV_VER_RES_MAX;
	LCD_initStruct.ClkAlways = 0;
    LCD_initStruct.IntEOTEn = 1;
    LCD_Init(LCD, &LCD_initStruct);
}

//帧中断-可用于用户测量准确的实际帧率(两次中断的 systick_tick 间隔)
void LCD_Handler(void)
{
    LCD_INTClr(LCD);
    LCD_Start(LCD);
}
