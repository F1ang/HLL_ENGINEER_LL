#include "start_task.h"

TaskHandle_t StartTask_Handler;

void Start_Task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区
	//创建遥控任务
	xTaskCreate((TaskFunction_t )Remoter_Task,             
							(const char*    )"remoter_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )15,
							(TaskHandle_t*  )&RemoterTask_Handler);
	

	
	//创建蜂鸣器任务
	xTaskCreate((TaskFunction_t )Buzzer_Task,  
							(const char*    )"buzzer_task",
							(uint16_t       )128,
							(void*          )NULL,
							(UBaseType_t    )3,
							(TaskHandle_t*  )&BuzzerTask_Handler);

						
	//创建底盘任务
	xTaskCreate((TaskFunction_t )Chassis_Task,  
							(const char*    )"chassis_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )20,
							(TaskHandle_t*  )&ChassisTask_Handler);
							
	//创建功能任务					
  xTaskCreate((TaskFunction_t )Function_Task,
							(const char*    )"function_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )19,
							(TaskHandle_t*  )&FunctionTask_Handler);
	//反转任务						
	xTaskCreate((TaskFunction_t )Overchip_Task,
							(const char*    )"overchip_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )19,
							(TaskHandle_t*  )&OverchipTask_Handler);
							
							
	//创建裁判系统任务
	xTaskCreate((TaskFunction_t )Judge_Task,
							(const char*    )"judge_task",   
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )9,
							(TaskHandle_t*  )&JudgeTask_Handler);
//							
	
	//创建监听任务
	xTaskCreate((TaskFunction_t )Detect_Task,     
							(const char*    )"detect_task",   
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )24,
							(TaskHandle_t*  )&DetectTask_Handler);
//	
//	//创建监听CPU使用率任务
//	#if CHECK_CPU_USE
//		xTaskCreate((TaskFunction_t )CPU_Task, /* 任务入口函数 */
//								(const char*    )"CPU_Task",/* 任务名字 */
//								(uint16_t       )512,   /* 任务栈大小 */
//								(void*          )NULL,	/* 任务入口函数参数 */
//								(UBaseType_t    )25,	    /* 任务的优先级 */
//								(TaskHandle_t*  )&CPU_Task_Handle);/* 任务控制块指针 */
//	#endif
								
	Led_Flow_Off();  
	INFO_LOG("\r\nAll tasks creat.\r\n");      
	INFO_LOG("\r\nSystem run.\r\n");
  taskEXIT_CRITICAL(); //退出临界区
	vTaskDelete(NULL);  //删除开始任务
}
