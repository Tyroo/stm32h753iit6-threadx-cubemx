/* 延时模块（验证通过） */
#include "stm32h7xx_hal.h"
#include "delay.h"


// 延时模块初始化函数
void Delay_Init(unsigned int Rate) 
{
	unsigned int ReloadCount;
	
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);	// 设置SysTick的时钟
	
	ReloadCount = DELAY_SYSCLK_HZ / Rate;
	
	SysTick->LOAD = ReloadCount;                 // 计数值加载
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   // 开启系统滴答中断
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    // 开始倒数
}


// 实现微秒级延时
void Delay_Us(unsigned int Us) 
{
	unsigned int CountValue = 0;
	unsigned int CountValueOld, CountValueNew;
	unsigned int StopCountValue = Us * DELAY_ONE_US;    // 计算出延时Us微秒需要计数的值
	
	CountValueOld = SysTick->VAL;

	// 当systick定时器递减计数器未减至0且减计数器处于开启时一直阻塞
	do 
	{
		CountValueNew = SysTick->VAL;
		CountValue += (SysTick->LOAD + CountValueOld - CountValueNew) % SysTick->LOAD;
		CountValueOld = CountValueNew;
	} 
	while(CountValue < StopCountValue);
}


// 实现毫秒级延时
void Delay_Ms(unsigned int Ms) 
{
	unsigned int CountValue = 0;
	unsigned int CountValueOld, CountValueNew;
	unsigned int StopCountValue = Ms * DELAY_ONE_MS;    // 计算出延时Ms微秒需要计数的值
	
	CountValueOld = SysTick->VAL;

	// 当systick定时器递减计数器未减至0且减计数器处于开启时一直阻塞
	do 
	{
		CountValueNew = SysTick->VAL;
		CountValue += (SysTick->LOAD + CountValueOld - CountValueNew) % SysTick->LOAD;
		CountValueOld = CountValueNew;
	} 
	while(CountValue < StopCountValue);
}

