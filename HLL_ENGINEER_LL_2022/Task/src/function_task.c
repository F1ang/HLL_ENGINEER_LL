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
extern M550_Mileage motor550_l;
extern M550_Mileage motor550_r;

static int KA,SetAngle_550_l,SetAngle_550_r;
static int Small_T,Small_T_l,Small_T_r,Small_S;//̧�������->΢�����

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
					case 3:Small_S=-300;break;//��΢��	
					case 2:Small_S=300;break;//��΢��
					case 1:Small_S=0;break;
					default:	break;
				}
				//���λ��1 2 3 4
				switch(robot_mode.mode_stretch)
				{
					case 1:KA=0;break;
					case 2:KA=-7120;break;//30600
					case 3:KA=0;break;
					case 4:KA=-7120;break;
				}
																				/*̧��*/
				//̧��΢��:��
				switch(robot_mode.mode_up_small_l)
				{
					case 2:Small_T_l=300;break;//+20
					case 1:Small_T_l=0;break;
					default:	break;
				}
				//̧��΢��:��
				switch(robot_mode.mode_up_small_r)
				{
					case 2:Small_T_r=-300;break;
					case 1:Small_T_r=0;break;
					default:	break;
				}
				
				//̧������
				switch(robot_mode.mode_up)
				{
					case 3:
						SetAngle_550_l=5700;
						SetAngle_550_r=-5700;
						Set_550_Motors_Angle(SetAngle_550_l,SetAngle_550_r,KA);
					break;
			
					case 2:
						SetAngle_550_l=2700;
						SetAngle_550_r=-2700;
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
				//΢��
				//if(Small_T_r!=1|Small_T_l!=1|Small_S!=1)Set_550_Motors_Angle(SetAngle_550_l+Small_T_l,SetAngle_550_r+Small_T_r,KA+Small_S);
				
																			/*��Ԯ����*/
				if(function_robot_mode->mode_rescue==1)
				{	
						LL_TIM_OC_SetCompareCH3(TIM3 ,950);//PB0_PWM_OUT(0)  ��Ԯ��   900  
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  ��Ԯ��  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,780);//PB0_PWM_OUT(0)  ��Ԯ��     600--800
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1150);//PB1_PWM_OUT(0)  ��Ԯ��    1100-1200 	
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
				//remoter.c��ʵ��
		}
		
																							/***ң����ģʽ***/
		else if(function_robot_mode->control_device == 2)
		{
				//���
				switch(robot_mode.mode_stretch)//���λ��1 2 3 4
				{
					case 1:KA=0;break;
					case 2:KA=-3060;break;//30600
					case 3:KA=0;break;
					case 4:KA=-3060;break;
				}

				//̧��
				switch(robot_mode.mode_up)
				{
					case 3:
						Set_550_Motors_Angle(5700,-5700,KA);//l r s
					break;
			
					case 2:
						Set_550_Motors_Angle(2700,-2700,KA);
					break;
			
					case 1:
					Set_550_Motors_Angle(0,0,KA);
					break;
			
					default:PI5_PWM_OUT(1500);PI6_PWM_OUT(1500);PI7_PWM_OUT(1500);
					break;
				}
//			Set_550_Motors_Speed(remoter_control->rc.ch4,-remoter_control->rc.ch4,KA);
//				Set_550_Motors_Speed(0,0,remoter_control->rc.ch4);
			//��ת  S2-3 to 1 �����ȡ����
				switch(robot_mode.mode_overturn)
				{
					
					case 1:			//ƽ: 
						Set_Overturn_Motors_Angle(-82000);//-82000
					//Set_Overturn_Motors_Speed(overturn_total_tar);//�ٶ�16384���Ƕ�ȡֵ8192��
						break;
					
					case 2:			//��:
						Set_Overturn_Motors_Angle(-6500);//-6900 -6200
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
						LL_TIM_OC_SetCompareCH3(TIM3 ,950);//PB0_PWM_OUT(0)  ��Ԯ��   900  
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  ��Ԯ��  1000-1200  
				}
				else
				{
						LL_TIM_OC_SetCompareCH3(TIM3 ,780);//PB0_PWM_OUT(0)  ��Ԯ��     600
					
	          LL_TIM_OC_SetCompareCH4(TIM3 ,1150);//PB1_PWM_OUT(0)  ��Ԯ��     	500
				}
		}//remote_end
		
/****DEBUG****/   		
//����VOFA
		//printf(" %d,%d\n",-SetAngle_550_l/15,motor550_l.speed_rpm);	//-287  
		
		//printf("%d, %d\n",2700,motor550_l.total_angle);		
		//printf("%d, %d\n",9700,motor550_r.total_angle);
		
		//printf(" %d , %d  \n",6120,motor550_s.total_angle);		
		//printf("%d, %d\n",-6500,overturn_motor_li.total_angle);
		//printf("%d, %d\n",-75300,overturn_motor_li.total_angle);	
    vTaskDelay(5); //��ʱ5ms��Ҳ����1000��ʱ�ӽ���			
	}
	
}



