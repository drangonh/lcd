//----------------------------ͷ�ļ�����----------------------------//
#include "dev_touch.h"

#if (1 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 272 == LCD_VDOT) ) // 480*272

#include "TP_GT911.h"

#elif (2 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 480 == LCD_VDOT) ) // 480*480

#if (0 == LCD_TYPE)
#include "TP_FT6336.h"

#elif (1 == LCD_TYPE)
#include "TP_CST826.h"

#endif //LCD_TYPE

#elif (3 == __RESOLUTION_TARGET__ || (800 == LCD_HDOT && 480 == LCD_VDOT) ) // 800*480

#include "TP_GT911.h"

#endif //__RESOLUTION_TARGET__

//----------------------------ȫ�ֱ���----------------------------//
tp_dev_t TP_Dev; //ʵ��������������
/* ���� tp_dev_t ������ TP_IC �ĳ���� */

//----------------------------��������----------------------------//
//����INT�жϷ�ʽ
void GPIOB7_Handler(void)
{
//    if (EXTI_State(TP_GPIO_INT, TP_PIN_INT))
    {
        EXTI_Clear(TP_GPIO_INT, TP_PIN_INT);
        touch_read_points();
    }
}

//��ȡ�����㺯��
void touch_read_points(void)
{
    uint8_t res = 0;

#if (1 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 272 == LCD_VDOT) ) // 480*272

    res = gt9x_read_points(&TP_Dev);

#elif (2 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 480 == LCD_VDOT) ) // 480*480

#if (0 == LCD_TYPE)
    res = ft6336_read_points(&TP_Dev);

#elif (1 == LCD_TYPE)
    res = cst826_read_points(&TP_Dev);

#endif //LCD_TYPE

#elif (3 == __RESOLUTION_TARGET__ || (800 == LCD_HDOT && 480 == LCD_VDOT) ) // 800*480

    res = gt9x_read_points(&TP_Dev);

#endif //__RESOLUTION_TARGET__
}

//����IC��ʼ������
void touch_ic_init(void)
{
    uint8_t res = 0;

#if (1 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 272 == LCD_VDOT) ) // 480*272

    res = gt9x_init();

#elif (2 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 480 == LCD_VDOT) ) // 480*480

#if (0 == LCD_TYPE)
    res = ft6336_init();

#elif (1 == LCD_TYPE)
    res = cst826_init();

#endif //LCD_TYPE

#elif (3 == __RESOLUTION_TARGET__ || (800 == LCD_HDOT && 480 == LCD_VDOT) ) // 800*480

    res = gt9x_init();

#endif //__RESOLUTION_TARGET__

#if 1 //�жϷ�ʽ
    GPIO_Init(TP_GPIO_INT, TP_PIN_INT, 0, 0, 1);
    EXTI_Init(TP_GPIO_INT, TP_PIN_INT, EXTI_FALL_EDGE); //�½��ش���
    NVIC_EnableIRQ(GPIOB7_IRQn);
    EXTI_Open(TP_GPIO_INT, TP_PIN_INT);
#endif
}
