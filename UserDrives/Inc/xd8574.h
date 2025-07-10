#ifndef __XD8574_H__
#define __XD8574_H__

#include "i2c.h"

#define XD8574_I2C_READ_BYTE(DEV, DATA)		I2C_ReadBytes(DEV, DATA, 1)
#define XD8574_I2C_WRITE_BYTE(DEV, DATA)	I2C_WriteBytes(DEV, DATA, 1)


typedef enum
{
	enXD8574_Device_0 = 0x70,
	enXD8574_Device_1,
	enXD8574_Device_2,
	enXD8574_Device_3,
	enXD8574_Device_4,
	enXD8574_Device_5,
	enXD8574_Device_6,
	enXD8574_Device_7,
} enXD8574_Device_t;

typedef enum
{
	enXD8574_Pin_P0 = 0x01,
	enXD8574_Pin_P1 = 0x02,
	enXD8574_Pin_P2 = 0x04,
	enXD8574_Pin_P3 = 0x08,
	enXD8574_Pin_P4 = 0x10,
	enXD8574_Pin_P5 = 0x20,
	enXD8574_Pin_P6 = 0x40,
	enXD8574_Pin_P7 = 0x80,
} enXD8574_Pin_t;

typedef enum
{
	enXD8574_PinMode_Input,
	enXD8574_PinMode_Output,
} enXD8574_PinMode_t;

typedef enum
{
	enXD8574_PinStatus_Low,
	enXD8574_PinStatus_High,
} enXD8574_PinStatus_t;


unsigned char XD8574_SetPinMode(enXD8574_Device_t enDevice, enXD8574_Pin_t enPin, enXD8574_PinMode_t enMode);
unsigned char XD8574_SetPinStatus(enXD8574_Device_t enDevice, enXD8574_Pin_t enPin, enXD8574_PinStatus_t enStatus);
unsigned char XD8574_GetPinStatus(enXD8574_Device_t enDevice, enXD8574_Pin_t enPin, unsigned char * pu8Status);

#endif