#ifndef __I2C_H__
#define __I2C_H__


#include "gpio.h"
#include "dwt.h"

// SDA和SCL需要配置为开漏输出模式

#define I2C_SDA_R()					((GPIOI->IDR & GPIO_PIN_3) != GPIO_PIN_RESET)

#define I2C_SDA_H()					(GPIOI->BSRR = (uint32_t)(GPIO_PIN_3))
#define I2C_SDA_L()					(GPIOI->BSRR = (uint32_t)(GPIO_PIN_3) << 16U)

#define I2C_SCL_H()					(GPIOH->BSRR = (uint32_t)(GPIO_PIN_6))
#define I2C_SCL_L()					(GPIOH->BSRR = (uint32_t)(GPIO_PIN_6) << 16U)

#define I2C_DELAY_US(t)				HAL_DWT_DelayUS(t)

#define I2C_DATA_HOLD_TIME_US		(2)
#define I2C_DATA_PREP_TIME_US		(2)

unsigned char I2C_ReadByte(void);
unsigned char I2C_WriteByte(unsigned char u8Data);

unsigned char I2C_ReadRegister(unsigned char u8Addr, unsigned char u8Reg, unsigned char * au8DataBuff, unsigned short u16Size);
unsigned char I2C_WriteRegister(unsigned char u8Addr, unsigned char u8Reg, unsigned char * au8DataBuff, unsigned short u16Size);

unsigned char I2C_DeviceScan(unsigned char au8AddrTab[], unsigned char u8ScanNum);

#endif
