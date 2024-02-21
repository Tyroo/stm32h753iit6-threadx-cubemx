/*
*********************************************************************************************************
*	                                  
*	模块名称 : 显示驱动
*	文件名称 : gx_display_driver_stm32h7_565rgb.h
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
#include   "tx_api.h"
#include   "gx_api.h"
#include   "gx_system.h"
#include   "gx_display.h"
#include   "gx_utility.h"
#include   "gx_display_driver_stm32h7_565rgb.h"

#include "ft5x06.h"
#include "stm32h7xx_hal.h"

/*
*********************************************************************************************************
*                                           变量和函数
*********************************************************************************************************
*/
static GX_WINDOW		*gx_window;
static GX_WINDOW_ROOT	*gx_window_root;

static TX_THREAD       	tx_guix_display_driver_thread;
static UCHAR			tx_guix_display_driver_thread_stack[GX_GUIX_DISPLAY_DRIVER_THREAD_STACK_SIZE];

static TX_THREAD		tx_guix_touchpad_event_thread;
static UCHAR			tx_guix_touchpad_event_thread_stack[GX_GUIX_TOUCHPAD_EVENT_THREAD_STACK_SIZE];

	   TX_QUEUE			tx_guix_display_queue;
static TX_BYTE_POOL		tx_guix_system_memory_pool;

/*
*********************************************************************************************************
*	函 数 名: gx_chromeart_horizontal_line_draw
*	功能说明: 单色线填充
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
static VOID gx_chromeart_horizontal_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos, INT width, GX_COLOR color)
{
    uint32_t  put;
    int length;
    GX_CANVAS *canvas = context->gx_draw_context_canvas;

    put = (uint32_t) canvas->gx_canvas_memory;
    put += (canvas->gx_canvas_x_resolution * 2 * ypos) + (xstart * 2);

    length = xend - xstart + 1;
	
	SCB_CleanInvalidateDCache();
	
	while(DMA2D->CR & DMA2D_CR_START);

	DMA2D->CR = DMA2D_R2M;
	DMA2D->OCOLR = color;

	/* 输出层 */
	DMA2D->OMAR = (uint32_t)put;
	DMA2D->OOR = canvas->gx_canvas_x_resolution - length;
	DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

	DMA2D->NLR = (uint32_t)(length << 16) | (uint16_t)width;
	
	DMA2D->CR |= DMA2D_CR_START;
}

/*
*********************************************************************************************************
*	函 数 名: gx_chromeart_vertical_line_draw
*	功能说明: 单色线填充
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
static VOID gx_chromeart_vertical_line_draw(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos, INT width, GX_COLOR color)
{
    uint32_t  put;
    GX_CANVAS *canvas = context->gx_draw_context_canvas;
  
    put = (uint32_t) canvas->gx_canvas_memory;

    put += (canvas->gx_canvas_x_resolution * 2 * ystart) + (xpos * 2);

	SCB_CleanInvalidateDCache();
	
	while(DMA2D->CR & DMA2D_CR_START);
	
	DMA2D->CR = DMA2D_R2M;
	DMA2D->OCOLR = color;

	/* 输出层 */
	DMA2D->OMAR = (uint32_t)put;
	DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
	DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

	DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)(yend - ystart + 1);
	DMA2D->CR |= DMA2D_CR_START;
}

