#ifndef __OVERCHIP_TASK_H
#define __OVERCHIP_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t OverchipTask_Handler;
void Overchip_Task(void *pvParameters);
#endif
