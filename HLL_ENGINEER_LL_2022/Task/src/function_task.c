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
static int KA,SetAngle_550_l,SetAngle_550_r,Small_T,Small_S;//抬升、伸出和微调

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
																					/***键鼠控制***/
		if(function_robot_mode->control_device == 1)
		{
																				/*伸出*/
			//伸缩微调
				switch(robot_mode.mode_stretch_small)
				{
					case 3:Small_S=-10;break;//缩微调	
					case 2:Small_S=+10;break;//伸微调
					case 1:Small_S=0;break;
					default:	break;
				}
				//伸出位点1 2 3 4
				switch(robot_mode.mode_stretch)
				{
					case 1:KA=0+Small_S;break;
					case 2:KA=-7120+Small_S;break;//30600
					case 3:KA=0+Small_S;break;
					case 4:KA=-7120+Small_S;break;
				}
																				/*抬升*/
				//抬升微调
				switch(robot_mode.mode_up_small)
				{
					case 3:Small_T=-10;break;	
					case 2:Small_T=+10;break;
					case 1:Small_T=0;break;
					default:	break;
				}
				//抬升进行
				switch(robot_mode.mode_up)
				{
					case 3:
						SetAngle_550_l=11000+Small_T;
						SetAngle_550_r=-11000-Small_T;
						Set_550_Motors_Angle(SetAngle_550_l,SetAngle_550_r,KA);
					break;
			
					case 2:
						SetAngle_550_l=9700+Small_T;
						SetAngle_550_r=-9700-Small_T;
						Set_550_Motors_Angle(SetAngle_550_l,SetAngle_550_r,KA);
					break;
			
					case 1:
						Set_550_Motors_Angle(0,0,KA);
					break;
			
					default:PI5_PWM_OUT(1500);PI6_PWM_OUT(1500);PI7_PWM_OUT(1500);
					break;
				}
																			/*救援钩子*/
				if(function_robot_mode->mode_rescue==1)
				{	
						LL_TIM_OC_SetCompareCH3(TIM3 ,500);//PB0_PWM_OUT(0)  救援右   500  
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  救援左  1000-1200  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,900);//PB0_PWM_OUT(0)  救援右     1200
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1200);//PB1_PWM_OUT(0)  救援左     	500
				}
				
																			/*爪子*/
				switch(robot_mode.mode_overturn)
				{
					case 1:			//平: 
						Set_Overturn_Motors_Angle(-82000);//-82000
					break;
				
					case 2:			//翻:
						Set_Overturn_Motors_Angle(-6500);//-6900
					break;
				}
																			/*复活卡*/
				if(function_robot_mode->mode_revive==1)
				{	
					REVIVE_OFF;            
				}
				else
				{
					REVIVE_ON;              
				}
																			/*夹取*/
				if(function_robot_mode->mode_chip==1)
				{	
					PUMP_OFF;             
				}
				else
				{
					PUMP_ON;              
				}
				
																			/***屏幕***/
				//remoter.c实现
		}
		
																							/***遥控器模式***/
		else if(function_robot_mode->control_device == 2)
		{
				//伸出
				switch(robot_mode.mode_stretch)//伸出位点1 2 3 4
				{
					case 1:KA=0;break;
					case 2:KA=-7120;break;//30600
					case 3:KA=0;break;
					case 4:KA=-7120;break;
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
					Set_550_Motors_Angle(0,0,KA);
					break;
			
					default:PI5_PWM_OUT(1500);PI6_PWM_OUT(1500);PI7_PWM_OUT(1500);
					break;
				}

			//翻转  S2-3 to 1 ，与夹取关联
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
				//复活
				if(function_robot_mode->mode_revive==1)
				{	
					REVIVE_OFF;            
				}
				else
				{
					REVIVE_ON;              
				}
			  //救援
				if(function_robot_mode->mode_rescue==1)
				{	
						LL_TIM_OC_SetCompareCH3(TIM3 ,500);//PB0_PWM_OUT(0)  救援右   500  
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  救援左  1000-1200  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,900);//PB0_PWM_OUT(0)  救援右     1200
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1200);//PB1_PWM_OUT(0)  救援左     	500
				}
		}//remote_end
		
/****DEBUG****/   		
//调参VOFA
		//printf(" %d , %d  \n",6120,motor550_s.total_angle);		
		//printf("%d, %d\n",-6500,overturn_motor_li.total_angle);
		//printf("%d, %d\n",-75300,overturn_motor_li.total_angle);	
    vTaskDelay(5); //延时5ms，也就是1000个时钟节拍			
	}
	
}



