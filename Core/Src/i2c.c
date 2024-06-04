#include "i2c.h"


// 发送起始信号，SCK高电平下，SDA由高变低
static void I2C_Start(void)
{
	I2C_SCL_H();
	
	I2C_SDA_H();
	
	I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
	
	I2C_SDA_L();
	
	I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
	
	I2C_SCL_L();
}

    
// 发送结束信号，SCK高电平下，SDA由低变高
static void I2C_Stop(void) 
{
	I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
	
	I2C_SDA_L();
	
	I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
	
	I2C_SCL_H();
	
	I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
	
	I2C_SDA_H();
	
	I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
}


// 发送8位数据
static void I2C_TxByte(unsigned char u8Data) 
{
	for(unsigned char u8Bit = 0; u8Bit < 8; u8Bit++) 
	{
		I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
		
		(u8Data & 0x80) ? I2C_SDA_H() : I2C_SDA_L();
		u8Data <<= 1;
		
		I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
		
		I2C_SCL_H();
		
		I2C_DELAY_US(I2C_DATA_HOLD_TIME_US);
		
		I2C_SCL_L();
	}
}


// 接收8位数据
static unsigned char I2C_RxByte(void) 
{
	unsigned char u8Data = 0;
	
	for(unsigned char u8Bit = 0; u8Bit < 8; u8Bit++) 
	{
		I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
		
		I2C_SDA_H();
		
		I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
		
		I2C_SCL_H();
		
		I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
		
		u8Data <<= 1;
		u8Data |= I2C_SDA_R();
		
		I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
		
		I2C_SCL_L();
	}
	
	return u8Data;
}


// 发送响应信号（应答、非应答），接收完数据后将SDA拉高
static void I2C_TxReply(unsigned char u8Ack)
{
	I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
	
	u8Ack ? I2C_SDA_L() : I2C_SDA_H();
	
	I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
	
	I2C_SCL_H();
	
	I2C_DELAY_US(I2C_DATA_HOLD_TIME_US);
	
	I2C_SCL_L();
}


// 接收响应信号（应答、非应答），接收完数据后将SDA拉高
static unsigned char I2C_RxReply(void)
{
	unsigned char u8Ask = 0;
	
	I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
	
	I2C_SDA_H();
	
	I2C_DELAY_US(I2C_DATA_PREP_TIME_US >> 1);
	
	I2C_SCL_H();
	
	I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
	
	u8Ask = (I2C_SDA_R() == 0);

	I2C_DELAY_US(I2C_DATA_HOLD_TIME_US >> 1);
	
	I2C_SCL_L();
	
	return u8Ask;
}


unsigned char I2C_ReadByte(void)
{
	unsigned char u8Data;
	
	I2C_Start();
	
	u8Data = I2C_RxByte();
	
	I2C_TxReply(0);
	
	I2C_Stop();
	
	return u8Data;
}


unsigned char I2C_WriteByte(unsigned char u8Data)
{
	unsigned char u8Ack;
	
	I2C_Start();
	
	I2C_TxByte(u8Data);
	
	u8Ack = I2C_RxReply();
	
	I2C_Stop();
	
	return u8Ack;
}


unsigned char I2C_ReadRegister(unsigned char u8Addr, unsigned char u8Reg, unsigned char * au8DataBuff, unsigned short u16Size)
{
	I2C_Start();
	
	// 发送设备地址（写入数据）
	I2C_TxByte(u8Addr & 0xFE);
	if (I2C_RxReply() == 0) { I2C_Stop(); return 0; }
	
	// 发送寄存器地址
	I2C_TxByte(u8Reg);
	if (I2C_RxReply() == 0) { I2C_Stop(); return 0; }
	
	I2C_Stop();
	
	I2C_Start();
	
	// 发送设备地址（读出数据）
	I2C_TxByte(u8Addr | 0x01);
	if (I2C_RxReply() == 0) { I2C_Stop(); return 0; }
	
	unsigned char u8Index = 0;
	
	while (u16Size--)
	{
		au8DataBuff[u8Index++] = I2C_RxByte();
		I2C_TxReply(u16Size);
	}
	
	I2C_Stop();
	
	return 1;
}


unsigned char I2C_WriteRegister(unsigned char u8Addr, unsigned char u8Reg, unsigned char * au8DataBuff, unsigned short u16Size)
{
	I2C_Start();
	
	// 发送设备地址（写入数据）
	I2C_TxByte(u8Addr & 0xFE);
	if (I2C_RxReply() == 0) { I2C_Stop(); return 0; }
	
	// 发送寄存器地址
	I2C_TxByte(u8Reg);
	if (I2C_RxReply() == 0) { I2C_Stop(); return 0; }
	
	unsigned char u8Index = 0;
	
	while (u16Size--)
	{
		I2C_TxByte(au8DataBuff[u8Index++]);
		
		if (I2C_RxReply() == 0) 
		{
			break;
		}
	}
	
	I2C_Stop();
	
	return (u8Index == u16Size);
}


unsigned char I2C_DeviceScan(unsigned char au8AddrTab[], unsigned char u8ScanNum)
{
	unsigned char u8DeviceAddr = 2, u8DeviceNum = 0;
	
	if (u8ScanNum > 127) u8ScanNum = 127;
	
	while(u8ScanNum--)
	{
		I2C_Start();
		
		I2C_TxByte(u8DeviceAddr);
		
		if (I2C_RxReply())
		{
			au8AddrTab[u8DeviceNum] = u8DeviceAddr;
			u8DeviceNum++;
		}
		
		u8DeviceAddr += 2;
		
		I2C_Stop();
	}
	
	return u8DeviceNum;
}
