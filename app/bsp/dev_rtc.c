//----------------------------头文件依赖----------------------------//
#include "dev_rtc.h"

//----------------------------函数定义----------------------------//
void rtc_init(void)
{
    RTC_InitStructure RTC_initStruct;

    RTC_initStruct.Year = 2022;
    RTC_initStruct.Month = 10;
    RTC_initStruct.Date = 24;
    RTC_initStruct.Hour = 10;
    RTC_initStruct.Minute = 24;
    RTC_initStruct.Second = 10;
    RTC_initStruct.SecondIEn = 0;
    RTC_initStruct.MinuteIEn = 0;
    RTC_Init(RTC, &RTC_initStruct);
	
    RTC_Start(RTC);
}
