#include "start_task.h"

TaskHandle_t StartTask_Handler;

void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL();           //�����ٽ���
	//����ң������
	xTaskCreate((TaskFunction_t )Remoter_Task,             
							(const char*    )"remoter_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )15,
							(TaskHandle_t*  )&RemoterTask_Handler);
	

	
	//��������������
	xTaskCreate((TaskFunction_t )Buzzer_Task,  
							(const char*    )"buzzer_task",
							(uint16_t       )128,
							(void*          )NULL,
							(UBaseType_t    )3,
							(TaskHandle_t*  )&BuzzerTask_Handler);

						
	//������������
	xTaskCreate((TaskFunction_t )Chassis_Task,  
							(const char*    )"chassis_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )20,
							(TaskHandle_t*  )&ChassisTask_Handler);
							
	//������������					
  xTaskCreate((TaskFunction_t )Function_Task,
							(const char*    )"function_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )19,
							(TaskHandle_t*  )&FunctionTask_Handler);
	//��ת����						
	xTaskCreate((TaskFunction_t )Overchip_Task,
							(const char*    )"overchip_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )19,
							(TaskHandle_t*  )&OverchipTask_Handler);
							
							
	//��������ϵͳ����
	xTaskCreate((TaskFunction_t )Judge_Task,
							(const char*    )"judge_task",   
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )9,
							(TaskHandle_t*  )&JudgeTask_Handler);
//							
	
	//������������
	xTaskCreate((TaskFunction_t )Detect_Task,     
							(const char*    )"detect_task",   
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )24,
							(TaskHandle_t*  )&DetectTask_Handler);
//	
//	//��������CPUʹ��������
//	#if CHECK_CPU_USE
//		xTaskCreate((TaskFunction_t )CPU_Task, /* ������ں��� */
//								(const char*    )"CPU_Task",/* �������� */
//								(uint16_t       )512,   /* ����ջ��С */
//								(void*          )NULL,	/* ������ں������� */
//								(UBaseType_t    )25,	    /* ��������ȼ� */
//								(TaskHandle_t*  )&CPU_Task_Handle);/* ������ƿ�ָ�� */
//	#endif
								
	Led_Flow_Off();  
	INFO_LOG("\r\nAll tasks creat.\r\n");      
	INFO_LOG("\r\nSystem run.\r\n");
  taskEXIT_CRITICAL(); //�˳��ٽ���
	vTaskDelete(NULL);  //ɾ����ʼ����
}