/*
*********************************************************************************************************
*	函 数 名: gx_chromeart_canvas_copy
*	功能说明: canvas复制加速
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
	GX_RECTANGLE dirty;
	GX_RECTANGLE overlap;
	USHORT      *read;
	USHORT      *write;
	INT          width;

	
    dirty.gx_rectangle_left = dirty.gx_rectangle_top = 0;
    dirty.gx_rectangle_right = canvas -> gx_canvas_x_resolution - 1;
    dirty.gx_rectangle_bottom = canvas -> gx_canvas_y_resolution - 1;

    _gx_utility_rectangle_shift(&dirty, canvas -> gx_canvas_display_offset_x, canvas -> gx_canvas_display_offset_y);

    if (_gx_utility_rectangle_overlap_detect(&dirty, &composite -> gx_canvas_dirty_area, &overlap))
    {
        width = overlap.gx_rectangle_right - overlap.gx_rectangle_left + 1;
        read = (USHORT *)canvas -> gx_canvas_memory;

        read += (overlap.gx_rectangle_top - dirty.gx_rectangle_top) * canvas -> gx_canvas_x_resolution;
        read += overlap.gx_rectangle_left - dirty.gx_rectangle_left;

        write = (USHORT *)composite -> gx_canvas_memory;
        write += overlap.gx_rectangle_top * composite -> gx_canvas_x_resolution;
        write += overlap.gx_rectangle_left;

		SCB_CleanInvalidateDCache();
		
		while(DMA2D->CR & DMA2D_CR_START);
		
		/* 前景层 */
		DMA2D->CR = DMA2D_M2M;
		DMA2D->FGMAR = (uint32_t) read;             
		DMA2D->FGOR = canvas->gx_canvas_x_resolution - width; 
		DMA2D->FGPFCCR = (0xff000000) | LTDC_PIXEL_FORMAT_RGB565; 
	
		/* 输出层 */
		DMA2D->OMAR = (uint32_t)write;
		DMA2D->OOR = GX_DISPLAY_SCREEN_WIDTH - width;
		DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
		
		DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)(overlap.gx_rectangle_bottom - overlap.gx_rectangle_top + 1);
		DMA2D->CR |= DMA2D_CR_START;
    }
}

/*
*********************************************************************************************************
*	函 数 名: gx_chromeart_pixelmap_draw
*	功能说明: pixelmap 绘制加速 
*	形    参: --
*	返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_pixelmap_draw(GX_DRAW_CONTEXT *context,
                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
	INT           width;
	INT           height;
	USHORT       *putrow;
	USHORT       *getshort;
	ULONG        *getlong;
	GX_CANVAS    *canvas;
	ULONG         reg_val;
	uint32_t      dma2d_mode;

    /* 如果pixelmap是压缩的，采用软件方式绘制 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        _gx_display_driver_565rgb_pixelmap_draw(context, xpos, ypos, pixelmap);
        return;
    }

    /* 如果是带alpha通道的alpha格式，也需要用软件方式绘制 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA &&
        pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        _gx_display_driver_565rgb_pixelmap_draw(context, xpos, ypos, pixelmap);
        return;
    }

    /* 如果是pixelmap原始数据，使用DMA2D加速 */
    GX_RECTANGLE *clip = context -> gx_draw_context_clip;
    canvas = context->gx_draw_context_canvas;

    putrow = (USHORT *)context -> gx_draw_context_memory;
    putrow += clip -> gx_rectangle_top * context -> gx_draw_context_pitch;
    putrow += clip -> gx_rectangle_left;

	if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        getshort = (USHORT *)(pixelmap -> gx_pixelmap_data);
        getshort += pixelmap -> gx_pixelmap_width * (clip -> gx_rectangle_top - ypos);
        getshort += (clip -> gx_rectangle_left - xpos);
        reg_val = (uint32_t) getshort;           
    }
    else
    {
        getlong = (ULONG *)(pixelmap -> gx_pixelmap_data);
        getlong += pixelmap -> gx_pixelmap_width * (clip -> gx_rectangle_top - ypos);
        getlong += (clip -> gx_rectangle_left - xpos);
        reg_val = (uint32_t) getlong;             
    }
	
    width = clip->gx_rectangle_right - clip -> gx_rectangle_left + 1;
    height = clip->gx_rectangle_bottom - clip->gx_rectangle_top + 1;
	
	SCB_CleanInvalidateDCache();
	
	while(DMA2D->CR & DMA2D_CR_START);
	
	DMA2D->FGMAR = reg_val;

    if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        reg_val = LTDC_PIXEL_FORMAT_RGB565;
    }
    else
    {
        reg_val = LTDC_PIXEL_FORMAT_ARGB8888;
    }

	/* 前景层 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        reg_val |= 0xff000000;
        DMA2D->FGPFCCR = reg_val;
        dma2d_mode = DMA2D_M2M_BLEND;       
    }
    else
    {
        reg_val |= 0xff010000;  
        DMA2D->FGPFCCR = reg_val;
        dma2d_mode = DMA2D_M2M;       
    }

	/* 前景层 */
	DMA2D->CR = dma2d_mode;         
	DMA2D->FGOR = pixelmap->gx_pixelmap_width - width;  

	/* 背景层 */
	if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
	{
		DMA2D->BGMAR = (uint32_t) putrow;       
		DMA2D->BGOR = canvas->gx_canvas_x_resolution - width; 
		DMA2D->BGPFCCR = 0xff010002;
	}

	/* 输出层 */
	DMA2D->OMAR = (uint32_t)putrow;
	DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
	DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

	DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)height;
	DMA2D->CR |= DMA2D_CR_START;
}

