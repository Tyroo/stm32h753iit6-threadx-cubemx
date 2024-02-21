/*
*********************************************************************************************************
*	                                  
*	模块名称 : 显示驱动
*	文件名称 : gx_display_driver_stm32F4_565rgb.h
*	版    本 : V1.0
*	说    明 : 底层驱动接口文件
*              
*	修改记录 :
*		版本号    日期         作者          说明
*		V1.0    2020-07-05   Eric2013  	    首版    
*                                     
*	Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __GX_DISPLAY_DRIVER_STM32H7_565RGB_H
#define __GX_DISPLAY_DRIVER_STM32H7_565RGB_H

#include "gx_user_generate_resources.h"
#include "gx_user_generate_specifications.h"

#define GX_CHROMEART_ENABLE					(1)		// 用于使能DMA2D加速
#define GX_FULL_CANVAS_PINGPONG_REFRESH		(1)		// 全屏画布乒乓刷新
#define GX_GUIX_THREAD_STACK_SIZE			(100)

#define GX_DISPLAY_SCREEN_WIDTH		(DISPLAY_SCREEN_1024X600_X_RESOLUTION)						// 显示屏的宽度
#define GX_DISPLAY_SCREEN_HEIGHT	(DISPLAY_SCREEN_1024X600_Y_RESOLUTION)						// 显示屏的高度

#define GX_DISPLAY_BUFFER_ADDR1		(0xC0000000)												// 显示缓冲区1地址
#define GX_DISPLAY_BUFFER_SIZE1		(GX_DISPLAY_SCREEN_WIDTH * GX_DISPLAY_SCREEN_HEIGHT * 2)	// 显示缓冲区1大小

#define GX_DISPLAY_BUFFER_ADDR2		(GX_DISPLAY_BUFFER_ADDR1 + GX_DISPLAY_BUFFER_SIZE1) 		// 显示缓冲区2地址
#define GX_DISPLAY_BUFFER_SIZE2		(GX_DISPLAY_BUFFER_SIZE1)									// 显示缓冲区2大小

#define GX_DISPLAY_CANVAS_ADDR		(GX_DISPLAY_BUFFER_ADDR2 + GX_DISPLAY_BUFFER_SIZE2)			// 画布显示缓冲区地址
#define GX_DISPLAY_CANVAS_SIZE		(GX_DISPLAY_BUFFER_SIZE1)									// 画布显示缓冲区地址

#define GX_SYSTEM_MEMORY_ADDR		(GX_DISPLAY_CANVAS_ADDR + GX_DISPLAY_CANVAS_SIZE)			// GUIX动态内存区首地址
#define GX_SYSTEM_MEMORY_SIZE		(1024 * 1024 * 4)											// GUIX动态内存区大小

void gx_display_init(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
