#ifndef __TCS34725_H__
#define __TCS34725_H__

#include "i2c.h"

#define TCS34725_I2C_READ_REG(ADDR, REG, DATA, SIZE)	I2C_ReadRegister(ADDR, REG, DATA, SIZE)
#define TCS34725_I2C_WRITE_REG(ADDR, REG, DATA, SIZE)	I2C_WriteRegister(ADDR, REG, DATA, SIZE)

#define TCS34725_DEVICE_ADDRESS	(0x52)


// TCS34725 Register Address:
#define TCS34725_ENABLE_REG	    (0x00)
#define TCS34725_ATIME_REG	    (0x01)
#define TCS34725_WTIME_REG	    (0x03)
#define TCS34725_AILTL_REG	    (0x04)
#define TCS34725_AILTH_REG	    (0x05)
#define TCS34725_AIHTL_REG	    (0x06)
#define TCS34725_AIHTH_REG      (0x07)
#define TCS34725_PERS_REG       (0x0C)
#define TCS34725_CONFIG_REG	    (0x0D)
#define TCS34725_CONTROL_REG	(0x0F)
#define TCS34725_ID_REG			(0x12)
#define TCS34725_STATUS_REG	    (0x13)
#define TCS34725_CDATAL_REG	    (0x14)
#define TCS34725_CDATAH_REG	    (0x15)
#define TCS34725_RDATAL_REG	    (0x16)
#define TCS34725_RDATAH_REG	    (0x17)
#define TCS34725_GDATAL_REG	    (0x18)
#define TCS34725_GDATAH_REG	    (0x19)
#define TCS34725_BDATAL_REG	    (0x1A)
#define TCS34725_BDATAH_REG	    (0x1B)


#define TCS34725_ENABLE_REG_AIEN_BIT_OFFSET		(4)
#define TCS34725_ENABLE_REG_WEN_BIT_OFFSET		(3)
#define TCS34725_ENABLE_REG_AEN_BIT_OFFSET		(1)
#define TCS34725_ENABLE_REG_PON_BIT_OFFSET		(0)	

#define TCS34725_CONFIG_REG_WLONG_BIT_OFFSET	(1)

#define TCS34725_STATUS_REG_AINT_BIT_OFFSET		(4)
#define TCS34725_STATUS_REG_AVALID_BIT_OFFSET	(0)

typedef enum
{
	enTCS34725_InterruptFunc_Every,				///< Every RGBC cycle generates an interrupt.
	enTCS34725_InterruptFunc_OverThershold_1,	///< 1 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_2,	///< 2 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_3,	///< 3 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_5,	///< 5 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_10,	///< 10 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_15,	///< 15 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_20,	///< 20 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_25,	///< 25 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_30,	///< 30 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_35,	///< 35 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_40,	///< 40 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_45,	///< 45 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_50,	///< 50 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_55,	///< 55 clear channel value outside of threshold range.
	enTCS34725_InterruptFunc_OverThershold_60,	///< 60 clear channel value outside of threshold range.
} enTCS34725_InterruptFunc_t;

typedef enum
{
	enTCS34725_Gain_1X,		///< 1x gain.
	enTCS34725_Gain_4X,		///< 4x gain.
	enTCS34725_Gain_16X,	///< 16x gain.
	enTCS34725_Gain_60X,	///< 60x gain.
} enTCS34725_Gain_t;


unsigned char TCS34725_EnableInterrupt(unsigned char u8Enable);
unsigned char TCS34725_EnableWait(unsigned char u8Enable);
unsigned char TCS34725_EnableConvert(unsigned char u8Enable);
unsigned char TCS34725_EnableLongWait(unsigned char u8Enable);

unsigned char TCS34725_SetConvertCycle(unsigned char u8Cycle);
unsigned char TCS34725_SetWaitCycle(unsigned char u8Cycle);
unsigned char TCS34725_SetBrightThreshold(unsigned short u16UpperThreshold, unsigned short u16LowerThreshold);
unsigned char TCS34725_SetInterruptFunc(enTCS34725_InterruptFunc_t enIntFunc);
unsigned char TCS34725_SetGain(enTCS34725_Gain_t enGain);

unsigned char TCS34725_GetDeviceID(unsigned char * pu8DeviceID);
unsigned char TCS34725_GetInterruptStatus(unsigned char * pu8Status);
unsigned char TCS34725_GetConvertStatus(unsigned char * pu8Status);
unsigned char TCS34725_GetConvertResult(unsigned short * pu16Red, unsigned short * pu16Green, unsigned short * pu16Blue, unsigned short * pu16Clear);

#endif
