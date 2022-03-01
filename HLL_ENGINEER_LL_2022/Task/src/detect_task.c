#include "detect_task.h"   

#include "listen.h"
#include "led.h"
#include "can1_motor.h"
#include "judge_system.h"

#include "remoter_task.h"
#include "usart_hll.h" 
#define MODULE_REONLINE(index) (module_status[index].time_out_flag==0 && module_status[index].old_time_out_flag==1)
#define MODULE_OFFLINE(index)  (module_status[index].time_out_flag==1 && module_status[index].old_time_out_flag==0)

  
//extern u16 rc_cnt;

enum errorList
{
	remote_control = 0u, //遥控器
	chassis_motor,       //底盘电机
	//gimbal_motor,        //云台电机
	//shooter_motor,       //发射机构电机
	//auto_aim,            //自瞄NUC
	judge_system,        //裁判系统
	//super_capacitor,     //超级电容
	//gyroscope            //陀螺仪
};

TaskHandle_t DetectTask_Handler;

static void Detect_Task_Init(void);

static Module_status_t module_status[8];
//static const Judge_data_t *judge_data;

void Detect_Task(void *pvParameters)
{
//	u8 cap_send_cnt = 0;
//	judge_data = Get_Judge_Data();
	INFO_PRINT("detect");
	//3、裁判系统
	usart6_base_init();
	Detect_Task_Init();

	vTaskDelay(800);
	
	while(1)
	{
    
		for(u8 i=0; i<8; i++)
		{
			//各个模块状态刷新
			Module_Status_Listen(&module_status[i]);
			
			if(MODULE_REONLINE(i))
			{
				LED_FLOW_ON(i);   
				INFO_LOG("Module%d Online.\r\n\r\n", i);
			}
			else if(MODULE_OFFLINE(i))
			{
				LED_FLOW_OFF(i);
				INFO_LOG("Module%d Offline.\r\n\r\n", i);
			}
			
		}
		
		/* 功率限制 */
//		if(module_status[chassis_motor].time_out_flag==0 && module_status[judge_system].time_out_flag==0 && module_status[super_capacitor].time_out_flag==0)  //判断底盘、裁判系统、底盘是否同时上线
//		{
//			cap_send_cnt++;
//			if(cap_send_cnt == 4)
//			{
//				cap_send_cnt = 0;
//				//DEBUG_SHOWDATA1("gglgl", judge_data->game_robot_status.chassis_power_limit);  //打印限制功率
//				//设置功率比限制功率小2W
//				//Set_Super_Capacitor( (judge_data->game_robot_status.chassis_power_limit-2) * 100);
//			}
//		}
		
		LED_GREEN_TOGGLE;

		vTaskDelay(250);
	}
	
	
	//vTaskDelete(NULL);
	
}

static void Detect_Task_Init(void)
{
	for(u8 i=0; i<8; i++)
	{
		Module_Status_Init(&module_status[i], 5, NULL, NULL);
	}
	
	module_status[5].reload_cnt = 10;
	
}

u8 Get_Module_Online_State(u8 id)
{
	// return (!module_status[id].time_out_flag);
	if(module_status[id].time_out_flag == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void Detect_Reload(u8 index)
{
	Module_Status_Reload(&module_status[index]);
}

//void Classis_Reload(int8_t motor_index)
//{
//	static u8 chassis_motor[4] = {0, 0, 0, 0};
//	
//	if(motor_index>=0 && motor_index<=3)
//	{
//		chassis_motor[motor_index] = 1;
//	}
//	else
//	{
//		return;
//	}
//	if(chassis_motor[0] && chassis_motor[1] && chassis_motor[2] && chassis_motor[3])
//	{
//		chassis_motor[0] = 0;
//		chassis_motor[1] = 0;
//		chassis_motor[2] = 0;
//		chassis_motor[3] = 0;
//		Module_Status_Reload(&module_status[1]);
//	}
//}

////void Up_Reload(int8_t motor_index)
////{
////	static u8 up_motor[2] = {0, 0};
////	
////	if(motor_index>=0 && motor_index<=1)
////	{
////		up_motor[motor_index] = 1;
////	}
////	else
////	{
////		return;
////	}
////	if(up_motor[0] && up_motor[1])
////	{
////		up_motor[0] = 0;
////		up_motor[1] = 0;
////		Module_Status_Reload(&module_status[2]);
////	}
////}

////void Overturn_Reload(int8_t motor_index)
////{
////	static u8 overturn_motor[2] = {0, 0};
////	
////	if(motor_index>=0 && motor_index<=1)
////	{
////		overturn_motor[motor_index] = 1;
////	}
////	else
////	{
////		return;
////	}
////	if(overturn_motor[0] && overturn_motor[1])
////	{
////		overturn_motor[0] = 0;
////		overturn_motor[1] = 0;
////		Module_Status_Reload(&module_status[2]);
////	}
////}

void Overturn_Reload(void)
{
	Module_Status_Reload(&module_status[1]);
}
void Stretch_Reload(void)
{
	Module_Status_Reload(&module_status[1]);
}
