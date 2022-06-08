#ifndef __CLIENT_UI_TASK
#define __CLIENT_UI_TASK

 #include "FreeRTOS.h"
 #include "task.h"
 #include "semphr.h"
 #include "queue.h"
 #include "main.h"

 #include "usart_hll.h"
 #include "led.h"
 
#include "UI_Draw.h"
extern TaskHandle_t Client_Ui_Task_Handler;

void Ui_Update(void);
void Client_Ui_Task(void *pvParameters);

#endif
