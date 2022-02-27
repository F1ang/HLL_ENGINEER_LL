#ifndef __CHASSIS_TASK_H
#define __CHASSIS_TASK_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t ChassisTask_Handler;

void Chassis_Task(void *pvParameters);

#endif

