#include "SWM320.h"
#include "main.h"
//----------------------------������������----------------------------//
volatile uint32_t Tick_Time = 0; //systick ����
volatile uint32_t TIMR0_Time = 0;

int main(void)
{	
	for (int i = 0; i < 666666; ++i) //�ڵ���׶�, Ԥ�������ܷɺ�, �ϵ��޷���ȡ MCU ��
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

