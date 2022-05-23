#include "buzzer_task.h"

/* 变量定义 */
TaskHandle_t BuzzerTask_Handler;
static QueueHandle_t buzzer_queue;

typedef struct
{
	uint8_t beep_time;    //滴滴数量
	uint16_t pwm;         //声音频率
	uint16_t delay_time;  //滴滴及滴滴间隔时间
}Beep_play_t;

void Buzzer_Task(void *pvParameters)
{
	//消息队列初始化
	buzzer_queue = xQueueCreate( 3, sizeof( Beep_play_t ) );
	
	//蜂鸣器播放数据
	Beep_play_t beep_msg;
	
	//1、初始化
	Buzzer_Init();
	vTaskDelay(200);
	

	while(1)
	{ 
		if( xQueueReceive(buzzer_queue, &beep_msg, portMAX_DELAY) == pdPASS)
		{
			for(u8 i=0; i<beep_msg.beep_time; i++)
			{
				BUZZER_ON(beep_msg.pwm);
				vTaskDelay(beep_msg.delay_time);
				
				BUZZER_OFF();
				vTaskDelay(beep_msg.delay_time);
			}
		}
		else
		{
			DEBUG_LOG("Beep task error!!!\r\n");
		}
		
	}
	
	
}

//设置蜂鸣器
BaseType_t Set_Beep_Time(uint8_t time, uint16_t pwm, uint16_t delay)
{
	Beep_play_t beep_msg_send;
	beep_msg_send.pwm = pwm;
	beep_msg_send.beep_time =1;//time->2，大于5似乎会阻塞
	beep_msg_send.delay_time = delay;
	return xQueueSend(buzzer_queue, &beep_msg_send, 0);
}

