#ifndef __FT5X06_H__
#define __FT5X06_H__

#include "stm32h7xx_hal.h"


#define FT5X06_IIC_DEVICE_ADDR		(0x70)

#define FT5X06_SET_PIN_RST(en)		((en) ? (GPIOI->BSRR = (uint32_t)GPIO_PIN_8) : (GPIOI->BSRR = (uint32_t)GPIO_PIN_8 << 16U))
#define FT5X06_GET_PIN_PEN()        ((GPIOH->IDR & GPIO_PIN_7) != GPIO_PIN_RESET)

//FT5206 部分寄存器定义 
#define FT5X06_RET_OPER_MODE 		0x00   		//FT5x06操作模式控制寄存器
#define FT5X06_REG_GESTURE 			0x01   		//FT5x06触摸手势寄存器
#define FT5X06_REG_NUM_FINGER       0x02		//触摸状态寄存器

#define FT5X06_TP1_REG 				0X03	  	//第一个触摸点数据地址
#define FT5X06_TP2_REG 				0X09		//第二个触摸点数据地址
#define FT5X06_TP3_REG 				0X0F		//第三个触摸点数据地址
#define FT5X06_TP4_REG 				0X15		//第四个触摸点数据地址
#define FT5X06_TP5_REG 				0X1B		//第五个触摸点数据地址  

#define	FT5X06_ID_G_LIB_VERSION		0xA1		//版本		
#define FT5X06_ID_G_MODE 			0xA4   		//FT5x06中断模式控制寄存器
#define FT5X06_ID_G_THGROUP			0x80   		//触摸有效值设置寄存器
#define FT5X06_ID_G_PERIODACTIVE	0x88   		//激活状态周期设置寄存器


void FT5X06_Init(void);

unsigned char FT5X06_ReadRegister(unsigned short u16RegAddr, unsigned char au8ValueBuff[], unsigned char u8Length);
unsigned char FT5X06_WriteRegister(unsigned short u16RegAddr, unsigned char au8ValueBuff[], unsigned char u8Length);

unsigned char FT5X06_GetTouchPoint(unsigned short * pu16XPoint, unsigned short * pu16YPoint);
unsigned char FT5X06_GetTouchPoints(unsigned short au16XPoint[], unsigned short au16YPoint[], unsigned char * pu8PointNum);
unsigned char FT5X06_GetTouchGesture(unsigned char * pu8Gesture);

unsigned char FT5X06_GetVersion(unsigned short * u16Version);
unsigned char FT5X06_SetOperationMode(unsigned char u8Mode);
unsigned char FT5X06_SetTouchNoticeMode(unsigned char u8Mode);
unsigned char FT5X06_SetThgroup(unsigned char u8Rate);
unsigned char FT5X06_SetPeriodActive(unsigned char u8Period);

#endif
