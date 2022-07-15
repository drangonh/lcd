#include "SWM320.h"
#include "main.h"
//----------------------------变量声明定义----------------------------//
volatile uint32_t Tick_Time = 0; //systick 心跳
volatile uint32_t TIMR0_Time = 0;

int main(void)
{	
	for (int i = 0; i < 666666; ++i) //在调测阶段, 预防程序跑飞后, 上电无法读取 MCU 核
        __NOP();
        
	uint32_t i;
	char str_Hi[] = "Hi from Synwit\r\n";
	
	SystemInit();
	
	serial_init();
   	
	printf("test\n");
	//UART_SendChars(str_Hi, strlen(str_Hi));
	while(1==1)
	{
		//UART_SendChars(str_Hi, strlen(str_Hi));
		
		for(i = 0; i < 10000000; i++) __NOP();
	}
}

