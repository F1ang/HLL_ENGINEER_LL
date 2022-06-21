#ifndef __FUNCTION_TASK_H
#define __FUNCTION_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define Small_T_l_Max 8000
#define Small_T_l_Min 0

#define KA_Max 7120
#define KA_Min 0

extern TaskHandle_t FunctionTask_Handler;
void Function_Task(void *pvParameters);
#endif
