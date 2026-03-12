#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

//TP Define
#define TRST		22
#define INT			27

#define TRST_0		DEV_Digital_Write(TRST, 0)
#define TRST_1		DEV_Digital_Write(TRST, 1)

#define INT_0		DEV_Digital_Write(INT, 0)
#define INT_1		DEV_Digital_Write(INT, 1)

/**
 * GPIO configuration
**/
extern int EPD_RST_PIN;
extern int EPD_DC_PIN;
extern int EPD_CS_PIN;
extern int EPD_BUSY_PIN;

extern int IIC_Address;

#define TP_SDA_PIN    2
#define TP_SCL_PIN    3

#define EPD_MOSI_PIN  11
#define EPD_SCK_PIN   10

/*------------------------------------------------------------------------------------------------------*/

UBYTE DEV_ModuleInit(void);
void  DEV_ModuleExit(void);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_Delay_ms(UDOUBLE xms);

void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);

UBYTE I2C_Write_Byte(UWORD Reg, char *Data, UBYTE len);
UBYTE I2C_Read_Byte(UWORD Reg, char *Data, UBYTE len);
#endif
