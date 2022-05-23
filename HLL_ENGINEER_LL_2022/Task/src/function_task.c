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
static int KA,SetAngle_550_l,SetAngle_550_r,Small_T,Small_S;//̧���������΢��

extern M550_Mileage motor550_s;
extern Pid_Position_t motor_overturn_speed_pid;

void Function_Task(void *pvParameters)
{

	remoter_control = Get_Remote_Control_Point();  //ң��������
	function_robot_mode = Get_Robot_Mode_Point();  //������ģʽ����
	//judge_data = Get_Judge_Data();
	vTaskDelay(1000);
	function_init();
	vTaskDelay(200);
	while(1)
	{
		LED_GREEN_TOGGLE;	
																					/***�������***/
		if(function_robot_mode->control_device == 1)
		{
																				/*���*/
			//����΢��
				switch(robot_mode.mode_stretch_small)
				{
					case 3:Small_S=-10;break;//��΢��	
					case 2:Small_S=+10;break;//��΢��
					case 1:Small_S=0;break;
					default:	break;
				}
				//���λ��1 2 3 4
				switch(robot_mode.mode_stretch)
				{
					case 1:KA=0+Small_S;break;
					case 2:KA=-7120+Small_S;break;//30600
					case 3:KA=0+Small_S;break;
					case 4:KA=-7120+Small_S;break;
				}
																				/*̧��*/
				//̧��΢��
				switch(robot_mode.mode_up_small)
				{
					case 3:Small_T=-10;break;	
					case 2:Small_T=+10;break;
					case 1:Small_T=0;break;
					default:	break;
				}
				//̧������
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
																			/*��Ԯ����*/
				if(function_robot_mode->mode_rescue==1)
				{	
						LL_TIM_OC_SetCompareCH3(TIM3 ,500);//PB0_PWM_OUT(0)  ��Ԯ��   500  
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  ��Ԯ��  1000-1200  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,900);//PB0_PWM_OUT(0)  ��Ԯ��     1200
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1200);//PB1_PWM_OUT(0)  ��Ԯ��     	500
				}
				
																			/*צ��*/
				switch(robot_mode.mode_overturn)
				{
					case 1:			//ƽ: 
						Set_Overturn_Motors_Angle(-82000);//-82000
					break;
				
					case 2:			//��:
						Set_Overturn_Motors_Angle(-6500);//-6900
					break;
				}
																			/*���*/
				if(function_robot_mode->mode_revive==1)
				{	
					REVIVE_OFF;            
				}
				else
				{
					REVIVE_ON;              
				}
																			/*��ȡ*/
				if(function_robot_mode->mode_chip==1)
				{	
					PUMP_OFF;             
				}
				else
				{
					PUMP_ON;              
				}
				
																			/***��Ļ***/
				//remoter.cʵ��
		}
		
																							/***ң����ģʽ***/
		else if(function_robot_mode->control_device == 2)
		{
				//���
				switch(robot_mode.mode_stretch)//���λ��1 2 3 4
				{
					case 1:KA=0;break;
					case 2:KA=-7120;break;//30600
					case 3:KA=0;break;
					case 4:KA=-7120;break;
				}
				//̧��
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

			//��ת  S2-3 to 1 �����ȡ����
				switch(robot_mode.mode_overturn)
				{
					
					case 1:			//ƽ: 
						Set_Overturn_Motors_Angle(-82000);//-82000
					//Set_Overturn_Motors_Speed(overturn_total_tar);//�ٶ�16384���Ƕ�ȡֵ8192��
						break;
					
					case 2:			//��:
						Set_Overturn_Motors_Angle(-6500);//-6900
						break;
					
				}
				
				//��ȡS1
				if(function_robot_mode->mode_chip==1)
				{	
					PUMP_OFF;             //�ɿ�����  1-3
				}
				else
				{
					PUMP_ON;              //��ȡ����  3-1
				}
				//����
				if(function_robot_mode->mode_revive==1)
				{	
					REVIVE_OFF;            
				}
				else
				{
					REVIVE_ON;              
				}
			  //��Ԯ
				if(function_robot_mode->mode_rescue==1)
				{	
						LL_TIM_OC_SetCompareCH3(TIM3 ,500);//PB0_PWM_OUT(0)  ��Ԯ��   500  
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  ��Ԯ��  1000-1200  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,900);//PB0_PWM_OUT(0)  ��Ԯ��     1200
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1200);//PB1_PWM_OUT(0)  ��Ԯ��     	500
				}
		}//remote_end
		
/****DEBUG****/   		
//����VOFA
		//printf(" %d , %d  \n",6120,motor550_s.total_angle);		
		//printf("%d, %d\n",-6500,overturn_motor_li.total_angle);
		//printf("%d, %d\n",-75300,overturn_motor_li.total_angle);	
    vTaskDelay(5); //��ʱ5ms��Ҳ����1000��ʱ�ӽ���			
	}
	
}



