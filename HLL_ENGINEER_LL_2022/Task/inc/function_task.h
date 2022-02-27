#ifndef __FUNCTION_TASK_H
#define __FUNCTION_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t FunctionTask_Handler;
void Function_Task(void *pvParameters);
#endif
