/*
1����������LL��δ����
2��������RTOS�����ʼ��Ҳ�ɷ�����
3��RTOS�������м伶APIҲ�ɷ�����


APB1_peripheral:45M  APB1_timer:90M 
APB2_peripheral:90M  APB2_timer:180M
cortex system timer:180M

*/
#include "centre.h"    

//���в�������
void base_init()
{
	//usart1__dma2
	usart1_base_init();    
	//usart6__dma2
	usart6_base_init();
	
	
/*ע��encoder:tim2 tim4 tim5   pwm:tim8��ch1~ch4��tim3��ch3��ch4   tim7_updata-20ms����
	�߼���ʱ���������ʹ�ܡ�ARPEʹ�ܴ���...
	*/	
	
	
	
//	//tim3--ch1~ch4��pwm���-----����������
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH1);
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH2);
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH3);
//	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH4);
//	
//  LL_TIM_EnableCounter(TIM3);
//  LL_TIM_EnableAllOutputs(TIM3);
//	
//	//tim2 ch2��pwm
//	LL_TIM_CC_EnableChannel(TIM2,LL_TIM_CHANNEL_CH2);
//	LL_TIM_EnableCounter(TIM2);
//  LL_TIM_EnableAllOutputs(TIM2);
//	
//	//tim4--updata
//	LL_TIM_EnableIT_UPDATE(TIM2); 
//  LL_TIM_EnableCounter(TIM2); 

//pwmͨ��ʹ��
/*  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
		LL_TIM_EnableCounter(TIM3);*/
// Enable counter 
/* 
		LL_TIM_CC_EnableChannel(TIM4,LL_TIM_CHANNEL_CH1);
		LL_TIM_CC_EnableChannel(TIM4,LL_TIM_CHANNEL_CH2);
		LL_TIM_EnableCounter(TIM4);
		
		LL_TIM_GetCounter();//������CNT
*/

	

}



