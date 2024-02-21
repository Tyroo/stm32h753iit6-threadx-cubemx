#ifndef __USER_APP_H
#define __USER_APP_H


#include "tx_api.h"
#include "lvgl.h"
#include "gx_user_app.h"

extern TX_SEMAPHORE tx_lvgl_thread_semaphore;

extern TX_SEMAPHORE	tx_guix_display_driver_semaphore;
extern volatile ULONG tx_guix_display_driver_output_buff;

void UserApp_Startup_Init(void);

#endif