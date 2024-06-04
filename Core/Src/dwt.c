#include "dwt.h"


void HAL_DWT_DelayUS(unsigned int uwDelay)
{
	unsigned int uwTimeDelay;
	unsigned int uwTimeStart = DWT->CYCCNT;
	
	do
	{
		uwTimeDelay = (DWT->CYCCNT - uwTimeStart) / (SystemCoreClock / 1000000);
	}
	while(uwTimeDelay < uwDelay);
}


void HAL_DWT_DelayMS(unsigned int uwDelay)
{
	unsigned int uwTimeDelay;
	unsigned int uwTimeStart = DWT->CYCCNT;
	
	do
	{
		uwTimeDelay = (DWT->CYCCNT - uwTimeStart) / (SystemCoreClock / 1000);
	}
	while(uwTimeDelay < uwDelay);
}


unsigned char HAL_DWT_TimeoutCheckUS(unsigned int uwTick, unsigned int uwTimeout)
{
	return ((DWT->CYCCNT - uwTick) < (SystemCoreClock / 1000000 * uwTimeout));
}


unsigned char HAL_DWT_TimeoutCheckMS(unsigned int uwTick, unsigned int uwTimeout)
{
	return ((DWT->CYCCNT - uwTick) < (SystemCoreClock / 1000 * uwTimeout));
}
