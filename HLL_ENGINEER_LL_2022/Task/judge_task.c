#include "judge_task.h"

#include <string.h>
//#include "usart8.h"
//#include "judge_system.h"
//#include "detect_task.h"

TaskHandle_t JudgeTask_Handler;

//static const uint8_t *judge_buf;
//static uint8_t judge_buf_copy[128];
//static uint32_t judge_buf_len = 0;

void Judge_Task(void *pvParameters)
{
//	judge_buf = Get_Judge_Buf();
//	
//	vTaskDelay(200);
//	
//	USART_ITConfig(UART8, USART_IT_IDLE, ENABLE);
//	
//	while(1)
//	{
//		//�ȴ�����֪ͨ
//		xTaskNotifyWait(0x00, 0xFFFFFFFF, &judge_buf_len, portMAX_DELAY);
//		
//		//�����ܵĵ�ԭʼ���ݿ���Ϊ����
//		memcpy(judge_buf_copy, judge_buf, judge_buf_len);
//		
//		//���²���ϵͳ״̬
//		Detect_Reload(5);
//		
//		//��һ���Ǵ�ӡ����ԭʼ����
//		//DEBUG_SHOWDATA1("judge_buf_len", judge_buf_len); for(u8 i=0; i<judge_buf_len; i++){DEBUG_PRINT("%d ", judge_buf_copy[i]);} DEBUG_PRINT("\r\n");
//		
//		//��������ϵͳ����
//		Analysis_Judge_System(judge_buf_copy, judge_buf_len);
//		
//	}
//	
//	//vTaskDelete(NULL);
//	
}



void Notify_Judge_Task( uint32_t send_data )
{	
//	/*��һ��������֪ͨ��xHandlingTask�Ǹ�����ľ����*/
//	xTaskNotifyFromISR(JudgeTask_Handler, send_data, eSetValueWithOverwrite, NULL);
}

