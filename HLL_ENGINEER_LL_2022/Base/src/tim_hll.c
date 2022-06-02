/*

	1、encoder:tim2 tim4 tim5   pwm:tim8的ch1~ch4(抬升1 抬升2 伸缩 舵机1)、  tim3 的ch3、ch4（舵机2, 舵机3）  pwm :tim9的ch1 ch2（舵机4，舵机5）  
	tim7_ pdata-20ms  高级定时器的主输出使能，实现。
	
	2、ARPE使能待加，debug
	
	3、tim7中断相关
	
	
*/

#include "tim_hll.h"   
     
M550_Mileage motor550_l;
M550_Mileage motor550_r; 
M550_Mileage motor550_s;		 
void get_550_measure(M550_Mileage *Mileage,uint8_t choise_motor);		 
		 
/*
    PI7     ------> TIM8_CH3
    PI6     ------> TIM8_CH2
    PI5     ------> TIM8_CH1
    PI2     ------> TIM8_CH4
*/
void tim8_base_init(void)
{
	LL_TIM_CC_EnableChannel(TIM8,LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIM8,LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_EnableChannel(TIM8,LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_EnableChannel(TIM8,LL_TIM_CHANNEL_CH4);
	
  LL_TIM_EnableCounter(TIM8);
  LL_TIM_EnableAllOutputs(TIM8);
	
	LL_TIM_OC_SetCompareCH1(TIM8 ,1500);//	PI5_PWM_OUT(1500);
	LL_TIM_OC_SetCompareCH2(TIM8 ,1500);//	PI6_PWM_OUT(1500);
	
	LL_TIM_OC_SetCompareCH3(TIM8 ,1500);//	PI7_PWM_OUT(1500);
	LL_TIM_OC_SetCompareCH4(TIM8 ,0);		//	PI2_PWM_OUT(0)   舵机1
}
/*
    PC9------> TIM3_CH4----->PB1
    PC8------> TIM3_CH3----->PB0
*/
void tim3_base_init(void)
{
	//AF待定,F4手册，P180   原差动pwm
	LL_GPIO_SetAFPin_8_15( GPIOB,LL_GPIO_PIN_0,LL_GPIO_AF_2);
	LL_GPIO_SetAFPin_8_15( GPIOB,LL_GPIO_PIN_1,LL_GPIO_AF_2);
	
	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH4);
	
  LL_TIM_EnableCounter(TIM3);
	
	LL_TIM_OC_SetCompareCH3(TIM3 ,950);//PB0_PWM_OUT(0)  救援右    
	LL_TIM_OC_SetCompareCH4(TIM3 ,900);//PB1_PWM_OUT(0)  救援左     
}
 /*
    PE5     ------> TIM9_CH1
    PE6     ------> TIM9_CH2
 */
void tim9_base_init(void)
{
	LL_TIM_CC_EnableChannel(TIM9,LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIM9,LL_TIM_CHANNEL_CH2);
	
  LL_TIM_EnableCounter(TIM9);
  LL_TIM_EnableAllOutputs(TIM9);
	
	LL_TIM_OC_SetCompareCH1(TIM9 ,1500);//PE5_PWM_OUT(ccr)  摄像头
	LL_TIM_OC_SetCompareCH2(TIM9 ,1500);//PE6_PWM_OUT(ccr)  舵机5
}

//encoder:x=tim2,tim4,tim5
//注：debug  LL_TIM_EnableAllOutputs
void timx_base_init(void)
{
	/*TIM2
  PB3   ------> TIM2_CH2----->PA1
  PA15  ------> TIM2_CH1----->PA0
  */
	LL_GPIO_SetAFPin_0_7( GPIOA,LL_GPIO_PIN_0,LL_GPIO_AF_1);
	LL_GPIO_SetAFPin_0_7( GPIOA,LL_GPIO_PIN_1,LL_GPIO_AF_1);
	
	LL_TIM_CC_EnableChannel(TIM2,LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIM2,LL_TIM_CHANNEL_CH2);
	LL_TIM_EnableCounter(TIM2);
	/*TIM4
  PD13   ------> TIM4_CH2
  PD12   ------> TIM4_CH1
	*/
	LL_TIM_CC_EnableChannel(TIM4,LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIM4,LL_TIM_CHANNEL_CH2);
	LL_TIM_EnableCounter(TIM4);
  /*TIM5
  PH11   ------> TIM5_CH2  
  PH10   ------> TIM5_CH1  
  */

	LL_TIM_CC_EnableChannel(TIM5,LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIM5,LL_TIM_CHANNEL_CH2);
	LL_TIM_EnableCounter(TIM5);
}

//tim7:updata
void tim7_base_init(void)
{
	LL_TIM_EnableIT_UPDATE(TIM7); 
  LL_TIM_EnableCounter(TIM7); 
}



void TIM7_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM7))
	{
		LL_TIM_ClearFlag_UPDATE(TIM7);
		
		motor550_l.speed_rpm=read_tim4_encoder();	
		motor550_r.speed_rpm=-read_tim5_encoder();
		motor550_s.speed_rpm=read_tim2_encoder();    
		motor550_l.total_angle+=motor550_l.speed_rpm;
		motor550_r.total_angle+=motor550_r.speed_rpm;
		motor550_s.total_angle+=motor550_s.speed_rpm;
		
//		get_550_measure(&motor550_l,4);	
//		get_550_measure(&motor550_r,5);	
//		get_550_measure(&motor550_s,2);	
	}
}

void get_550_measure(M550_Mileage *Mileage,uint8_t choise_motor)
{
	
	Mileage->last_speed_rmp=Mileage->speed_rpm;
	switch (choise_motor)
	{
	case 2:
		Mileage->speed_rpm=read_tim2_encoder();//s
		break;
	case 4:
		Mileage->speed_rpm=read_tim4_encoder();//l
	break;
	case 5:
		Mileage->speed_rpm=-read_tim5_encoder();//r
	break;	
	default:
		break;
	}
			Mileage->total_angle+=Mileage->speed_rpm;
	
	Mileage->delta=Mileage->total_angle-Mileage->last_total_angle;
}


//buzzer:tim12
void tim12_base_init(void)
{
/*TIM12
    PH6------> TIM12_CH1
*/
	LL_TIM_CC_EnableChannel(TIM12,LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableCounter(TIM12);
}




