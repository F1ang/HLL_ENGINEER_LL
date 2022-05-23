#include "buzzer_task.h"

/* �������� */
TaskHandle_t BuzzerTask_Handler;
static QueueHandle_t buzzer_queue;

typedef struct
{
	uint8_t beep_time;    //�ε�����
	uint16_t pwm;         //����Ƶ��
	uint16_t delay_time;  //�εμ��εμ��ʱ��
}Beep_play_t;

void Buzzer_Task(void *pvParameters)
{
	//��Ϣ���г�ʼ��
	buzzer_queue = xQueueCreate( 3, sizeof( Beep_play_t ) );
	
	//��������������
	Beep_play_t beep_msg;
	
	//1����ʼ��
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

//���÷�����
BaseType_t Set_Beep_Time(uint8_t time, uint16_t pwm, uint16_t delay)
{
	Beep_play_t beep_msg_send;
	beep_msg_send.pwm = pwm;
	beep_msg_send.beep_time =1;//time->2������5�ƺ�������
	beep_msg_send.delay_time = delay;
	return xQueueSend(buzzer_queue, &beep_msg_send, 0);
}

