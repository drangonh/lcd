//----------------------------头文件依赖----------------------------//
#include "TP_FT6336.h"

/* Tips : 本驱动例程为触摸 IC : FT6336 */

//----------------------------本地宏定义----------------------------//
#define FT6336_ADDRESS 			0x90
#define TD_STAT_ADDR 			0x2
#define TT_MODE_BUFFER_INVALID 	0x08
#define TD_STAT_NUMBER_TOUCH 	0x07

#define TOUCH1_XH_ADDR 			0x03
#define TOUCH2_XH_ADDR 			0x09
#define TOUCH3_XH_ADDR 			0x0F
#define TOUCH4_XH_ADDR 			0x15

//----------------------------本地全局变量----------------------------//
static const uint8_t FT6336_X_Base[4] = {TOUCH1_XH_ADDR, TOUCH2_XH_ADDR, TOUCH3_XH_ADDR, TOUCH4_XH_ADDR};

//----------------------------本地函数定义----------------------------//
static inline void delay(void)
{
    for (uint32_t i = 0; i < 800; i++)
        __NOP();
}

// TP_IC 写入寄存器
static uint32_t touch_write_regs(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint32_t i;
    uint8_t ack;

    ack = I2C_Start(I2C0, FT6336_ADDRESS | 0);
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

    ack = I2C_Start(I2C0, FT6336_ADDRESS | 0);
    if (ack == 0)
        goto rd_fail;

    ack = I2C_Write(I2C0, reg & 0XFF);
    if (ack == 0)
        goto rd_fail;

    delay();//此处必须延时等待一会再启动

    ack = I2C_Start(I2C0, FT6336_ADDRESS | 1); //ReStart
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

static void FT6336_GetTpOnePoint(const uint8_t x_base, tp_dev_t *point, uint8_t *tp_stu)
{
	uint8_t values[4] = {0};
    touch_read_regs(x_base, values, 4);
    if (x_base == 0x3)
        *tp_stu = (values[0] >> 6) & 0x3;

    point->x[0] = (((uint16_t)(values[0] & 0x0f)) << 8) | values[1];
    point->y[0] = (((uint16_t)(values[2] & 0x0f)) << 8) | values[3];
}

//----------------------------对外接口----------------------------//
/**
  \brief		TP_IC 读取触摸点
  \param [in]	tp_dev_t 
  \return       0 : success    非0: error
  \note    		\
 */
uint8_t ft6336_read_points(tp_dev_t *tp_dev)
{
    uint8_t i = 0;
    uint8_t temp_value;
    uint8_t points_num;
	
    tp_dev_t temp_point;
    int Tp_Point_x = 0;
    int Tp_Point_y = 0;
	
    do
    { //make sure data in buffer is valid
        touch_read_regs(TD_STAT_ADDR, &temp_value, 1);
        if (i++ == 0x30)
        {
            tp_dev->status &= ~TP_PRES_DOWN;
            return 1;
        }
    } while (temp_value & TT_MODE_BUFFER_INVALID);

    touch_read_regs(TD_STAT_ADDR, &temp_value, 1);
    points_num = (uint8_t)(temp_value & TD_STAT_NUMBER_TOUCH);
    if ((points_num == 0) || (points_num > 5))
        points_num = 1;
	
    for (i = 0; i < points_num; i++)
    {
        FT6336_GetTpOnePoint(FT6336_X_Base[i], &temp_point, &temp_point.status);
        if (1 == temp_point.status || (temp_point.x[0] > LV_HOR_RES_MAX) || (temp_point.y[0] > LV_VER_RES_MAX))
        {
            tp_dev->status &= ~TP_PRES_DOWN;
            return 1;
        }
        Tp_Point_x += temp_point.x[0];
        Tp_Point_y += temp_point.y[0];
    }

    tp_dev->x[0] = Tp_Point_x / points_num;
    tp_dev->y[0] = Tp_Point_y / points_num;
    tp_dev->status = TP_PRES_DOWN;
    return 0;
}

/**
  \brief		TP_IC 初始化
  \param [in]	\
  \return       0 : success    非0: error
  \note    		\
 */
uint8_t ft6336_init(void)
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
	
    if ((temp_value == 0x06) || (temp_value == 0x36) || (temp_value == 0x64))
    {
        //
    }
    swm_delay_ms(10);

    return 0;
}
