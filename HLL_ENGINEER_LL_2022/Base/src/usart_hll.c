/*
	1��DBUS-����1��remote_task��ֵ�ź�������֪ͨ
	2������ϵͳ-����8��Notify_Judge_Task��ֵ�ź���
	3��printf-usart3
	4��usart6����
*/
#include "usart_hll.h"
//1��DBUS
#define RC_RX_BUF_NUM 36
uint8_t UartRxflag = 0;            							//�յ�һ֡��־
static uint8_t rc_rx_buf0[RC_RX_BUF_NUM];
static uint8_t rc_rx_buf1[RC_RX_BUF_NUM];
static volatile uint16_t usart1_dma_rxd_data_len;	///< USART1 DMA �Ѿ����յ������ݳ���

//2������ϵͳ
static uint8_t uart6_rx_buf[128];//usart6
static uint8_t uart6_rx_length = 0;

static uint8_t uart8_rx_buf[128];//usart8
static uint8_t uart8_rx_length = 0;

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
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)(&USART1->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)rc_rx_buf0);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, RC_RX_BUF_NUM);

		///< ���� DMA ˫����
	LL_DMA_SetMemory1Address(DMA2, LL_DMA_STREAM_2, (uint32_t)(rc_rx_buf1));
	LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM0);
	LL_DMA_EnableDoubleBufferMode(DMA2, LL_DMA_STREAM_2);
	
//	LL_USART_EnableIT_RXNE(USART1);
	LL_USART_ClearFlag_IDLE(USART1);	
	LL_USART_EnableIT_IDLE(USART1);  //ע����һ��IDLE+RXNE��������BUFF+IDLE����ѡ�� ��������:BUFF+DMA(ѡ)
	
	LL_USART_EnableDMAReq_RX(USART1);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_2);

}	

/*DBUS������1�жϻص�*/
void USART_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(USART1))			
		{		
			//ֹͣ����
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2);			
			//���־λ			
			LL_USART_ClearFlag_IDLE(USART1);
			LL_DMA_ClearFlag_TC2(DMA2);			
			///< ��ȡ��֡�����ݳ���
			usart1_dma_rxd_data_len = RC_RX_BUF_NUM - LL_DMA_GetDataLength(DMA2, LL_DMA_STREAM_2);
			//�ж��ĸ�BUFF��ΪCPUʹ��
			if (LL_DMA_GetCurrentTargetMem(DMA2, LL_DMA_STREAM_2) == LL_DMA_CURRENTTARGETMEM1)
			{
				LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM0);
			}
			else
			{
				LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM1);
			}
			//�������
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, RC_RX_BUF_NUM);
			
			Rc_Data_Update(); 
	
			//debug
			LED_RED_ON;	
			
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);	
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
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, RC_RX_BUF_NUM);	
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
	LL_USART_Enable(USART1);
}



//����ϵͳ����������  ��ע��RX����DMA,TX������ѯ
void uart8_base_init(void)
{
	//RX
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_6, (uint32_t)(&UART8->DR));
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_6, (uint32_t)uart8_rx_buf);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_6, 128);
		
	LL_DMA_ClearFlag_TC6(DMA1);		
	LL_USART_ClearFlag_IDLE(UART8); 
	LL_USART_EnableIT_IDLE(UART8);   //ע��ʹ��IDLE+BUFF��DMAģʽ
	LL_USART_EnableDMAReq_RX(UART8);
	LL_DMA_EnableStream(DMA1,LL_DMA_STREAM_6);
}
/*����ϵͳ������8�жϻص�*/
void UART8_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(UART8))			
		{			
			LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);
			LL_USART_ClearFlag_IDLE(UART8);
			LL_DMA_ClearFlag_TC6(DMA1);//add1			
			//�ѽ������ݳ���
		  	uart8_rx_length = 128 - LL_DMA_GetDataLength (DMA1, LL_DMA_STREAM_6);
			//���贫�䳤��
			LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_6, 128);
			//debug
			LED_RED_OFF;
			// ֪ͨ����ϵͳ����	
			Notify_Judge_Task(uart8_rx_length);				  			
			LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_6);						
		}
}

const uint8_t* Get_Judge_Buf(void)
{
	return uart8_rx_buf;
}

uint8_t Get_Judge_Buf_Len(void)
{
	return uart8_rx_length;
}

//TX��ѯ����:UI
uint8_t usart8_send(uint8_t data)
{	
		uint8_t uart8_tx_length;
		uart8_tx_length=sizeof(data);
		LL_USART_TransmitData8(UART8,data);  
    while (LL_USART_IsActiveFlag_TXE(UART8) == 0)
    {
    }
    return 0;
}









//����ϵͳ����������:����
void usart6_base_init(void)
{
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_1, (uint32_t)(&USART6->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_1, (uint32_t)uart6_rx_buf);
	
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);
//	LL_USART_EnableIT_RXNE(USART6);   //ʹ�ܴ���1���ж�
	LL_USART_EnableIT_IDLE(USART6);   //ʹ�ܴ���1�Ŀ����ж�   ע��ʹ��IDLE+RXNE��DMAģʽ
	
	LL_USART_EnableDMAReq_RX(USART6);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_1);
}
/*����ϵͳ������6�жϻص�*/
void USART6_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(USART6))			
		{			
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_1);
			LL_USART_ClearFlag_IDLE(USART6); 									
			
			//�ѽ������ݳ���
		  uart6_rx_length = 128 - LL_DMA_GetDataLength (DMA2, LL_DMA_STREAM_1);
			
			//���贫�䳤��
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);
			// ֪ͨ����ϵͳ����	
			Notify_Judge_Task(uart6_rx_length);				  		
					
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);						
		}
}

const uint8_t* Get_Judge_Buf_6(void)
{
	return uart6_rx_buf;
}

uint8_t Get_Judge_Buf_Len_6(void)
{
	return uart6_rx_length;
}