/*
*********************************************************************************************************
*	函 数 名: gx_chromeart_pixelmap_blend
*	功能说明: pixelmap blend 加速
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap, UCHAR alpha)
{
	INT           width;
	INT           height;
	USHORT       *putrow;
	USHORT       *getshort;
	ULONG        *getlong;
	GX_CANVAS    *canvas;
	ULONG        blend_reg;

     /* 如果pixelmap是压缩的，采用软件方式绘制 */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        _gx_display_driver_565rgb_pixelmap_blend(context, xpos, ypos, pixelmap, alpha);
        return;
    }
	
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA &&
        pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        _gx_display_driver_565rgb_pixelmap_blend(context, xpos, ypos, pixelmap, alpha);
        return;
    }

    /* 使用DMA2D加速绘制 */
    GX_RECTANGLE *clip = context -> gx_draw_context_clip;
    canvas = context->gx_draw_context_canvas;

    putrow = (USHORT *)context -> gx_draw_context_memory;
    putrow += clip -> gx_rectangle_top * context->gx_draw_context_pitch;
    putrow += clip -> gx_rectangle_left;

    if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        getshort = (USHORT *)(pixelmap -> gx_pixelmap_data);
        getshort += pixelmap -> gx_pixelmap_width * (clip -> gx_rectangle_top - ypos);
        getshort += (clip -> gx_rectangle_left - xpos);
    }
    else
    {
        getlong = (ULONG *)(pixelmap -> gx_pixelmap_data);
        getlong += pixelmap -> gx_pixelmap_width * (clip -> gx_rectangle_top - ypos);
        getlong += (clip -> gx_rectangle_left - xpos);
    }

    width = clip->gx_rectangle_right - clip -> gx_rectangle_left + 1;
    height = clip->gx_rectangle_bottom - clip->gx_rectangle_top + 1;
	
    blend_reg = alpha;
    blend_reg <<= 24;
	
	if (pixelmap->gx_pixelmap_format == GX_COLOR_FORMAT_565RGB)
    {
        DMA2D->FGMAR = (uint32_t) getshort;             
        blend_reg |= LTDC_PIXEL_FORMAT_RGB565;
    }
    else
    {
        DMA2D->FGMAR = (uint32_t) getlong;             
        blend_reg |= LTDC_PIXEL_FORMAT_ARGB8888;
    }

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        blend_reg |= 0x00020000;
    }
    else
    {
        blend_reg |= 0x00010000;
    }

	DMA2D->CR = 0x00020000UL | (1 << 9);

	/* 前景层 */
	DMA2D->FGOR = pixelmap->gx_pixelmap_width - width; 
	DMA2D->FGPFCCR = blend_reg;
	
	/* 背景层 */
	DMA2D->BGMAR = (uint32_t) putrow;
	DMA2D->BGOR = canvas->gx_canvas_x_resolution - width; 
	DMA2D->BGPFCCR = 0xff010002;

	/* 输出层 */
	DMA2D->OMAR = (uint32_t)putrow;
	DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
	DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

	DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)height;
	
	SCB_CleanInvalidateDCache();

	while(DMA2D->CR & DMA2D_CR_START);

	DMA2D->CR |= DMA2D_CR_START;
}

