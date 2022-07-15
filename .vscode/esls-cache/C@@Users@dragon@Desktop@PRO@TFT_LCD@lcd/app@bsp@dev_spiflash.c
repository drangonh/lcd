//----------------------------头文件依赖----------------------------//
#include "dev_spiflash.h"

//----------------------------本地宏定义----------------------------//
#define SPI_PORT_W25X 				SPI0

//----------------------------本地函数定义----------------------------//
static uint8_t spi_flash_is_busy(void);
static void spi_flash_waitfor_write_end(void);
static void spi_flash_write_enable(void);
static uint32_t spi_flash_read_id_device(void);
static uint32_t spi_flash_read_id(void);

//----------------------------函数定义----------------------------//
int spi_flash_init(void)
{
	SPI_InitStructure SPI_initStruct;

	GPIO_Init(SPI_PORT_CS, SPI_PIN_CS, 1, 0, 0); // 输出，接CS
	SPI_CS_SET();

	PORT_Init(SPI_PORT_SCLK, SPI_PIN_SCLK, SPI_FUNMUX_SCKL, 0);
	PORT_Init(SPI_PORT_MOSI, SPI_PIN_MOSI, SPI_FUNMUX_MOSI, 0);
	PORT_Init(SPI_PORT_MISO, SPI_PIN_MISO, SPI_FUNMUX_MISO, 1);

	SPI_initStruct.clkDiv = SPI_CLKDIV_4;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_FIRST_EDGE;
	SPI_initStruct.IdleLevel = SPI_LOW_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXHFullIEn = 0;
	SPI_initStruct.TXEmptyIEn = 0;
	SPI_initStruct.TXCompleteIEn = 0;
	SPI_Init(SPI_PORT_W25X, &SPI_initStruct);
	SPI_Open(SPI_PORT_W25X);

	uint32_t id = 0;
	/*
	uint32_t id_device = spi_flash_read_id_device();
	uint32_t id_flash = spi_flash_read_id();
	printf("id_device = 0x[%x], id_flash = 0x[%x]\r\n", id_device, id_flash);
	//( SPI_FLASH_ID ) ? 0 : 1;
	*/

	return id;
}

/**
 * @brief  擦除FLASH扇区-以4K对齐擦除
 * @param  addr - 要擦除的扇区地址
 * @retval 无
 */
void spi_flash_erase_sector(uint32_t addr)
{
	spi_flash_write_enable(); /* 发送FLASH写使能命令 */
	/* 擦除扇区 */
	SPI_CS_CLR();										   /* 通讯开始：CS低 */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_SectorErase);		   /* 发送扇区擦除指令*/
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF0000) >> 16); /*发送擦除扇区地址的高位*/
	/* 发送擦除扇区地址的中位 */
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF00) >> 8);
	/* 发送擦除扇区地址的低位 */
	SPI_ReadWrite(SPI_PORT_W25X, addr & 0xFF);
	SPI_CS_SET(); /*通讯结束：CS高 */
	/* 等待擦除完毕*/
	// spi_flash_waitfor_write_end();
	while (spi_flash_is_busy())
		;
}

void spi_flash_erase_chip(void)
{
	spi_flash_write_enable(); //发送FLASH写使能命令
	/* 擦除扇区 */
	SPI_CS_CLR();								  //通讯开始：CS低
	SPI_ReadWrite(SPI_PORT_W25X, W25X_ChipErase); //发送 全部 扇区擦除指令
	SPI_CS_SET();								  //通讯结束：CS高

	while (spi_flash_is_busy())
		;
}

/**
 * @brief  读取FLASH数据
 * @param 	pBuffer，存储读出数据的指针
 * @param   ReadAddr，读取地址
 * @param   NumByteToRead，读取数据长度
 * @retval 无
 */
void spi_flash_read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPI_CS_CLR();								 /* 通讯开始：CS低 */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_ReadData); /* 发送 读 指令 */

	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF0000) >> 16); /* 发送 读 地址高位 */
	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF00) >> 8);	   /* 发送 读 地址中位 */
	SPI_ReadWrite(SPI_PORT_W25X, ReadAddr & 0xFF);			   /* 发送 读 地址低位 */

	while (NumByteToRead--) /* 读取数据 */
	{
		*pBuffer = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte); /* 读取一个字节*/
		pBuffer++;											 /* 指向下一个字节缓冲区 */
	}

	SPI_CS_SET(); /*通讯结束：CS高 */
}

