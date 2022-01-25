#include "start_task.h"

TaskHandle_t StartTask_Handler;

void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区
	
	
	
	
  taskEXIT_CRITICAL(); //退出临界区
	vTaskDelete(NULL);  //删除开始任务
}
