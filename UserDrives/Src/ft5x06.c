#include "i2c.h"
#include "ft5x06.h"


void FT5X06_Init(void)
{
	FT5X06_SET_PIN_RST(0);
	HAL_Delay(20);
 	FT5X06_SET_PIN_RST(1);
	
	FT5X06_SetOperationMode(0);		// 进入正常操作模式
	FT5X06_SetTouchNoticeMode(0);	// 查询模式
	FT5X06_SetThgroup(22);			// 设置触摸有效值22，越小越灵敏
	FT5X06_SetPeriodActive(12);		// 激活周期，不能小于12，最大14
	
	HAL_Delay(50);					// 等待处理完毕
}


unsigned char FT5X06_ReadRegister(unsigned short u16RegAddr, unsigned char au8ValueBuff[], unsigned char u8Length)
{
	return I2C_ReadRegister(FT5X06_IIC_DEVICE_ADDR, u16RegAddr, au8ValueBuff, u8Length);
}


unsigned char FT5X06_WriteRegister(unsigned short u16RegAddr, unsigned char au8ValueBuff[], unsigned char u8Length)
{
	return I2C_WriteRegister(FT5X06_IIC_DEVICE_ADDR, u16RegAddr, au8ValueBuff, u8Length);
}


unsigned char FT5X06_GetTouchPoint(unsigned short * pu16XPoint, unsigned short * pu16YPoint)
{
	unsigned char u8Ret;
	unsigned char au8PointData[4];
	
	u8Ret = FT5X06_ReadRegister(FT5X06_TP1_REG, au8PointData, 4);
	
	if (u8Ret)
	{
		(*pu16YPoint) = (((au8PointData[0] & 0x0F) << 8) | au8PointData[1]);
		(*pu16XPoint) = (((au8PointData[2] & 0x0F) << 8) | au8PointData[3]);
	}
	
	return u8Ret;
}


unsigned char FT5X06_GetTouchPoints(unsigned short au16XPoint[], unsigned short au16YPoint[], unsigned char * pu8PointNum)
{
	return 0;
}


unsigned char FT5X06_GetTouchGesture(unsigned char * pu8Gesture)
{
	return FT5X06_ReadRegister(FT5X06_REG_GESTURE, pu8Gesture, 1);
}


unsigned char FT5X06_GetVersion(unsigned short * u16Version)
{
	unsigned char u8Ret;
	unsigned char au8VersionData[2];
	
	u8Ret = FT5X06_ReadRegister(FT5X06_ID_G_LIB_VERSION, au8VersionData, 2);
	
	if (u8Ret)
	{
		(*u16Version) = ((au8VersionData[0] << 8) | au8VersionData[1]);
	}
	
	return u8Ret;
}


unsigned char FT5X06_SetOperationMode(unsigned char u8Mode)
{
	// 设置操作模式
	return FT5X06_WriteRegister(FT5X06_RET_OPER_MODE, &u8Mode, 1);		 
}


unsigned char FT5X06_SetTouchNoticeMode(unsigned char u8Mode)
{
	// 设置触摸通知模式（设置触摸有效后INT引脚发送给MCU的时序）
	return FT5X06_WriteRegister(FT5X06_ID_G_MODE, &u8Mode, 1);	
}


unsigned char FT5X06_SetThgroup(unsigned char u8Rate)
{
	//设置触摸有效值，越小越灵敏
	return FT5X06_WriteRegister(FT5X06_ID_G_THGROUP, &u8Rate, 1);
}


unsigned char FT5X06_SetPeriodActive(unsigned char u8Period)
{
	if ((u8Period < 12) || (u8Period > 14)) return 0;
	
	return FT5X06_WriteRegister(FT5X06_ID_G_PERIODACTIVE, &u8Period, 1);	//激活周期，不能小于12，最大14
}
