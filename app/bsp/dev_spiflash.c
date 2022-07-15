//----------------------------ͷ�ļ�����----------------------------//
#include "dev_spiflash.h"

//----------------------------���غ궨��----------------------------//
#define SPI_PORT_W25X 				SPI0

//----------------------------���غ�������----------------------------//
static uint8_t spi_flash_is_busy(void);
static void spi_flash_waitfor_write_end(void);
static void spi_flash_write_enable(void);
static uint32_t spi_flash_read_id_device(void);
static uint32_t spi_flash_read_id(void);

//----------------------------��������----------------------------//
int spi_flash_init(void)
{
	SPI_InitStructure SPI_initStruct;

	GPIO_Init(SPI_PORT_CS, SPI_PIN_CS, 1, 0, 0); // �������CS
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
 * @brief  ����FLASH����-��4K�������
 * @param  addr - Ҫ������������ַ
 * @retval ��
 */
void spi_flash_erase_sector(uint32_t addr)
{
	spi_flash_write_enable(); /* ����FLASHдʹ������ */
	/* �������� */
	SPI_CS_CLR();										   /* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_SectorErase);		   /* ������������ָ��*/
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF0000) >> 16); /*���Ͳ���������ַ�ĸ�λ*/
	/* ���Ͳ���������ַ����λ */
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF00) >> 8);
	/* ���Ͳ���������ַ�ĵ�λ */
	SPI_ReadWrite(SPI_PORT_W25X, addr & 0xFF);
	SPI_CS_SET(); /*ͨѶ������CS�� */
	/* �ȴ��������*/
	// spi_flash_waitfor_write_end();
	while (spi_flash_is_busy())
		;
}

void spi_flash_erase_chip(void)
{
	spi_flash_write_enable(); //����FLASHдʹ������
	/* �������� */
	SPI_CS_CLR();								  //ͨѶ��ʼ��CS��
	SPI_ReadWrite(SPI_PORT_W25X, W25X_ChipErase); //���� ȫ�� ��������ָ��
	SPI_CS_SET();								  //ͨѶ������CS��

	while (spi_flash_is_busy())
		;
}

/**
 * @brief  ��ȡFLASH����
 * @param 	pBuffer���洢�������ݵ�ָ��
 * @param   ReadAddr����ȡ��ַ
 * @param   NumByteToRead����ȡ���ݳ���
 * @retval ��
 */
void spi_flash_read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPI_CS_CLR();								 /* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_ReadData); /* ���� �� ָ�� */

	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF0000) >> 16); /* ���� �� ��ַ��λ */
	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF00) >> 8);	   /* ���� �� ��ַ��λ */
	SPI_ReadWrite(SPI_PORT_W25X, ReadAddr & 0xFF);			   /* ���� �� ��ַ��λ */

	while (NumByteToRead--) /* ��ȡ���� */
	{
		*pBuffer = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte); /* ��ȡһ���ֽ�*/
		pBuffer++;											 /* ָ����һ���ֽڻ����� */
	}

	SPI_CS_SET(); /*ͨѶ������CS�� */
}

/**
 * @brief  ���ٶ�ȡFLASH����
 * @param 	pBuffer���洢�������ݵ�ָ��
 * @param   ReadAddr����ȡ��ַ
 * @param   NumByteToRead����ȡ���ݳ���
 * @retval ��
 */
void spi_flash_read_fast(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SPI_CS_CLR();									 /* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_FastReadData); /* ���� �� ָ�� */

	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF0000) >> 16); /* ���� �� ��ַ��λ */
	SPI_ReadWrite(SPI_PORT_W25X, (ReadAddr & 0xFF00) >> 8);	   /* ���� �� ��ַ��λ */
	SPI_ReadWrite(SPI_PORT_W25X, ReadAddr & 0xFF);			   /* ���� �� ��ַ��λ */
	SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);

	while (NumByteToRead--) /* ��ȡ���� */
	{
		*pBuffer = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte); /* ��ȡһ���ֽ�*/
		pBuffer++;											 /* ָ����һ���ֽڻ����� */
	}

	SPI_CS_SET(); /*ͨѶ������CS�� */
}

/**
 * @brief  ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
 * @param  buff  - Ҫд�����ݵ�ָ��
 * @param  bytes - д���ַ
 * @param  bytes - д�����ݳ��ȣ�����С�ڵ��� SPI_FLASH_PerWritePageSize
 * @retval ��
 */
void spi_flash_write_page(uint8_t *buff, uint32_t addr, uint16_t bytes)
{
	/* ����FLASHдʹ������ */
	spi_flash_write_enable();

	SPI_CS_CLR(); /* ͨѶ��ʼ��CS�� */
	/* дҳдָ��*/
	SPI_ReadWrite(SPI_PORT_W25X, W25X_PageProgram);
	/*����д��ַ�ĸ�λ*/
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF0000) >> 16);
	/*����д��ַ����λ*/
	SPI_ReadWrite(SPI_PORT_W25X, (addr & 0xFF00) >> 8);
	/*����д��ַ�ĵ�λ*/
	SPI_ReadWrite(SPI_PORT_W25X, addr & 0xFF);

	/*
	if(bytes > SPI_FLASH_PerWritePageSize)
	{
		bytes = SPI_FLASH_PerWritePageSize;
		printf("spi_flash_write_page too large! \r\n");
	}
	*/

	/* д������*/
	while (bytes--)
	{
		SPI_ReadWrite(SPI_PORT_W25X, *buff); /* ���͵�ǰҪд����ֽ����� */
		buff++;								 /* ָ����һ�ֽ����� */
	}

	SPI_CS_SET(); /*ͨѶ������CS�� */

	// spi_flash_waitfor_write_end();
	while (spi_flash_is_busy())
		;
}

