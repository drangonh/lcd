//----------------------------头文件依赖----------------------------//
#include "TP_CST826.h"

/* Tips : 本驱动例程为触摸 IC : CST826 */

//----------------------------本地宏定义----------------------------//
#define CST826_ADDRESS 			0x2A
#define TD_STAT_ADDR 			0x2
#define TT_MODE_BUFFER_INVALID 	0x08
#define TD_STAT_NUMBER_TOUCH 	0x07

//----------------------------本地函数定义----------------------------//
static inline void delay(void)
{
    for (uint32_t i = 0; i < 1000; i++)
        __NOP();
}

// TP_IC 写入寄存器
static uint32_t touch_write_regs(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint32_t i;
    uint8_t ack;

    ack = I2C_Start(I2C0, CST826_ADDRESS | 0);
    if (ack == 0)
        goto wr_fail;

    ack = I2C_Write(I2C0, reg & 0XFF);
    if (ack == 0)
        goto wr_fail;

    for (i = 0; i < len; i++)
    {
        ack = I2C_Write(I2C0, buf[i]);
        if (ack == 0)
            goto wr_fail;
    }

    I2C_Stop(I2C0);
    delay();
    return 0;

wr_fail:
    I2C_Stop(I2C0);
    delay();
    return 1;
}

// TP_IC 读取寄存器
static uint32_t touch_read_regs(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ack;

    ack = I2C_Start(I2C0, CST826_ADDRESS | 0);
    if (ack == 0)
        goto rd_fail;

    ack = I2C_Write(I2C0, reg & 0XFF);
    if (ack == 0)
        goto rd_fail;

    delay();//此处必须延时等待一会再启动

    ack = I2C_Start(I2C0, CST826_ADDRESS | 1); //ReStart
    if (ack == 0)
        goto rd_fail;

    for (i = 0; i < len - 1; i++)
    {
        buf[i] = I2C_Read(I2C0, 1);
    }
    buf[i] = I2C_Read(I2C0, 0);

    I2C_Stop(I2C0);
    delay();
    return 0;

rd_fail:
    I2C_Stop(I2C0);
    delay();
    return 1;
}

//----------------------------对外接口----------------------------//
/**
  \brief		TP_IC 读取触摸点
  \param [in]	tp_dev_t 
  \return       0 : success    非0: error
  \note    		\
 */
uint8_t cst826_read_points(tp_dev_t *tp_dev)
{
    uint8_t tp_value[5] = {0};
    
    touch_read_regs(TD_STAT_ADDR, tp_value, 5);
    if ((tp_value[0] == 1) || (tp_value[0] == 2))
    {
        tp_dev->x[0] = (((uint16_t)(tp_value[1]&0x0f))<<8) | tp_value[2]; //得到X
        tp_dev->y[0] = (((uint16_t)(tp_value[3]&0x0f))<<8) | tp_value[4]; //得到y
		if ((tp_dev->x[0] > LV_HOR_RES_MAX) || (tp_dev->y[0] > LV_VER_RES_MAX))
		{
			tp_dev->status &= ~TP_PRES_DOWN;
			tp_dev->x[0] = 0;
			tp_dev->y[0] = 0;
			return 0;
		}
		tp_dev->status = TP_PRES_DOWN;
        return 0;
    }
    else if(tp_value[0] == 0)//校验失败
    {
        tp_dev->x[0] = 0;
        tp_dev->y[0] = 0;
        tp_dev->status &= ~TP_PRES_DOWN;
    }
    return 1;
}

/**
  \brief		TP_IC 初始化
  \param [in]	\
  \return       0 : success    非0: error
  \note    		\
 */
uint8_t cst826_init(void)
{
    uint8_t i = 0;
    uint8_t temp_value = 0;
    I2C_InitStructure I2C_initStruct;

    PORT_Init(TP_PORT_SCL, TP_PIN_SCL, TP_PIN_SCL_FUN, 1); //配置为 I2C0 SCL 引脚
    PORT_Init(TP_PORT_SDA, TP_PIN_SDA, TP_PIN_SDA_FUN, 1); //配置为 I2C0 SDA 引脚
    PORT->PORTA_PULLU |= (1 << TP_PIN_SCL);//必须使能上拉，用于模拟开漏
    PORT->PORTA_PULLU |= (1 << TP_PIN_SDA);
	
    I2C_initStruct.Master = 1;
    I2C_initStruct.Addr7b = 1;
    I2C_initStruct.MstClk = 400000;
    I2C_initStruct.MstIEn = 0;
    I2C_Init(I2C0, &I2C_initStruct);
    I2C_Open(I2C0);

    GPIO_Init(TP_GPIO_INT, TP_PIN_INT, 1, 0, 1);

    GPIO_Init(TP_GPIO_RST, TP_PIN_RST, 1, 1, 0); //复位
    GPIO_ClrBit(TP_GPIO_RST, TP_PIN_RST);
    swm_delay_ms(10);
    GPIO_SetBit(TP_GPIO_RST, TP_PIN_RST); // 释放复位
    swm_delay_ms(10);

    do
    {
        touch_read_regs(0xa3, &temp_value, 1);
        i++;
        swm_delay_ms(50);
        //printf("Touch IC ID:%d\r\n", temp_value);
        if ((temp_value == 0x06) || (temp_value == 0x36) || (temp_value == 0x64))
            break;
    } while (i < 20);
	
    swm_delay_ms(10);

    return 0;
}
