/*
	1��DBUS-����1��remote_task��ֵ�ź�������֪ͨ
	2������ϵͳ-����6��Notify_Judge_Task��ֵ�ź���
	
*/
#include "bsp_usart.h"
//DBUS
#define RC_RX_BUF_NUM 18
uint8_t UartRxBuf[RC_RX_BUF_NUM];							 //cope����
uint8_t UartRxDmaBuf[RC_RX_BUF_NUM];           //ԭʼ����
uint8_t UartRxflag = 0;            //ȷ���յ�һ֡��־

static uint8_t rc_rx_buf0[RC_RX_BUF_NUM];
static uint8_t rc_rx_buf1[RC_RX_BUF_NUM];

//����ϵͳ
 uint8_t uart6_rx_buf[128];
static uint8_t uart6_rx_length = 0;

int fputc(int ch, FILE *f)			//printf-usart3
{
	LL_USART_TransmitData8(USART3,ch);
	while(!LL_USART_IsActiveFlag_TC(USART3))
	{
	}
	return ch;
}
/*DBUS������1�жϻص�*/
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
			for(i=0; i<18; i++)
			{			
				UartRxBuf[i] = UartRxDmaBuf[i];
			}								
			UartRxflag = 1; //��־�Ѿ��ɹ����յ�һ���ȴ�����
//			Rc_Data_Update(); // /*֪ͨң�������񣬴���ң��������,remote_task.c*/		
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 18);						
		}
}

/*
  ��������Get_Rc_Buf
  ����  ����ȡrc_rx_buf
  ����  ��bufx
  ����ֵ������1��������ָ��
*/
const uint8_t* Get_Rc_Bufferx(uint8_t bufx)
{
	if(bufx == 0)
	{
		return rc_rx_buf0;
	}
	else if(bufx == 1)
	{
		return rc_rx_buf1;
	}
	return NULL;
}
/*
	DMA��˫����ѡ��rc_rx_buf0��rc_rx_buf1
*/
uint8_t Get_Rc_Available_Bufferx(void)
{
	if(LL_DMA_GetCurrentTargetMem  (DMA2,LL_DMA_STREAM_2 ) 
)
	{
		return 0;
	}
	return 1;
}

/*
  ��������Usart1_DMA_Reset
  ����  ����������1�ʹ���1DMA
  ����  ����
  ����ֵ����
*/
void Usart1_DMA_Reset(void)
{	
	LL_USART_Enable(USART1);
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2); 
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 18);	
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
	LL_USART_Enable(USART1);
}

/*����ϵͳ������6�жϻص�*/
void USART6_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(USART6))			
		{			
			LL_USART_ClearFlag_IDLE(USART6); 											
			if(uart6_rx_buf[2] == 0xEE)			
			{			
				NVIC_SystemReset();			
			}		
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_1);
		  uart6_rx_length = 128 - LL_DMA_GetDataLength (DMA2, LL_DMA_STREAM_1);//�ѽ������ݳ���	 		
//	  Notify_Judge_Task(uart8_rx_length);; // /*֪ͨ����ϵͳ���񣬴����������,judge_task.c*/		
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);		
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);	//����judge_task.c						
		}
}

const uint8_t* Get_Judge_Buf(void)
{
	return uart6_rx_buf;
}

uint8_t Get_Judge_Buf_Len(void)
{
	return uart6_rx_length;
}




