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
//		PA2_PWM_OUT(2000);
		//Set_3508_Motors_Angle(remoter_control->rc.ch0*50,0);
		//Can2_Send_4Msg(0x200,remoter_control->rc.ch0,0,0,0)
		//can2_print();
//    INFO_PRINT("%d\n",motor_overturn_speed_pid.output);
//		vTaskDelay(20);
		//INFO_PRINT("%d\n",motor550_r);
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
					
		}
		
		
		//遥控器模式
		else if(function_robot_mode->control_device == 2)
		{
			//抬升
				switch(function_robot_mode->mode_up)
				{
					case 1:             //下降
				  {
						up_total_tar=0;
						break;
					}
					case 2:              //抬升中
					{
						up_total_tar=5000;
						break;
					}
	 				case 3:               //抬升高
					{
						up_total_tar=10000;
						break;
					}
				}
				//夹取
				if(function_robot_mode->mode_chip==1)
				{
					
					PUMP_OFF;             //松开代码
				}
				else
				{
					
					PUMP_ON;              //夹取代码
				}
				
      overturn_total_tar=remoter_control->rc.ch4*	2;
		}
//		/******//******//******//******//*DEBUG*//******//******//******//******/
			int KA;KA=(int)((float)remoter_control->rc.ch1/660*300);
//		//u8 k;if(k%50!=0){printf("KA:%d",KA);k++;}else{k++;}
//		Set_550_Motors_Speed(KA,-KA,0);
//		Set_550_Motors_Speed(0,0,KA);
//		PI5_PWM_OUT(KA+1500);PI6_PWM_OUT(KA+1500);
		switch(remoter_control->rc.s2)
		{
			case 1:
				Set_550_Motors_Angle(11000,-11000,KA);//DEBUG
				break;
			
			case 3:
				Set_550_Motors_Angle(9700,-9700,KA);
				break;
			
			case 2:
				Set_550_Motors_Angle(0,-0,KA);
				break;
			
			default:PI5_PWM_OUT(0);PI6_PWM_OUT(1500);PB0_PWM_OUT(1500);PB1_PWM_OUT(1500);
				break;
		}

		
//		/******//******//******//******//******//******//******//******//******/

		Set_Overturn_Motors_Speed(overturn_total_tar);
		
    vTaskDelay(5); //延时5ms，也就是1000个时钟节拍	
			
	}
	
}



