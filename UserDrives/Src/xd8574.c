#include "xd8574.h"


unsigned char XD8574_SetPinMode(enXD8574_Device_t enDevice, enXD8574_Pin_t enPin, enXD8574_PinMode_t enMode)
{
	unsigned char u8RegValue;
	
	if (enMode != enXD8574_PinMode_Input) return 1;
	
	if (!XD8574_I2C_READ_BYTE(enDevice, &u8RegValue)) return 0;
	
	u8RegValue |= (0x01 << enPin);
	
	if (!XD8574_I2C_WRITE_BYTE(enDevice, &u8RegValue)) return 0;
	
	return 1;
}

unsigned char XD8574_SetPinStatus(enXD8574_Device_t enDevice, enXD8574_Pin_t enPin, enXD8574_PinStatus_t enStatus)
{
	unsigned char u8RegValue;
	
	if (!XD8574_I2C_READ_BYTE(enDevice, &u8RegValue)) return 0;
	
	if (enStatus == enXD8574_PinStatus_Low)
	{
		u8RegValue &= ~enPin;
	}
	else
	{
		u8RegValue |= enPin;
	}
	
	if (!XD8574_I2C_WRITE_BYTE(enDevice, &u8RegValue)) return 0;
	
	return 1;
}

unsigned char XD8574_GetPinStatus(enXD8574_Device_t enDevice, enXD8574_Pin_t enPin, unsigned char * pu8Status)
{
	unsigned char u8RegValue;
	
	if (!XD8574_I2C_READ_BYTE(enDevice, &u8RegValue)) return 0;
	
	*pu8Status = ((u8RegValue >> enPin) & 0x01);
	
	return 1;
}
