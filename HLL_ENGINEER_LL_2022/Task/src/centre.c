/*
1、集中配置LL库未配置
2、后续的RTOS任务初始化也可放在这
3、RTOS的任务中间级API也可放在这


APB1_peripheral:45M  APB1_timer:90M 
APB2_peripheral:90M  APB2_timer:180M
cortex system timer:180M

*/
#include "centre.h"    

//集中补充配置
void base_init()
{
	//usart1__dma2
	usart1_base_init();    
	//usart6__dma2
	usart6_base_init();
	
	
/*注：encoder:tim2 tim4 tim5   pwm:tim8的ch1~ch4、tim3的ch3、ch4   tim7_updata-20ms待加
	高级定时器的主输出使能、ARPE使能待加...
	*/	
	
	
	
//	//tim3--ch1~ch4的pwm输出-----先配置在这
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH1);
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH2);
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH3);
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH4);
//	
//  LL_TIM_EnableCounter(TIM3);
//  LL_TIM_EnableAllOutputs(TIM3);
//	
//	//tim2 ch2的pwm
//	LL_TIM_CC_EnableChannel(TIM2,LL_TIM_CHANNEL_CH2);
//	LL_TIM_EnableCounter(TIM2);
//  LL_TIM_EnableAllOutputs(TIM2);
//	
//	//tim4--updata
//	LL_TIM_EnableIT_UPDATE(TIM2); 
//  LL_TIM_EnableCounter(TIM2); 

//pwm通道使能
/*  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
		LL_TIM_EnableCounter(TIM3);*/
// Enable counter 
/* 
		LL_TIM_CC_EnableChannel(TIM4,LL_TIM_CHANNEL_CH1);
		LL_TIM_CC_EnableChannel(TIM4,LL_TIM_CHANNEL_CH2);
		LL_TIM_EnableCounter(TIM4);
		
		LL_TIM_GetCounter();//读脉冲CNT
*/

	

}



