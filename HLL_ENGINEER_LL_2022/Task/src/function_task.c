#include "function_task.h"
#include "remoter_task.h"
#include "can1_motor.h"
#include "can2_motor.h"
#include "remoter.h"
#include "math2.h"
#include "judge_system.h"
#include "detect_task.h"
#include "function.h" 
#include "tim_hll.h"
#include "motor_550.h"   
#include "pump.h"	
#include "usart_hll.h" 
TaskHandle_t FunctionTask_Handler;

const static Rc_ctrl_t* remoter_control;
static Robot_mode_t* function_robot_mode;

//overturn
extern M3508_Mileage overturn_motor_li;
extern float overturn_speed;
extern s32 all_angle;
u8 overturn_seq=1;
u16 overturn_angle=0;

//flag
extern u8 up_flag;
extern u8 stretch_flag;
extern u8 chip_flag;
extern u8 overturn_flag;

//target_angle
extern s32 overturn_total_tar;
extern s32 stretch_total_tar;
s32 up_total_tar;

extern M550_Mileage motor550_s;
extern Pid_Position_t motor_overturn_speed_pid;

void Function_Task(void *pvParameters)
{

	remoter_control = Get_Remote_Control_Point();  //遥控器数据
	function_robot_mode = Get_Robot_Mode_Point();  //机器人模式数据
	//judge_data = Get_Judge_Data();
	vTaskDelay(1000);
	function_init();
	vTaskDelay(200);
	while(1)
	{
		LED_GREEN_TOGGLE;	
		//键鼠控制
		if(function_robot_mode->control_device == 1)
		{
			if(up_flag)//抬升
			{
				if(function_robot_mode->mode_up==1)
				{
					
					            //添加下降代码
					up_flag=0;
				}
				else
				{
					
					            //添加抬升代码
					up_flag=0;
				}
			}
			if(stretch_flag)//伸出
			{
				
				if(function_robot_mode->mode_stretch==1)
				{
					
					             //缩回代码
					stretch_flag=0;
				}
				else
				{
					
					              //伸出代码
					stretch_flag=0;
				}
				
			}
			if(chip_flag)//夹取
			{
				if(function_robot_mode->mode_chip==1)
				{
					
					PUMP_OFF;	//松
					chip_flag=0;
				}
				else
				{
					
					PUMP_ON;              
					chip_flag=0;
				}
				
			}
			if(function_robot_mode->mode_overturn==2)
			{
				if(overturn_seq==1)overturn_task_first();	            //翻转松开再翻回来一整套流程
				if(overturn_seq==2)overturn_task_second();
				function_robot_mode->mode_overturn=1;
			}			
		}//mouse_end
		
		
		//遥控器模式
		else if(function_robot_mode->control_device == 2)
		{
				//伸出
				int KA;
				switch(robot_mode.mode_stretch)//1 2 3 4
				{
					case 1:KA=0;break;
					case 2:KA=0;break;
					case 3:KA=0;break;
					case 4:KA=0;break;
				}
				//抬升
				switch(robot_mode.mode_up)
				{
					case 3:
					Set_550_Motors_Angle(11000,-11000,KA);
					break;
			
					case 2:
					Set_550_Motors_Angle(9700,-9700,KA);
					break;
			
					case 1:
					Set_550_Motors_Angle(0,-0,KA);
					break;
			
					default:PI5_PWM_OUT(0);PI6_PWM_OUT(1500);PB0_PWM_OUT(1500);PB1_PWM_OUT(1500);
					break;
				}

			//翻转  S1-3 to 1 ，与夹取关联
				switch(robot_mode.mode_overturn)
				{
					
					case 1:			//平: 
						Set_Overturn_Motors_Angle(-82000);//-82000
					//Set_Overturn_Motors_Speed(overturn_total_tar);//速度16384（角度取值8192）
						break;
					
					case 2:			//翻:
						Set_Overturn_Motors_Angle(-6500);//-6900
						break;
					
				}
				
				//夹取S1
				if(function_robot_mode->mode_chip==1)
				{	
					PUMP_OFF;             //松开代码  1-3
				}
				else
				{
					PUMP_ON;              //夹取代码  3-1
				}										
		}//remote_end
		
	
		/***debug***/
				printf("%d, %d\n",-6500,overturn_motor_li.total_angle);
		//	printf("%d, %d\n",-75300,overturn_motor_li.total_angle);
		
    vTaskDelay(5); //延时5ms，也就是1000个时钟节拍			
	}
	
}



