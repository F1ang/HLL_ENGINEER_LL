/*
	1、encoder:tim2 tim4 tim5   pwm:tim8的ch1~ch4、tim3的ch3、ch4   tim7_updata-20ms待加
	高级定时器的主输出使能，实现。
	
	2、ARPE使能待加，debug
	
	3、tim7中断相关，待加
*/

#include "tim_hll.h"   
     
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
}

//驱动绿色电调
void tim3_base_init(void)
{
	/*TIM3 GPIO Configuration
    PC9------> TIM3_CH4----->PB1
    PC8------> TIM3_CH3----->PB0
  */
	
	//AF待定,F4手册，P180
	LL_GPIO_SetAFPin_8_15( GPIOB,LL_GPIO_PIN_0,LL_GPIO_AF_2);
	LL_GPIO_SetAFPin_8_15( GPIOB,LL_GPIO_PIN_1,LL_GPIO_AF_2);
	
	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH4);
	
  LL_TIM_EnableCounter(TIM3);
	
//	//debug
//	LL_TIM_EnableAllOutputs(TIM3);
//	//debug
//	LL_TIM_OC_SetCompareCH1(TIM3,100);
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


M550_Mileage motor550_l;
M550_Mileage motor550_r; 
M550_Mileage motor550_s;
void TIM7_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM7))
	{
	 //读取编码器
		motor550_l.speed_rpm=read_tim4_encoder();	
		motor550_r.speed_rpm=-read_tim5_encoder();
		motor550_s.speed_rpm=-read_tim2_encoder();    
		motor550_l.total_angle+=motor550_l.speed_rpm;
		motor550_r.total_angle+=motor550_r.speed_rpm;
		motor550_s.total_angle+=motor550_s.speed_rpm;
	
		LL_TIM_ClearFlag_UPDATE(TIM7);
	}

}


//buzzer:tim12
void tim12_base_init(void)
{
/*TIM12
    PH6------> TIM12_CH1
*/
	LL_TIM_CC_EnableChannel(TIM12,LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableCounter(TIM12);
//	LL_TIM_EnableAllOutputs(TIM12);
}




