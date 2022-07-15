#ifndef __DEV_SPIFLASH_H__
#define __DEV_SPIFLASH_H__

#include "SWM320.h"

//----------------------------端口配置----------------------------//
#define SPI_PORT_CS				GPIOP
#define SPI_PIN_CS				PIN22

#define SPI_PORT_SCLK			PORTP
#define SPI_PIN_SCLK			PIN23
#define SPI_FUNMUX_SCKL			FUNMUX1_SPI0_SCLK

#define SPI_PORT_MOSI			PORTP
#define SPI_PIN_MOSI			PIN18
#define SPI_FUNMUX_MOSI			FUNMUX0_SPI0_MOSI

#define SPI_PORT_MISO			PORTP
#define SPI_PIN_MISO			PIN19
#define SPI_FUNMUX_MISO			FUNMUX1_SPI0_MISO

#define SPI_CS_CLR()  			GPIO_ClrBit(SPI_PORT_CS, SPI_PIN_CS) // SPI片选信号 拉低	
#define SPI_CS_SET() 			GPIO_SetBit(SPI_PORT_CS, SPI_PIN_CS) // SPI片选信号 置高

//----------------------------SPI-Flash 参数----------------------------//
//#define  SPI_FLASH_ID              	0xEF3015   //W25X16
//#define  SPI_FLASH_ID              	0xEF4015   //W25Q16
//#define  SPI_FLASH_ID              	0XEF4017   //W25Q64
#define  SPI_FLASH_ID              		0XEF4018   //W25Q128

#define SPI_FLASH_PAGE_SIZE              256  //页字节数(单次可写的最大值)

//----------------------------SPI-Flash 命令----------------------------//
#define W25X_WriteEnable		      	0x06 
#define W25X_WriteDisable		      	0x04 
#define W25X_ReadStatusReg_1		    0x05
#define W25X_ReadStatusReg_2		    0x35
#define W25X_WriteStatusReg		    	0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      	0x0B 
#define W25X_FastReadDual		      	0x3B 
#define W25X_PageProgram		      	0x02 
#define W25X_BlockErase			      	0xD8 
#define W25X_SectorErase		      	0x20 
#define W25X_ChipErase			      	0xC7 
#define W25X_PowerDown			      	0xB9 
#define W25X_ReleasePowerDown	    	0xAB 
#define W25X_DeviceID			     	0xAB 
#define W25X_ManufactDeviceID   		0x90 
#define W25X_JedecDeviceID		    	0x9F

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                  0x01
#define Dummy_Byte                0xFF

//----------------------------函数声明----------------------------//
int spi_flash_init(void);

void spi_flash_erase_sector(uint32_t addr);
void spi_flash_erase_chip(void);
void spi_flash_read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void spi_flash_read_fast(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void spi_flash_write_page(uint8_t *buff, uint32_t addr, uint16_t bytes);
void spi_flash_write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

void spi_flash_wakeup(void);
void spi_flash_power_down(void);

#endif //__DEV_SPIFLASH_H__
