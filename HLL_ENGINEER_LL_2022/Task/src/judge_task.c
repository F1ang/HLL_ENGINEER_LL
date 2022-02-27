#include "judge_task.h"
#include <string.h>

TaskHandle_t JudgeTask_Handler;

static const uint8_t *judge_buf;
static uint8_t judge_buf_copy[128];
static uint32_t judge_buf_len = 0;


//debug
extern M550_Mileage motor550_s;

void Judge_Task(void *pvParameters)
{
	judge_buf = Get_Judge_Buf();  
	//3、初始化
	usart6_base_init();
	vTaskDelay(200);

//	LL_USART_EnableIT_IDLE(USART6);
	
	while(1)
	{
		
		//debug
		LED_GREEN_ON;
		vTaskDelay(200);
		LED_GREEN_OFF;
		vTaskDelay(200);
		//debug
		printf("%d\r\n",motor550_s.speed_rpm);
//		PI5_PWM_OUT(950);
	
//		PB0_PWM_OUT(900);
		
		
		//LL_mDelay(200);
		//buzzer声沙哑，猜测是没快速进行任务切换，后期是否可以移到Function_task
		
		//等待任务通知 portMAX_DELAY
//		xTaskNotifyWait(0x00, 0xFFFFFFFF, &judge_buf_len, 10);
		
		//将接受的的原始数据拷贝为副本
		memcpy(judge_buf_copy, judge_buf, judge_buf_len);
		
		//更新裁判系统状态
		Detect_Reload(5);  
		
//		//下一行是打印所有原始数据
//		DEBUG_SHOWDATA1("judge_buf_len", judge_buf_len); for(u8 i=0; i<judge_buf_len; i++){DEBUG_PRINT("%d ", judge_buf_copy[i]);} DEBUG_PRINT("\r\n");
//		
		//解析裁判系统数据
		Analysis_Judge_System(judge_buf_copy, judge_buf_len);  
		
	}
	
	//vTaskDelete(NULL);
	
}



void Notify_Judge_Task( uint32_t send_data )
{	
	/*向一个任务发送通知，xHandlingTask是该任务的句柄。*/
	xTaskNotifyFromISR(JudgeTask_Handler, send_data, eSetValueWithOverwrite, NULL);
}

