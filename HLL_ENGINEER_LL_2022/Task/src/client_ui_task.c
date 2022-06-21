/*
    二值信号量发送UI任务
*/
#include "client_ui_task.h"

TaskHandle_t Client_Ui_Task_Handler;  
static SemaphoreHandle_t ui_update_semaphore;
Judge_data_t *judge_data;
void Client_Ui_Task(void *pvParameters)
{	  
 	ui_update_semaphore = xSemaphoreCreateBinary();
	Start_UI();
	judge_data=Get_Judge_Data();    
	vTaskDelay(200);
	while(1)
	{ 
		if( xSemaphoreTake(ui_update_semaphore, 500) == pdTRUE )
		{
			Dynamic_UI();
			//printf("Dynamic_UI\r\n");
		}    
		else
		{
//			printf("client_task\r\n");
		}
		vTaskDelay(5); 
	}
	
	
}


void Ui_Update(void)
{
	xSemaphoreGive(ui_update_semaphore);//任务级
	//taskYIELD();//任务切换
}