/**
 * @brief  快速读取FLASH数据
 * @param 	pBuffer，存储读出数据的指针
 * @param   ReadAddr，读取地址
 * @param   NumByteToRead，读取数据长度
 * @retval 无
 */
void spi_flash_read_fast(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPI_CS_CLR();									 /* 通讯开始：CS低 */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_FastReadData); /* 发送 读 指令 */

	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF0000) >> 16); /* 发送 读 地址高位 */
	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF00) >> 8);	   /* 发送 读 地址中位 */
	SPI_ReadWrite(SPI_PORT_W25X, ReadAddr & 0xFF);			   /* 发送 读 地址低位 */
	SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);

	while (NumByteToRead--) /* 读取数据 */
	{
		*pBuffer = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte); /* 读取一个字节*/
		pBuffer++;											 /* 指向下一个字节缓冲区 */
	}

	SPI_CS_SET(); /*通讯结束：CS高 */
}

/**
 * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
 * @param  buff  - 要写入数据的指针
 * @param  bytes - 写入地址
 * @param  bytes - 写入数据长度，必须小于等于 SPI_FLASH_PerWritePageSize
 * @retval 无
 */
void spi_flash_write_page(uint8_t *buff, uint32_t addr, uint16_t bytes)
{
	/* 发送FLASH写使能命令 */
	spi_flash_write_enable();

	SPI_CS_CLR(); /* 通讯开始：CS低 */
	/* 写页写指令*/
	SPI_ReadWrite(SPI_PORT_W25X, W25X_PageProgram);
	/*发送写地址的高位*/
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF0000) >> 16);
	/*发送写地址的中位*/
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF00) >> 8);
	/*发送写地址的低位*/
	SPI_ReadWrite(SPI_PORT_W25X, addr & 0xFF);

	/*
	if(bytes > SPI_FLASH_PerWritePageSize)
	{
		bytes = SPI_FLASH_PerWritePageSize;
		printf("spi_flash_write_page too large! \r\n");
	}
	*/

	/* 写入数据*/
	while (bytes--)
	{
		SPI_ReadWrite(SPI_PORT_W25X, *buff); /* 发送当前要写入的字节数据 */
		buff++;								 /* 指向下一字节数据 */
	}

	SPI_CS_SET(); /*通讯结束：CS高 */

	// spi_flash_waitfor_write_end();
	while (spi_flash_is_busy())
		;
}

/**
 * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pBuffer，要写入数据的指针
 * @param  WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度
 * @retval 无
 */
