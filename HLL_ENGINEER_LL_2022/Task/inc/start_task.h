#ifndef __START_TASK
#define __START_TASK

#include "FreeRTOS.h"
#include "task.h"
#include "usart_hll.h"
#include "remoter_task.h"
#include "judge_task.h"
#include "led.h"
extern TaskHandle_t StartTask_Handler;

void Start_Task(void *pvParameters);

#endif

