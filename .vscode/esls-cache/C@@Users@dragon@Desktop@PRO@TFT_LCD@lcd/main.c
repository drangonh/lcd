#include "SWM320.h"

int main(void)
{	
	__NOP();
        
	
	SystemInit();
	
	GPIO_Init(GPIOM, PIN16, 0, 1, 0);			//���룬����ʹ�ܣ���KEY
	
	GPIO_Init(GPIOM, PIN18, 1, 0, 0);			//����� ��LED
	
	while(1==1)
	{
		if(GPIO_GetBit(GPIOM, PIN16) == 0)		//����������
		{
			GPIO_SetBit(GPIOM, PIN18);
		}
		else
		{
			GPIO_ClrBit(GPIOM, PIN18);
		}
	}
}