/**
 * @brief  ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
 * @param	pBuffer��Ҫд�����ݵ�ָ��
 * @param  WriteAddr��д���ַ
 * @param  NumByteToWrite��д�����ݳ���
 * @retval ��
 */
void spi_flash_write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PAGE_SIZE;			   /*mod�������࣬��writeAddr��SPI_FLASH_PAGE_SIZE��������������AddrֵΪ0*/
	count = SPI_FLASH_PAGE_SIZE - Addr;				   /*��count������ֵ���պÿ��Զ��뵽ҳ��ַ*/
	NumOfPage = NumByteToWrite / SPI_FLASH_PAGE_SIZE;   /*�����Ҫд��������ҳ*/
	NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_SIZE; /*mod�������࣬�����ʣ�಻��һҳ���ֽ���*/

	if (Addr == 0) /* Addr=0,��WriteAddr �պð�ҳ���� aligned  */
	{
		if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PAGE_SIZE */
		{
			spi_flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
		}
		else /* NumByteToWrite > SPI_FLASH_PAGE_SIZE */
		{
			while (NumOfPage--) /*�Ȱ�����ҳ��д��*/
			{
				spi_flash_write_page(pBuffer, WriteAddr, SPI_FLASH_PAGE_SIZE);
				WriteAddr += SPI_FLASH_PAGE_SIZE;
				pBuffer += SPI_FLASH_PAGE_SIZE;
			}

			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
			spi_flash_write_page(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	/* ����ַ�� SPI_FLASH_PAGE_SIZE ������  */
	else
	{
		/* NumByteToWrite < SPI_FLASH_PAGE_SIZE */
		if (NumOfPage == 0)
		{
			/*��ǰҳʣ���count��λ�ñ�NumOfSingleС��д����*/
			if (NumOfSingle > count)
			{
				temp = NumOfSingle - count;

				/*��д����ǰҳ*/
				spi_flash_write_page(pBuffer, WriteAddr, count);
				WriteAddr += count;
				pBuffer += count;

				/*��дʣ�������*/
				spi_flash_write_page(pBuffer, WriteAddr, temp);
			}
			else /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
			{
				spi_flash_write_page(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else /* NumByteToWrite > SPI_FLASH_PAGE_SIZE */
		{
			/*��ַ����������count�ֿ������������������*/
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / SPI_FLASH_PAGE_SIZE;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_SIZE;

			spi_flash_write_page(pBuffer, WriteAddr, count);
			WriteAddr += count;
			pBuffer += count;

			/*������ҳ��д��*/
			while (NumOfPage--)
			{
				spi_flash_write_page(pBuffer, WriteAddr, SPI_FLASH_PAGE_SIZE);
				WriteAddr += SPI_FLASH_PAGE_SIZE;
				pBuffer += SPI_FLASH_PAGE_SIZE;
			}
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
			if (NumOfSingle != 0)
			{
				spi_flash_write_page(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

//�������ģʽ
void spi_flash_power_down(void)
{
	SPI_CS_CLR();								  /* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_PowerDown); /* ���� ���� ���� */
	SPI_CS_SET();								  /*ͨѶ������CS�� */
}

void spi_flash_wakeup(void) //����SPI_FLASH
{
	/*ѡ�� FLASH: CS �� */
	SPI_CS_CLR();

	/* ���� �ϵ� ���� */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_ReleasePowerDown);

	/* ֹͣ�ź� FLASH: CS �� */
	SPI_CS_SET(); //�ȴ�TRES1
}

//----------------------------���غ�������----------------------------//
static uint8_t spi_flash_is_busy(void);
static void spi_flash_waitfor_write_end(void);
static void spi_flash_write_enable(void);
static uint32_t spi_flash_read_id_device(void);
static uint32_t spi_flash_read_id(void);

/**
 * @brief  оƬæ��⣬ִ�в�����д���������Ҫִ��æ������ж�оƬ�Ƿ��Ѿ���ɲ���
 * @param  none
 * @retval 1 оƬ����ִ���ڲ�������д�����    0 оƬδ��ִ���ڲ�����
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
 * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
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
 * @brief  ��FLASH���� дʹ�� ����
 * @param  none
 * @retval none
 */
void spi_flash_write_enable(void)
{
	SPI_CS_CLR();									/* ͨѶ��ʼ��CS�� */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_WriteEnable); /* ����дʹ������*/
	SPI_CS_SET();									/*ͨѶ������CS�� */
}

/**
 * @brief  ��ȡ FLASH Device ID
 * @param  ��
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
 * @brief  ��ȡFLASH ID
 * @param  ��
 * @retval FLASH ID
 */
uint32_t spi_flash_read_id(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	SPI_CS_CLR();
	/* ����JEDECָ���ȡID */
	SPI_ReadWrite(SPI_PORT_W25X, W25X_JedecDeviceID);
	Temp0 = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte); /* ��ȡһ���ֽ����� */
	Temp1 = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	Temp2 = SPI_ReadWrite(SPI_PORT_W25X, Dummy_Byte);
	SPI_CS_SET();
	/*�����������������Ϊ�����ķ���ֵ*/
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}
