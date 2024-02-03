#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "print.h"


#if PRINT_PRINTF_FUNC_USE

#if defined(__clang__)

__ASM(".global __use_no_semihosting\n\t");

#else

#pragma import(__use_no_semihosting)

#endif
               
FILE __stdout;       

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 

}

void _ttywrch(int ch)
{
	ch = ch;
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	// 循环发送,直到发送完毕
	while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) == RESET);
	
	USART1->TDR = ch;
	
	return ch;
}

#endif


void Print(const char * str)
{
	HAL_UART_Transmit(&huart1, (const uint8_t *)str, strlen(str), 200);
}


void Input(void)
{

}
