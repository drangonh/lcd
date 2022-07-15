#ifndef __DEV_TOUCH_H__
#define __DEV_TOUCH_H__

//----------------------------ͷ�ļ�����----------------------------//
#include "main.h"

//----------------------------�궨��----------------------------//
//�˿�����
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


#define TP_PRES_DOWN 		(0x80) //����������
#define TP_CATH_PRES 		(0x40) //�а�������
#define TP_MAX_TOUCH 		(5)    //������֧�ֵĵ���,�̶�Ϊ5��

//----------------------------��������----------------------------//
//������������
typedef struct
{
    uint16_t x[TP_MAX_TOUCH]; //��ǰ����
    uint16_t y[TP_MAX_TOUCH]; //�����������5������
    uint8_t status;           //�ʵ�״̬
                              //b7:����1/�ɿ�0;
                              //b6~b5:����
                              //b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
} tp_dev_t;

//----------------------------�ⲿ����----------------------------//
extern tp_dev_t TP_Dev;

//----------------------------��������----------------------------//
void touch_ic_init(void);
void touch_read_points(void);

#endif //__DEV_TOUCH_H__
