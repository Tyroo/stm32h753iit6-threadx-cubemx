#ifndef __USER_APP_H
#define __USER_APP_H


#include "tx_api.h"
#include "lvgl.h"

extern TX_SEMAPHORE tx_lvgl_thread_semaphore;

void UserApp_Startup_Init(void);

#endif