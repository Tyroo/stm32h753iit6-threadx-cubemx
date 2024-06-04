#ifndef __DWT_H__
#define __DWT_H__

#include "stm32h7xx_hal.h"


static inline void HAL_DWT_Init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline unsigned int HAL_DWT_GetTick(void)
{
	return DWT->CYCCNT;
}

static inline void HAL_DWT_Start(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline void HAL_DWT_Stop(void)
{
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
	DWT->CYCCNT = 0;
}

static inline void HAL_DWT_Suspend(void)
{
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
}

static inline void HAL_DWT_Resume(void)
{
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


void HAL_DWT_DelayUS(unsigned int uwDelay);
void HAL_DWT_DelayMS(unsigned int uwDelay);

unsigned char HAL_DWT_TimeoutCheckUS(unsigned int uwTick, unsigned int uwTimeout);
unsigned char HAL_DWT_TimeoutCheckMS(unsigned int uwTick, unsigned int uwTimeout);

#endif