/*
*********************************************************************************************************
*	函 数 名: gx_chromeart_glyph_8bit_draw
*	功能说明: glyph 8bit 加速
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void gx_chromeart_glyph_8bit_draw(GX_DRAW_CONTEXT *context, GX_RECTANGLE *draw_area,
                                         GX_POINT *map_offset, const GX_GLYPH *glyph)
{
	GX_CANVAS  *canvas;
	GX_UBYTE   *read_glyph;
	USHORT     *write_row;
	GX_COLOR    text_color;
	UINT        height;
	UINT        width;
	UCHAR       red, green, blue;
	GX_UBYTE    alpha;

    text_color =  context -> gx_draw_context_brush.gx_brush_line_color;

    alpha = context ->gx_draw_context_brush.gx_brush_alpha;

    if (alpha == 0) return;

    red = (text_color & 0xf800) >> 8;
    green = (text_color & 0x07e0) >> 3;
    blue = (text_color & 0x001f) << 3;
    text_color = (red << 16) | (green << 8) | blue;
    width = draw_area -> gx_rectangle_right - draw_area -> gx_rectangle_left + 1;

    canvas = context->gx_draw_context_canvas;

    read_glyph = (GX_UBYTE *)glyph -> gx_glyph_map;

    if (map_offset -> gx_point_y)
    {
        read_glyph = read_glyph + (glyph -> gx_glyph_width * map_offset -> gx_point_y);
    }

    read_glyph += map_offset -> gx_point_x;

    width = draw_area->gx_rectangle_right - draw_area->gx_rectangle_left + 1;
    height = draw_area -> gx_rectangle_bottom - draw_area -> gx_rectangle_top + 1;

    write_row = (USHORT *)canvas->gx_canvas_memory;
    write_row += draw_area->gx_rectangle_top * context->gx_draw_context_pitch;
    write_row += draw_area->gx_rectangle_left;
   
	DMA2D->CR = DMA2D_M2M_BLEND;

	/* 前景层 */
	DMA2D->FGMAR = (uint32_t)read_glyph;
	DMA2D->FGOR = glyph->gx_glyph_width - width; 
	DMA2D->FGCOLR = text_color;
	DMA2D->FGPFCCR =(alpha << 24) | 0x00020009;   /* 原始前景层图像的 alpha 通道值替换为当前配置的与原始 alpha 通道值的乘积，AL88 */

	/* 背景层 */
	DMA2D->BGMAR = (uint32_t) write_row;         
	DMA2D->BGOR = canvas->gx_canvas_x_resolution - width;  
	DMA2D->BGPFCCR = 0xFF000002;	              /* 不修改背景层图像的 alpha 通道值，RGB565 */
	
	/* 输出层 */
	DMA2D->OMAR = (uint32_t)write_row;
	DMA2D->OOR = canvas->gx_canvas_x_resolution - width;
	DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;

	DMA2D->NLR = (uint32_t)(width << 16) | (uint16_t)height;

	SCB_CleanInvalidateDCache();

	while(DMA2D->CR & DMA2D_CR_START);

	DMA2D->CR |= DMA2D_CR_START;
}

/*
*********************************************************************************************************
*	函 数 名: stm32h7_565rgb_buffer_toggle
*	功能说明: 更新canvas内容到LCD显存
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if GX_FULL_CANVAS_PINGPONG_REFRESH && GX_CHROMEART_ENABLE

static void gx_display_buffer_565rgb_toggle(ULONG thread_input)
{
	ULONG tx_guix_display_queue_msg;
	
	for( ; ; )
	{
		if (tx_queue_receive(&tx_guix_display_queue, &tx_guix_display_queue_msg, TX_WAIT_FOREVER) == TX_SUCCESS)
		{
			while(DMA2D->CR & DMA2D_CR_START) 
				tx_thread_sleep(1);
			
			DMA2D->OMAR = (uint32_t)tx_guix_display_queue_msg;
			DMA2D->CR = 0x00000000UL | (1 << 9);
			DMA2D->FGMAR = (uint32_t)GX_DISPLAY_CANVAS_ADDR;
			DMA2D->FGOR = 0;
			DMA2D->OOR = 0;
			
			/* 前景层和输出区域都采用RGB565颜色格式 */
			DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;
			DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
			DMA2D->NLR = (uint32_t)(GX_DISPLAY_SCREEN_WIDTH << 16) | (uint16_t)(GX_DISPLAY_SCREEN_HEIGHT);
			DMA2D->CR |= DMA2D_CR_START;
		}
	}
}

#else

static void gx_display_buffer_565rgb_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
	GX_RECTANGLE    Limit;
	GX_RECTANGLE    Copy;
	ULONG           offset;
	INT             copy_width;
	INT             copy_height;

#if !defined(GX_CHROMEART_ENABLE)
	INT             row;
	INT             src_stride_ulongs;
	INT             dest_stride_ulongs;
