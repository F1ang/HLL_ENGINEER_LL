#ifndef __JUDGE_TASK_H
#define __JUDGE_TASK_H
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include "usart_hll.h"
#include "judge_system.h"
#include "detect_task.h"   
#include "crc_check.h"
#include "led.h"
#include "tim_hll.h"   
extern TaskHandle_t JudgeTask_Handler;

extern void Judge_Task(void *pvParameters);

extern void Notify_Judge_Task( uint32_t send_data );

#endif
