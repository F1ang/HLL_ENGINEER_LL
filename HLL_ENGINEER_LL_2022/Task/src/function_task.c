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
#include "math2.h"
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
extern M550_Mileage motor550_l;
extern M550_Mileage motor550_r;

static int KA,SetAngle_550_l,SetAngle_550_r;
static int Small_T,Small_T_l,Small_T_r,Small_S;//抬升、伸出->微调相关
long int pos=-72000;
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
			//开环伸缩
				switch(robot_mode.mode_stretch_small)  
				{
					case 3:Small_S-=10;break;//缩	
					case 2:Small_S+=10;break;//伸
					//case 1:Small_S=0;break;
					default:	break;
				}
				
				//伸出位点1 2 3 4	
				if(robot_mode.mode_open_stretch==1)
				{
						switch(robot_mode.mode_stretch)
						{
							case 1:KA=0;break;
							case 2:KA=-4800;break;//3060 兑换
							case 3:KA=-10420;break;//资源岛 兑换
							case 4:KA=0;break;
						}
				}
				else 
				{
								KA=Small_S;
				}
																				/*抬升*/		
				//抬升进行               
			 if(robot_mode.mode_open_up==1)
				 {
						switch(robot_mode.mode_up)
						{
							case 3:
								SetAngle_550_l=3900;//空接-兑换站
								SetAngle_550_r=-3900;
								Set_550_Motors_Angle(SetAngle_550_l,SetAngle_550_r,KA);
							break;
					
							case 2:
								SetAngle_550_l=2500;//资源岛
								SetAngle_550_r=-2500;
								Set_550_Motors_Angle(SetAngle_550_l,SetAngle_550_r,KA);
							break;
					
							case 1:
								SetAngle_550_l=0;
								SetAngle_550_r=0;
								Set_550_Motors_Angle(0,0,KA);
							break;
					
							default:PI5_PWM_OUT(1500);PI6_PWM_OUT(1500);PI7_PWM_OUT(1500);
							break;
						}
				}
				 //开环
				else 
				{
						//开环升
						switch(robot_mode.mode_up_small_l)
						{
							case 2:Small_T_l+=10;break;//+20
							//case 1:Small_T_l=0;break;
							default:	break;
						}
						//开环降
						switch(robot_mode.mode_up_small_r)
						{
							case 2:Small_T_l=-10;break;
							//case 1:Small_T_r=0;break;
							default:	break;
						}
						
						if(Small_T_l>Small_T_l_Max)Small_T_l=Small_T_l_Max;
						else if(Small_T_l<Small_T_l_Min)Small_T_l=Small_T_l_Min;
					
						if(KA>KA_Max)KA=KA_Max;
						else if(KA<KA_Min)KA=KA_Min;
					
						if(robot_mode.mode_up_small_l==2)Set_550_Motors_Angle(Small_T_l,-Small_T_l,KA);//升
						if(robot_mode.mode_up_small_r==2)Set_550_Motors_Angle(Small_T_l,-Small_T_l,KA);//降
				}
				//微调
				//if(Small_T_r!=1|Small_T_l!=1|Small_S!=1)Set_550_Motors_Angle(SetAngle_550_l+Small_T_l,SetAngle_550_r+Small_T_r,KA+Small_S);
				
																			/*救援钩子*/
				if(function_robot_mode->mode_rescue==1)
				{	
						LL_TIM_OC_SetCompareCH3(TIM3 ,950);//PB0_PWM_OUT(0)  救援右   900  
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  救援左  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,780);//PB0_PWM_OUT(0)  救援右     600--800
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1150);//PB1_PWM_OUT(0)  救援左    1100-1200 	
				}
				
																			/*爪子*/
				switch(robot_mode.mode_overturn)
				{
					case 1:			//平: 
						Ramp_Calc_over(700,-72000);
					  Set_Overturn_Motors_Angle(output);
					break;
				
					case 2:			//翻:
						Ramp_Calc_over(700,-6500);
						Set_Overturn_Motors_Angle(output);
					break;
					
					case 3:
						
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
				//remoter.c中实现
		}
		
																							/***遥控器模式***/
		else if(function_robot_mode->control_device == 2)
		{
			if(robot_mode.open==1)	
			{//伸出
				switch(robot_mode.mode_stretch)//伸出位点1 2 3 4
				{
					case 1:KA=0;break;
					case 2:KA=-4800;break;//30600
					case 3:KA=-10420;break;
					case 4:KA=0;break;
				}

				//抬升
				switch(robot_mode.mode_up)
				{
					case 3:
						Set_550_Motors_Angle(3800,-3800,KA);//l r s
					break;
			
					case 2:
						Set_550_Motors_Angle(2500,-2500,KA);//3700
					break;
			
					case 1:
					Set_550_Motors_Angle(0,0,KA);
					break;
			
					default:PI5_PWM_OUT(1500);PI6_PWM_OUT(1500);PI7_PWM_OUT(1500);
					break;
				}
		 }
		else if(robot_mode.open==2)Set_550_Motors_Speed(0,0,remoter_control->rc.ch4);	
		
		else if(robot_mode.open==3)Set_550_Motors_Speed(remoter_control->rc.ch4,-remoter_control->rc.ch4,0);
			
			//翻转  S2-3 to 1 ，与夹取关联
				switch(robot_mode.mode_overturn)
				{
					
					case 1:			//平: 
						Ramp_Calc_over(1000,-72000);
					  Set_Overturn_Motors_Angle(output);
						//Set_Overturn_Motors_Angle(-72000);//-82000  降
					  //Set_Overturn_Motors_Speed(overturn_total_tar);//速度16384（角度取值8192）
						break;
					
					case 2:			//翻:
						Ramp_Calc_over(1000,-6500);
						Set_Overturn_Motors_Angle(output);
						//Set_Overturn_Motors_Angle(-6500);//-6500 -6200 抬
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
						LL_TIM_OC_SetCompareCH3(TIM3 ,950);//PB0_PWM_OUT(0)  救援右   900  
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  救援左  1000-1200  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,780);//PB0_PWM_OUT(0)  救援右     600
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1150);//PB1_PWM_OUT(0)  救援左     	500
				}
		}//remote_end
		
/****DEBUG****/   		
//调参VOFA
		//printf(" %d,%d\n",-SetAngle_550_l/15,motor550_l.speed_rpm);	//-287  
		
		//printf("%d, %d\n",2700,motor550_l.total_angle);		
		//printf("%d, %d\n",9700,motor550_r.total_angle);
		
		//printf("%d   \r\n",motor550_s.total_angle);		
		
		//printf("%d\n",overturn_motor_li.speed_rpm);
		
		
		
		//printf("%d, %d\n",-75300,overturn_motor_li.total_angle);	
    vTaskDelay(5); //延时5ms，也就是1000个时钟节拍			
	}
	
}