#endif

	ULONG *get;
	ULONG *put;

	gx_utility_rectangle_define(&Limit, 0, 0,
						canvas->gx_canvas_x_resolution - 1,
						canvas->gx_canvas_y_resolution - 1);

	if (gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
	{
		Copy.gx_rectangle_left &= 0xfffe;
		Copy.gx_rectangle_right |= 0x01;
		copy_width = Copy.gx_rectangle_right - Copy.gx_rectangle_left + 1;
		copy_height = Copy.gx_rectangle_bottom - Copy.gx_rectangle_top + 1;

		/* 从canvas读取更新区 */
        offset = Copy.gx_rectangle_top * canvas->gx_canvas_x_resolution;
        offset += Copy.gx_rectangle_left;
        offset /= 2;
        get = canvas ->gx_canvas_memory;
        get += offset;

		/* 从LCD显存读取要更新的区域，将canvas更新的数据复制进来 */
		put = (ULONG *)GX_DISPLAY_BUFFER_ADDR1;
		offset = (canvas->gx_canvas_display_offset_y + Copy.gx_rectangle_top) * GX_DISPLAY_SCREEN_WIDTH;
		offset += canvas->gx_canvas_display_offset_x + Copy.gx_rectangle_left;
		offset /= 2;
		put += offset;

#if !defined(GX_CHROMEART_ENABLE)	
		src_stride_ulongs = canvas ->gx_canvas_x_resolution / 2;
		dest_stride_ulongs = g_LcdWidth / 2;
		copy_width /= 2; 
		
		for(row = 0; row < copy_height; row++)
		{
			memcpy(put, get, copy_width * 4);
			put += dest_stride_ulongs;
			get += src_stride_ulongs;
		}
#else	
		DMA2D->CR = 0x00000000UL | (1 << 9);
		DMA2D->FGMAR = (uint32_t)get;
		DMA2D->OMAR = (uint32_t)put;
		DMA2D->FGOR = canvas->gx_canvas_x_resolution - copy_width;
		DMA2D->OOR = GX_DISPLAY_SCREEN_WIDTH - copy_width;

		/* 前景层和输出区域都采用RGB565颜色格式 */
		DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;
		DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
		DMA2D->NLR = (uint32_t)(copy_width << 16) | (uint16_t)copy_height;
		
		while(DMA2D->CR & DMA2D_CR_START);
		
		DMA2D->CR |= DMA2D_CR_START;
#endif
	}
}

#endif

/*
*********************************************************************************************************
*	函 数 名: gx_display_driver_565rgb_setup
*	功能说明: 驱动接口函数
*	形    参: ---
*	返 回 值: GX_SUCCESS
*********************************************************************************************************
*/
static UINT gx_display_driver_565rgb_setup(GX_DISPLAY *display)
{
#if GX_FULL_CANVAS_PINGPONG_REFRESH && GX_CHROMEART_ENABLE
	
	VOID * tx_display_queue_memptr;
	
	_gx_display_driver_565rgb_setup(display, (VOID*)GX_DISPLAY_BUFFER_ADDR1, NULL);
	
	tx_byte_allocate(&tx_guix_system_memory_pool, &tx_display_queue_memptr, sizeof(ULONG), TX_NO_WAIT);
	
	tx_queue_create(&tx_guix_display_queue, "GUIX Display driver queue", sizeof(ULONG), tx_display_queue_memptr, sizeof(ULONG));
	
	tx_thread_create(&tx_guix_display_driver_thread, "GUIX Display driver thread", gx_display_buffer_565rgb_toggle, 0,  
		tx_guix_display_driver_thread_stack, GX_GUIX_DISPLAY_DRIVER_THREAD_STACK_SIZE, 0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);
#else
    _gx_display_driver_565rgb_setup(display, (VOID*)GX_DISPLAY_BUFFER_ADDR1, gx_display_buffer_565rgb_toggle);
#endif

#if defined(GX_CHROMEART_ENABLE)	
    display -> gx_display_driver_pixelmap_blend     = gx_chromeart_pixelmap_blend;
	display -> gx_display_driver_pixelmap_draw      = gx_chromeart_pixelmap_draw;
	display -> gx_display_driver_canvas_copy        = gx_chromeart_canvas_copy;

	display->gx_display_driver_horizontal_line_draw = gx_chromeart_horizontal_line_draw;
    display -> gx_display_driver_vertical_line_draw = gx_chromeart_vertical_line_draw;
	display -> gx_display_driver_8bit_glyph_draw    = gx_chromeart_glyph_8bit_draw;
#endif

    return(GX_SUCCESS);
}

