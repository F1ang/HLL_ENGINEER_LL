/*
	DBUS
*/
#include "usart_bsp.h"

uint8_t UartRxBuf[18];							 //cope����
uint8_t UartRxDmaBuf[18];           //ԭʼ����
uint8_t UartRxflag = 0;            //ȷ���յ�һ֡��־

int fputc(int ch, FILE *f)			//printf-usart3
{
	LL_USART_TransmitData8(USART3,ch);
	while(!LL_USART_IsActiveFlag_TC(USART3))
	{
	}
	return ch;
}
//�жϻص�
void USART_RxIdleCallback(void)
{
		uint8_t i;
		if(LL_USART_IsActiveFlag_IDLE(USART1))			
		{			
			//�����־λ			
			LL_USART_ClearFlag_IDLE(USART1); 											
			if(UartRxDmaBuf[2] == 0xEE)			
			{			
				NVIC_SystemReset();			
			}		
			//��ֹͣUART��DMA����ͣ����		
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2); 			
			/* 3.�������ݽ����������� */			
			for(i=0; i<8; i++)
			{			
				UartRxBuf[i] = UartRxDmaBuf[i];
			}								
			UartRxflag = 1; //��־�Ѿ��ɹ����յ�һ���ȴ�����		
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 8);		
			/* 4.�����µ�һ��DMA���� */	
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);							
		}
}