void spi_flash_write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PAGE_SIZE;			   /*mod运算求余，若writeAddr是SPI_FLASH_PAGE_SIZE整数倍，运算结果Addr值为0*/
	count = SPI_FLASH_PAGE_SIZE - Addr;				   /*差count个数据值，刚好可以对齐到页地址*/
	NumOfPage = NumByteToWrite / SPI_FLASH_PAGE_SIZE;   /*计算出要写多少整数页*/
	NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_SIZE; /*mod运算求余，计算出剩余不满一页的字节数*/

	if (Addr == 0) /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
	{
		if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PAGE_SIZE */
		{
			spi_flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
		}
		else /* NumByteToWrite > SPI_FLASH_PAGE_SIZE */
		{
			while (NumOfPage--) /*先把整数页都写了*/
			{
				spi_flash_write_page(pBuffer, WriteAddr, SPI_FLASH_PAGE_SIZE);
				WriteAddr += SPI_FLASH_PAGE_SIZE;
				pBuffer += SPI_FLASH_PAGE_SIZE;
			}

			/*若有多余的不满一页的数据，把它写完*/
			spi_flash_write_page(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	/* 若地址与 SPI_FLASH_PAGE_SIZE 不对齐  */
	else
	{
		/* NumByteToWrite < SPI_FLASH_PAGE_SIZE */
		if (NumOfPage == 0)
		{
			/*当前页剩余的count个位置比NumOfSingle小，写不完*/
			if (NumOfSingle > count)
			{
				temp = NumOfSingle - count;

				/*先写满当前页*/
				spi_flash_write_page(pBuffer, WriteAddr, count);
				WriteAddr += count;
				pBuffer += count;

				/*再写剩余的数据*/
				spi_flash_write_page(pBuffer, WriteAddr, temp);
			}
			else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
			{
				spi_flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else /* NumByteToWrite > SPI_FLASH_PAGE_SIZE */
		{
			/*地址不对齐多出的count分开处理，不加入这个运算*/
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / SPI_FLASH_PAGE_SIZE;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_SIZE;

			spi_flash_write_page(pBuffer, WriteAddr, count);
			WriteAddr += count;
			pBuffer += count;

			/*把整数页都写了*/
			while (NumOfPage--)
			{
				spi_flash_write_page(pBuffer, WriteAddr, SPI_FLASH_PAGE_SIZE);
				WriteAddr += SPI_FLASH_PAGE_SIZE;
				pBuffer += SPI_FLASH_PAGE_SIZE;
			}
			/*若有多余的不满一页的数据，把它写完*/
			if (NumOfSingle != 0)
			{
				spi_flash_write_page(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

//进入掉电模式
void spi_flash_power_down(void)
{
	SPI_CS_CLR();								  /* 通讯开始：CS低 */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_PowerDown); /* 发送 掉电 命令 */
	SPI_CS_SET();								  /*通讯结束：CS高 */
}

void spi_flash_wakeup(void) //唤醒SPI_FLASH
{
	/*选择 FLASH: CS 低 */
	SPI_CS_CLR();

	/* 发上 上电 命令 */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_ReleasePowerDown);

	/* 停止信号 FLASH: CS 高 */
	SPI_CS_SET(); //等待TRES1
}

//----------------------------本地函数定义----------------------------//
static uint8_t spi_flash_is_busy(void);
static void spi_flash_waitfor_write_end(void);
static void spi_flash_write_enable(void);
static uint32_t spi_flash_read_id_device(void);
static uint32_t spi_flash_read_id(void);

/**
 * @brief  芯片忙检测，执行擦除、写入操作后需要执行忙检测以判断芯片是否已经完成操作
 * @param  none
 * @retval 1 芯片正在执行内部擦除、写入操作    0 芯片未在执行内部操作
 */
uint8_t spi_flash_is_busy(void)
{
	uint16_t FLASH_Status = 0;

	SPI_CS_CLR();

	SPI_ReadWrite(SPI_PORT_W25X, W25X_ReadStatusReg_1);
	FLASH_Status = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);

	SPI_CS_SET();

	return ((FLASH_Status & WIP_Flag) == 0x01) ? 1 : 0;
}

/**
 * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
 * @param  none
 * @retval none
 */
void spi_flash_waitfor_write_end(void)
{
	uint16_t FLASH_Status;

	SPI_CS_CLR();

	SPI_ReadWrite(SPI_PORT_W25X, W25X_ReadStatusReg_1);

	/* Loop as long as the memory is busy with a write cycle */
	do
	{
		/* Send a dummy byte to generate the clock needed by the FLASH
		and put the value of the status register in FLASH_Status variable */
		FLASH_Status = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	} while ((FLASH_Status & WIP_Flag) == 0x01); /* Write in progress */

	SPI_CS_SET();
}

/**
 * @brief  向FLASH发送 写使能 命令
 * @param  none
 * @retval none
 */
void spi_flash_write_enable(void)
{
	SPI_CS_CLR();									/* 通讯开始：CS低 */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_WriteEnable); /* 发送写使能命令*/
	SPI_CS_SET();									/*通讯结束：CS高 */
}

/**
 * @brief  读取 FLASH Device ID
 * @param  无
 * @retval FLASH Device ID
 */
uint32_t spi_flash_read_id_device(void)
{
	uint32_t Temp = 0;
	SPI_CS_CLR();
	/* Send "RDID " instruction */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_DeviceID);
	SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);

	/* Read a byte from the FLASH */
	Temp = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	SPI_CS_SET();
	return Temp;
}

/**
 * @brief  读取FLASH ID
 * @param  无
 * @retval FLASH ID
 */
uint32_t spi_flash_read_id(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	SPI_CS_CLR();
	/* 发送JEDEC指令，读取ID */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_JedecDeviceID);
	Temp0 = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte); /* 读取一个字节数据 */
	Temp1 = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	Temp2 = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	SPI_CS_SET();
	/*把数据组合起来，作为函数的返回值*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}
