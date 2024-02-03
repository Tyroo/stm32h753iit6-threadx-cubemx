#include "user_app.h"


#define TX_LVGL_THREAD_STACK_SIZE		(2048)


TX_THREAD               tx_lvgl_thread;
TX_SEMAPHORE			tx_lvgl_thread_semaphore;
UCHAR                   tx_lvgl_thread_stack[TX_LVGL_THREAD_STACK_SIZE];


static void UserApp_Startup_LvglThread(ULONG thread_input)
{
	for( ; ; )
	{
		lv_timer_handler();
		tx_thread_sleep(1);
	}
}


void UserApp_Startup_Init(void)
{
	lv_init();
	lv_port_disp_init(); 
	lv_port_indev_init();
	
	lv_demo_widgets();
	
	tx_semaphore_create(&tx_lvgl_thread_semaphore, "LvglThreadSemaphore", 0);
	
    tx_thread_create(&tx_lvgl_thread, "LvglThread", UserApp_Startup_LvglThread, 0,  
		tx_lvgl_thread_stack, TX_LVGL_THREAD_STACK_SIZE, 0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);
}


