/*
	1��DBUS-����1��remote_task��ֵ�ź�������֪ͨ
	2������ϵͳ-����6��Notify_Judge_Task��ֵ�ź���
	3��printf-usart3
*/
#include "usart_hll.h"
//1��DBUS
#define RC_RX_BUF_NUM 18
uint8_t UartRxflag = 0;            							//�յ�һ֡��־
static uint8_t rc_rx_buf0[RC_RX_BUF_NUM];
static uint8_t rc_rx_buf1[RC_RX_BUF_NUM];

//2������ϵͳ
static uint8_t uart6_rx_buf[128];
static uint8_t uart6_rx_length = 0;

//3��printf-usart3
int fputc(int ch, FILE *f)											
{
	LL_USART_TransmitData8(USART3,ch);
	while(!LL_USART_IsActiveFlag_TC(USART3))
	{
	}
	return ch;
}



//DBUS_LL����������
/*��DMAy Streamx������ʱ������˫����ģʽ�͵�ǰ�ڴ�Ŀ�ꡣ*/
void usart1_base_init(void)
{
	LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_2);
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)(&USART1->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)rc_rx_buf0);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 18);

	LL_DMA_DisableStream(DMA2,LL_DMA_STREAM_2);	//˫����ģʽ������
	LL_DMA_ConfigAddresses  (DMA2,LL_DMA_STREAM_2,(uint32_t)(&USART1->DR),(uint32_t)rc_rx_buf1,LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetCurrentTargetMem  (DMA2,LL_DMA_STREAM_2,LL_DMA_CURRENTTARGETMEM0);
	LL_DMA_EnableDoubleBufferMode  (DMA2,LL_DMA_STREAM_2);	
	
	LL_USART_EnableIT_RXNE(USART1);   
	LL_USART_EnableIT_IDLE(USART1);   				//ע��ʹ��IDLE+RXNE��DMAģʽ
	
	LL_USART_EnableDMAReq_RX(USART1);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_2);
}	

/*DBUS������1�жϻص�*/
void USART_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(USART1))			
		{			
			//���־λ			
			LL_USART_ClearFlag_IDLE(USART1); 											
			if(rc_rx_buf0[2] == 0xEE)			
			{			
				NVIC_SystemReset();			
			}		
			//��ͣ����		
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2); 				
			//�ɹ����յ�һ֡
			UartRxflag = 1; 
			//֪ͨң��������
			Rc_Data_Update();    
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



//����ϵͳ����������
void usart6_base_init(void)
{
	LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_1);
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_1, (uint32_t)(&USART6->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_1, (uint32_t)uart6_rx_buf);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);
	LL_USART_EnableIT_RXNE(USART6);   //ʹ�ܴ���1���ж�
	LL_USART_EnableIT_IDLE(USART6);   //ʹ�ܴ���1�Ŀ����ж�   ע��ʹ��IDLE+RXNE��DMAģʽ
	
	LL_USART_EnableDMAReq_RX(USART6);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_1);
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
			//�ѽ������ݳ���
		  uart6_rx_length = 128 - LL_DMA_GetDataLength (DMA2, LL_DMA_STREAM_1);
			//���贫�䳤��
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);
			// ֪ͨ����ϵͳ����	
			Notify_Judge_Task(uart6_rx_length);				  		
					
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



