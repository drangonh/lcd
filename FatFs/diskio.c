/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"		/* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */

#include "main.h"

#define MANUFACTURE_ID_W25Q128 	(0xEF4018)
#define FLASH_SECTOR_SIZE 		(4096)      // 4KB �ɲ�������
#define FLASH_BLOCK_SIZE 		(1024 << 6) // 64KB �ɲ�������
#define FLASH_SECTOR_COUNT 		(1024 << 2) //����FATfs������ٸ����� = ������(16MB) / ������С(4KB)
//( (16 * 1024 * 1024) / (FLASH_SECTOR_SIZE) )

/* Definitions of physical drive number for each drive */
#define DEV_MMC 		0 /* Example: Map MMC/SD card to physical drive 0 */
#define DEV_SPIFLASH 	1 /* Example: Map SPI FLASH to physical drive 1 */
/* �˴��̷�������Ӧ ffconf.h �� #define FF_VOLUME_STRS		"sd","spi" */

DSTATUS spiflash_stat = STA_NOINIT, mmc_stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv)
	{
	case DEV_SPIFLASH:
		stat = spiflash_stat;
		break;
	case DEV_MMC:
		stat = mmc_stat;
		break;
	}
	return stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv)
	{
	case DEV_SPIFLASH:
		// MANUFACTURE_ID_W25Q128
		spi_flash_init();// ID ��֤
		{
			spiflash_stat &= ~STA_NOINIT;
		}
		
		stat = spiflash_stat;
		break;

	case DEV_MMC:
		if (sdcard_init() == SD_RES_OK)
		{
			mmc_stat &= ~STA_NOINIT;
		}
		else
		{
			// while(1);
		}
		stat = mmc_stat;
		break;
	}
	return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	  /* Physical drive nmuber to identify the drive */
	BYTE *buff,	  /* Data buffer to store read data */
	LBA_t sector, /* Start sector in LBA */
	UINT count	  /* Number of sectors to read */
)
{
	DRESULT res = RES_PARERR;
	//BYTE *buff �˴�����(uint8_t * => uint32_t *)��ַ����, ���û��ⲿ����ʱ��ʽ��������
	switch (pdrv)
	{
	case DEV_SPIFLASH:
	{
		spi_flash_read(buff, sector * FLASH_SECTOR_SIZE, count * FLASH_SECTOR_SIZE);
		res = RES_OK;
		break;
	}

	case DEV_MMC:
	{
		uint32_t result = SD_RES_ERR;
		if (count == 1)
		{
			result = SDIO_BlockRead(sector, (uint32_t *)buff);
		}
		else
		{
			result = SDIO_MultiBlockRead(sector, count, (uint32_t *)buff);
		}
		res = (SD_RES_OK == result) ? RES_OK : RES_ERROR;
		break;
	}
	}
	return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
	BYTE pdrv,		  /* Physical drive nmuber to identify the drive */
	const BYTE *buff, /* Data to be written */
	LBA_t sector,	  /* Start sector in LBA */
	UINT count		  /* Number of sectors to write */
)
{
	DRESULT res = RES_PARERR;
	//BYTE *buff �˴�����(uint8_t * => uint32_t *)��ַ����, ���û��ⲿ����ʱ��ʽ��������
	switch (pdrv)
	{
	case DEV_SPIFLASH:
	{
		for (uint32_t i = 0; i < count; ++i)
		{
			spi_flash_erase_sector( ( sector + i ) * FLASH_SECTOR_SIZE );//�Ȳ�����д��
		}
		
		spi_flash_write((uint8_t *)buff, sector * FLASH_SECTOR_SIZE, count * FLASH_SECTOR_SIZE);
		res = RES_OK;
		break;
	}

	case DEV_MMC:
	{
		uint32_t result = SD_RES_ERR;
		if (count == 1)
		{
			result = SDIO_BlockWrite(sector, (uint32_t *)buff);
		}
		else
		{
			result = SDIO_MultiBlockWrite(sector, count, (uint32_t *)buff);
		}
		res = (SD_RES_OK == result) ? RES_OK : RES_ERROR;
		break;
	}
	}
	return res;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd,  /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;

	switch (pdrv)
	{
	case DEV_SPIFLASH:
		switch (cmd)
		{
		// Get R/W sector size (WORD)
		case GET_SECTOR_SIZE: // ������С
			*(WORD *)buff = FLASH_SECTOR_SIZE;
			break;

		// Get erase block size in unit of sector (DWORD)
		case GET_BLOCK_SIZE:	// �������С, δʹ��, һ��������Ϊ��Ԫ���в���
			*(DWORD *)buff = 1; // FLASH_BLOCK_SIZE;
			break;

		case GET_SECTOR_COUNT: // ����������
			*(DWORD *)buff = FLASH_SECTOR_COUNT;
			break;

		case CTRL_SYNC:
		default:
			break;
		}
		res = RES_OK;
		break;

	case DEV_MMC:
		switch (cmd)
		{
		// Get R/W sector size (WORD)
		case GET_SECTOR_SIZE:
			*(WORD *)buff = 512;
			break;

		// Get erase block size in unit of sector (DWORD)
		case GET_BLOCK_SIZE:
			*(DWORD *)buff = SD_cardInfo.CardBlockSize;
			break;

		case GET_SECTOR_COUNT:
			*(DWORD *)buff = SD_cardInfo.CardCapacity / 512;
			break;

		case CTRL_SYNC:
		default:
			break;
		}
		res = RES_OK;
		break;
	}
	return res;
}
