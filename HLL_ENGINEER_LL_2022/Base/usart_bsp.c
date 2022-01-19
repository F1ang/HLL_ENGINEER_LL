/*
	DBUS
*/
#include "usart_bsp.h"

uint8_t UartRxBuf[18];							 //cope数据
uint8_t UartRxDmaBuf[18];           //原始数据
uint8_t UartRxflag = 0;            //确认收到一帧标志

int fputc(int ch, FILE *f)			//printf-usart3
{
	LL_USART_TransmitData8(USART3,ch);
	while(!LL_USART_IsActiveFlag_TC(USART3))
	{
	}
	return ch;
}
//中断回调
void USART_RxIdleCallback(void)
{
		uint8_t i;
		if(LL_USART_IsActiveFlag_IDLE(USART1))			
		{			
			//先清标志位			
			LL_USART_ClearFlag_IDLE(USART1); 											
			if(UartRxDmaBuf[2] == 0xEE)			
			{			
				NVIC_SystemReset();			
			}		
			//先停止UART流DMA，暂停接收		
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2); 			
			/* 3.搬移数据进行其他处理 */			
			for(i=0; i<8; i++)
			{			
				UartRxBuf[i] = UartRxDmaBuf[i];
			}								
			UartRxflag = 1; //标志已经成功接收到一包等待处理		
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 8);		
			/* 4.开启新的一次DMA接收 */	
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);							
		}
}





