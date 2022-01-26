/*
1、集中配置LL库未配置
2、后续的RTOS任务初始化也可放在这
3、RTOS的任务中间级API也可放在这


APB1_peripheral:45M  APB1_timer:90M 
APB2_peripheral:90M  APB2_timer:180M
cortex system timer:180M

*/
#include "centre.h"    


extern uint8_t UartRxDmaBuf[18];   //上位机传输的原始数据   一帧18byte
//集中补充配置
void base_init()
{
	//usart1__dma1
	LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_2);
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)(&USART1->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)UartRxDmaBuf);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 18);
	LL_USART_EnableIT_RXNE(USART1);   //使能串口1的中断
	LL_USART_EnableIT_IDLE(USART1);   //使能串口1的空闲中断   注：使用IDLE+RXNE的DMA模式
	
	LL_USART_EnableDMAReq_RX(USART1);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_2);
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



