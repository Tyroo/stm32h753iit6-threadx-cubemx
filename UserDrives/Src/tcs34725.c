#include "tcs34725.h"


static unsigned char TCS34725_ReadByte(unsigned char u8Reg, unsigned char * pu8Data)
{
	return TCS34725_I2C_READ_REG(TCS34725_DEVICE_ADDRESS, u8Reg | 0x80, pu8Data, 1);
}

static unsigned char TCS34725_ReadBytes(unsigned char u8Reg, unsigned char au8Data[], unsigned char u8Size)
{
	return TCS34725_I2C_READ_REG(TCS34725_DEVICE_ADDRESS, u8Reg | 0xA0, au8Data, u8Size);
}

static unsigned char TCS34725_WriteByte(unsigned char u8Reg, unsigned char * pu8Data)
{
	return TCS34725_I2C_WRITE_REG(TCS34725_DEVICE_ADDRESS, u8Reg | 0x80, pu8Data, 1);
}

static unsigned char TCS34725_WriteBytes(unsigned char u8Reg, unsigned char * au8Data, unsigned char u8Size)
{
	return TCS34725_I2C_WRITE_REG(TCS34725_DEVICE_ADDRESS, u8Reg | 0xA0, au8Data, u8Size);
}

unsigned char TCS34725_EnableInterrupt(unsigned char u8Enable)
{
	unsigned char u8RegValue;
	
	if (!TCS34725_ReadByte(TCS34725_ENABLE_REG, &u8RegValue)) return 0;
	
	if (u8Enable)
	{
		u8RegValue |= (0x01 << TCS34725_ENABLE_REG_AIEN_BIT_OFFSET);
	}
	else
	{
		u8RegValue &= ~(0x01 << TCS34725_ENABLE_REG_AIEN_BIT_OFFSET);
	}
	
	return TCS34725_WriteByte(TCS34725_ENABLE_REG, &u8RegValue);
}

unsigned char TCS34725_EnableWait(unsigned char u8Enable)
{
	unsigned char u8RegValue;
	
	if (!TCS34725_ReadByte(TCS34725_ENABLE_REG, &u8RegValue)) return 0;
	
	if (u8Enable)
	{
		u8RegValue |= (0x01 << TCS34725_ENABLE_REG_WEN_BIT_OFFSET);
	}
	else
	{
		u8RegValue &= ~(0x01 << TCS34725_ENABLE_REG_WEN_BIT_OFFSET);
	}
	
	return TCS34725_WriteByte(TCS34725_ENABLE_REG, &u8RegValue);
}

unsigned char TCS34725_EnableConvert(unsigned char u8Enable)
{
	unsigned char u8RegValue;
	
	if (!TCS34725_ReadByte(TCS34725_ENABLE_REG, &u8RegValue)) return 0;
	
	if (u8Enable)
	{
		u8RegValue |= (0x01 << TCS34725_ENABLE_REG_AEN_BIT_OFFSET);
		u8RegValue |= (0x01 << TCS34725_ENABLE_REG_PON_BIT_OFFSET);
	}
	else
	{
		u8RegValue &= ~(0x01 << TCS34725_ENABLE_REG_AEN_BIT_OFFSET);
		u8RegValue &= ~(0x01 << TCS34725_ENABLE_REG_PON_BIT_OFFSET);
	}
	
	return TCS34725_WriteByte(TCS34725_ENABLE_REG, &u8RegValue);
}


unsigned char TCS34725_SetConvertCycle(unsigned char u8Cycle)
{
	unsigned char u8RegValue = 0 - u8Cycle;
	
	return TCS34725_WriteByte(TCS34725_ATIME_REG, &u8RegValue);
}


unsigned char TCS34725_SetWaitCycle(unsigned char u8Cycle)
{
	unsigned char u8RegValue = 0 - u8Cycle;
	
	return TCS34725_WriteByte(TCS34725_WTIME_REG, &u8RegValue);
}

unsigned char TCS34725_SetBrightThreshold(unsigned short u16UpperThreshold, unsigned short u16LowerThreshold)
{
	unsigned int u32RegValue = (u16UpperThreshold << 16) | u16LowerThreshold;
	
	return TCS34725_WriteBytes(TCS34725_AILTL_REG, (unsigned char *)&u32RegValue, sizeof(u32RegValue));
}

unsigned char TCS34725_SetInterruptFunc(enTCS34725_InterruptFunc_t enIntFunc)
{
	unsigned char u8RegValue = enIntFunc & 0x0F;
	
	return TCS34725_WriteByte(TCS34725_PERS_REG, &u8RegValue);
}

unsigned char TCS34725_EnableLongWait(unsigned char u8Enable)
{
	unsigned char u8RegValue = 0;
	
	if (u8Enable)
	{
		u8RegValue |= (0x01 << TCS34725_CONFIG_REG_WLONG_BIT_OFFSET);
	}
	
	return TCS34725_WriteByte(TCS34725_CONFIG_REG, &u8RegValue);
}

unsigned char TCS34725_SetGain(enTCS34725_Gain_t enGain)
{
	unsigned char u8RegValue = enGain;
	
	return TCS34725_WriteByte(TCS34725_CONTROL_REG, &u8RegValue);
}

unsigned char TCS34725_GetDeviceID(unsigned char * pu8DeviceID)
{
	return TCS34725_ReadByte(TCS34725_ID_REG, pu8DeviceID);
}


unsigned char TCS34725_GetInterruptStatus(unsigned char * pu8Status)
{
	unsigned char u8Status;
	
	if (!TCS34725_ReadByte(TCS34725_STATUS_REG, &u8Status)) return 0;
	
	u8Status >>= TCS34725_STATUS_REG_AINT_BIT_OFFSET;
	u8Status &= 0x01;
	*pu8Status = u8Status;
	
	return 1;
}

unsigned char TCS34725_GetConvertStatus(unsigned char * pu8Status)
{
	unsigned char u8Status;
	
	if (!TCS34725_ReadByte(TCS34725_STATUS_REG, &u8Status)) return 0;
	
	u8Status >>= TCS34725_STATUS_REG_AVALID_BIT_OFFSET;
	u8Status &= 0x01;
	*pu8Status = u8Status;
	
	return 1;
}

unsigned char TCS34725_GetConvertResult(unsigned short * pu16Red, unsigned short * pu16Green, unsigned short * pu16Blue, unsigned short * pu16Clear)
{
	if ((pu16Clear) && (!TCS34725_ReadBytes(TCS34725_CDATAL_REG, (unsigned char *)pu16Clear, 2))) return 0;
	if ((pu16Red) && (!TCS34725_ReadBytes(TCS34725_RDATAL_REG, (unsigned char *)pu16Red, 2))) return 0;
	if ((pu16Green) && (!TCS34725_ReadBytes(TCS34725_GDATAL_REG, (unsigned char *)pu16Green, 2))) return 0;
	if ((pu16Blue) && (!TCS34725_ReadBytes(TCS34725_BDATAL_REG, (unsigned char *)pu16Blue, 2))) return 0;
	
	return 1;
}
