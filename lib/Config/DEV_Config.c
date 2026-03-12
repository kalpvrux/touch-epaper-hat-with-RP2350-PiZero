#include "DEV_Config.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdint.h>

int EPD_RST_PIN = 17;
int EPD_DC_PIN = 25;
int EPD_CS_PIN = 8;
int EPD_BUSY_PIN = 24;

int IIC_Address = 0x14;

#define SPI_PORT spi1
#define I2C_PORT i2c1

void DEV_Digital_Write(UWORD Pin, UBYTE Value) { gpio_put(Pin, Value); }

UBYTE DEV_Digital_Read(UWORD Pin) { return gpio_get(Pin); }

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode) {
  gpio_init(Pin);
  if (Mode == 0) {
    gpio_set_dir(Pin, GPIO_IN);
    gpio_pull_up(Pin);
  } else {
    gpio_set_dir(Pin, GPIO_OUT);
  }
}

void DEV_Delay_ms(UDOUBLE xms) { sleep_ms(xms); }

static void DEV_GPIO_Init(void) {
  DEV_GPIO_Mode(EPD_RST_PIN, 1);
  DEV_GPIO_Mode(EPD_DC_PIN, 1);
  DEV_GPIO_Mode(EPD_CS_PIN, 1); // CS as manual GPIO (not SPI-managed)
  DEV_GPIO_Mode(EPD_BUSY_PIN, 0);

  DEV_Digital_Write(EPD_CS_PIN, 1);

  DEV_GPIO_Mode(TRST, 1);
  DEV_GPIO_Mode(INT, 0);
}

UBYTE DEV_ModuleInit(void) {
  stdio_init_all();

  DEV_GPIO_Init();

  // SPI Init
  spi_init(SPI_PORT, 4 * 1000 * 1000); // 4MHz
  gpio_set_function(EPD_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(EPD_MOSI_PIN, GPIO_FUNC_SPI);

  // I2C Init
  i2c_init(I2C_PORT, 400 * 1000); // 400kHz
  gpio_set_function(TP_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(TP_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(TP_SDA_PIN);
  gpio_pull_up(TP_SCL_PIN);

  return 0;
}

void DEV_SPI_WriteByte(uint8_t Value) {
  spi_write_blocking(SPI_PORT, &Value, 1);
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len) {
  spi_write_blocking(SPI_PORT, pData, Len);
}

UBYTE I2C_Write_Byte(UWORD Reg, char *Data, UBYTE len) {
  uint8_t buf[256];
  buf[0] = (Reg >> 8) & 0xff;
  buf[1] = Reg & 0xff;
  for (UBYTE i = 0; i < len; i++) {
    buf[i + 2] = Data[i];
  }
  i2c_write_blocking(I2C_PORT, IIC_Address, buf, len + 2, false);
  return 0;
}

UBYTE I2C_Read_Byte(UWORD Reg, char *Data, UBYTE len) {
  uint8_t reg_buf[2];
  reg_buf[0] = (Reg >> 8) & 0xff;
  reg_buf[1] = Reg & 0xff;

  i2c_write_blocking(I2C_PORT, IIC_Address, reg_buf, 2, true); // nostop
  i2c_read_blocking(I2C_PORT, IIC_Address, (uint8_t *)Data, len, false);
  return 0;
}

void DEV_ModuleExit(void) {
  DEV_Digital_Write(EPD_CS_PIN, 0);
  DEV_Digital_Write(EPD_DC_PIN, 0);
  DEV_Digital_Write(EPD_RST_PIN, 0);
  TRST_0;

  i2c_deinit(I2C_PORT);
  spi_deinit(SPI_PORT);
}
