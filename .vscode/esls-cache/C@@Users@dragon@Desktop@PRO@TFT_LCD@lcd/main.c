#include "SWM320.h"

int main(void)
{	
	__NOP();
        
	
	SystemInit();
	
	GPIO_Init(GPIOM, PIN16, 0, 1, 0);			//输入，上拉使能，接KEY
	
	GPIO_Init(GPIOM, PIN18, 1, 0, 0);			//输出， 接LED
	
	while(1==1)
	{
		if(GPIO_GetBit(GPIOM, PIN16) == 0)		//按键被按下
		{
			GPIO_SetBit(GPIOM, PIN18);
		}
		else
		{
			GPIO_ClrBit(GPIOM, PIN18);
		}
	}
}

