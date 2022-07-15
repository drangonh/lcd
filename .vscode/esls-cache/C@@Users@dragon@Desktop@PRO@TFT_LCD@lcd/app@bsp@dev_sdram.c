//----------------------------头文件依赖----------------------------//
#include "dev_sdram.h"

//----------------------------函数定义----------------------------//
void sdram_init(void)
{
    SDRAM_InitStructure SDRAM_InitStruct;

    PORT->PORTP_SEL0 = 0xAAAAAAAA; //PP0-23 => ADDR0-23
    PORT->PORTP_SEL1 &= ~0x00000F0F;
    PORT->PORTP_SEL1 |= 0x00000A0A;

    PORT->PORTM_SEL0 = 0xAAAAAAAA; //PM0-15 => DATA15-0
    PORT->PORTM_INEN = 0xFFFF;
    //PM16 => OEN,PM17 => WEN,PM18 => NORFL_CSN,PM19 => SDRAM_CSN,PM20 => SRAM_CSN,PM21 => SDRAM_CKE
    PORT->PORTM_SEL1 = 0x888;
	
    /* 特别说明：SDRAM 为内置叠封, 此处 SDRAM 初始化 个别 IO 并无实际使用，
	数据手册上封装引出的 IO 均未被内部 SDRAM 使用，不会造成 IO 被冲突占用, 换言之, 芯片封装引出的引脚皆可正常使用
	用户在调用本函数后，再次初始化 IO 便可正常使用 (GPIO_Init) */
	
    SDRAM_InitStruct.CellSize = SDRAM_CELLSIZE_64Mb;
    SDRAM_InitStruct.CellBank = SDRAM_CELLBANK_4;
    SDRAM_InitStruct.CellWidth = SDRAM_CELLWIDTH_16;
    //SDRAM_InitStruct.CASLatency = SDRAM_CASLATENCY_2;
    SDRAM_InitStruct.TimeTMRD = SDRAM_TMRD_3;
    SDRAM_InitStruct.TimeTRRD = SDRAM_TRRD_2;
    SDRAM_InitStruct.TimeTRAS = SDRAM_TRAS_6;
    SDRAM_InitStruct.TimeTRC = SDRAM_TRC_8;
    SDRAM_InitStruct.TimeTRCD = SDRAM_TRCD_3;
    SDRAM_InitStruct.TimeTRP = SDRAM_TRP_3;
    SDRAM_Init(&SDRAM_InitStruct);
}
