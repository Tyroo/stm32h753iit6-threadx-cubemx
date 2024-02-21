#ifndef __USER_APP_H
#define __USER_APP_H


#include "tx_api.h"
#include "lvgl.h"
#include "gx_user_app.h"

extern TX_SEMAPHORE tx_lvgl_thread_semaphore;
extern TX_QUEUE		tx_guix_display_queue;

void UserApp_Startup_Init(void);

#endif