static VOID * gx_system_memory_alloc(ULONG size)
{
    VOID * memptr;

    if (tx_byte_allocate(&tx_guix_system_memory_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }
    return NULL;
}

static void gx_system_memory_free(VOID *mem)
{
    tx_byte_release(mem);
}

static void gx_system_memory_setup(void)
{
	/* 初始化内存池 */
    tx_byte_pool_create(&tx_guix_system_memory_pool, "GX_SYSTEM_MEMORY_POOL", (VOID *)GX_SYSTEM_MEMORY_ADDR,  GX_SYSTEM_MEMORY_SIZE);
    /* 注册动态内存申请和释放函数 */
    _gx_system_memory_allocator_set(gx_system_memory_alloc, gx_system_memory_free);
}


static void gx_touchpad_event_process(ULONG thread_input)
{
	GX_EVENT touchpad_event;
	
	UCHAR touchpad_state_old = 1;
	UCHAR touchpad_state_new = 1;
	
	for( ; ; )
	{
		touchpad_state_new = FT5X06_GET_PIN_PEN();
		
		if (touchpad_state_new != touchpad_state_old)
		{
			if (touchpad_state_new)
			{
				touchpad_event.gx_event_type = GX_EVENT_PEN_UP;
			}
			else
			{
				touchpad_event.gx_event_type = GX_EVENT_PEN_DOWN;
			}
			
			touchpad_event.gx_event_sender = 0;
			touchpad_event.gx_event_target = 0;
			touchpad_event.gx_event_display_handle = GX_DISPLAY_BUFFER_ADDR1;
			
			FT5X06_GetTouchPoint((unsigned short *)&touchpad_event.gx_event_payload.gx_event_pointdata.gx_point_x,
				(unsigned short *)&touchpad_event.gx_event_payload.gx_event_pointdata.gx_point_y);
			
			gx_system_event_send(&touchpad_event);
		}
		
		if (!touchpad_state_old)
		{
			touchpad_event.gx_event_type = GX_EVENT_PEN_DRAG;
			touchpad_event.gx_event_sender = 0;
			touchpad_event.gx_event_target = 0;
			touchpad_event.gx_event_display_handle = GX_DISPLAY_BUFFER_ADDR1;
			
			FT5X06_GetTouchPoint((unsigned short *)&touchpad_event.gx_event_payload.gx_event_pointdata.gx_point_x,
				(unsigned short *)&touchpad_event.gx_event_payload.gx_event_pointdata.gx_point_y);
			
			gx_system_event_fold(&touchpad_event);
		}
			
		touchpad_state_old = touchpad_state_new;
		
		tx_thread_sleep(20);
	}
}


static void gx_touchpad_event_setup(void)
{
	tx_thread_create(&tx_guix_touchpad_event_thread, "GUIX touchpad event thread", gx_touchpad_event_process, 0,  
		tx_guix_touchpad_event_thread_stack, GX_GUIX_TOUCHPAD_EVENT_THREAD_STACK_SIZE, 0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);
}


void gx_display_init(void)
{
	/* 初始化GUIX */
    gx_system_initialize();
	
	/* 初始化GUIX动态内存区 */
	gx_system_memory_setup();
	
	/* 创建GUIX触摸板事件处理线程 */
	gx_touchpad_event_setup();
	
	/* 配置显示屏 */
    gx_studio_display_configure(DISPLAY_SCREEN_1024X600, gx_display_driver_565rgb_setup,
		LANGUAGE_ENGLISH, DISPLAY_SCREEN_1024X600_THEME_1, &gx_window_root);
	
    /* 创建窗口 */
    gx_studio_named_widget_create("window", (GX_WIDGET *)gx_window_root, (GX_WIDGET **)&gx_window);

	/* 显示根窗口 */
    gx_widget_show(gx_window_root);

    /* 启动GUIX */
    gx_system_start();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
