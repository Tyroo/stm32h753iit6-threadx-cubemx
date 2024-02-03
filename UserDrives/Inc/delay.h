#ifndef __DELAY_H__
#define __DELAY_H__

/**
*	@include
**/
#include "stm32h7xx_hal_rcc.h"


/**
*	@define
**/
// 系统时钟频率下，1s的计数值
#define DELAY_SYSCLK_HZ     (400000000)
// 延时1us修正值
#define CORRECT_VALUE       (45)
// 延时1us计数值
#define DELAY_ONE_US        ((DELAY_SYSCLK_HZ / 1000000) - CORRECT_VALUE)
// 延时1ms计数值
#define DELAY_ONE_MS        (DELAY_ONE_US * 1000) 	        


/**
* @function
*	作用：延时模块初始化函数
*	参数：void
*	返回值：void
**/
void Delay_Init(uint32_t Rate);


/**
* @function
*	作用：实现微秒级延时
*	参数：{ Us：要延时的微秒数 }
*	返回值：void
**/
void Delay_Us(uint32_t Us);


/**
* @function
*	作用：实现毫秒级延时
*	参数：{ Ms：要延时的毫秒数 }
*	返回值：void
**/
void Delay_Ms(uint32_t Ms);

#endif
