#ifndef __BUZZER_TASK
#define __BUZZER_TASK

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "main.h"

#include "usart_hll.h"
#include "buzzer.h"
#include "led.h"
#include "can2_motor.h"
extern TaskHandle_t BuzzerTask_Handler;

void Buzzer_Task(void *pvParameters);

BaseType_t Set_Beep_Time(uint8_t send_msg, uint16_t pwm, uint16_t time);

#endif
