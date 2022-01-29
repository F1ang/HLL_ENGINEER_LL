/*
	1、DBUS-串口1，remote_task二值信号量任务通知
	2、裁判系统-串口6，Notify_Judge_Task二值信号量
	
*/
#include "bsp_usart.h"
//DBUS
#define RC_RX_BUF_NUM 18
uint8_t UartRxBuf[RC_RX_BUF_NUM];							 //cope数据
uint8_t UartRxDmaBuf[RC_RX_BUF_NUM];           //原始数据
uint8_t UartRxflag = 0;            //确认收到一帧标志

static uint8_t rc_rx_buf0[RC_RX_BUF_NUM];
static uint8_t rc_rx_buf1[RC_RX_BUF_NUM];

//裁判系统
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
/*DBUS，串口1中断回调*/
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
			for(i=0; i<18; i++)
			{			
				UartRxBuf[i] = UartRxDmaBuf[i];
			}								
			UartRxflag = 1; //标志已经成功接收到一包等待处理
//			Rc_Data_Update(); // /*通知遥控器任务，处理遥控器数据,remote_task.c*/		
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 18);						
		}
}

/*
  函数名：Get_Rc_Buf
  描述  ：获取rc_rx_buf
  参数  ：bufx
  返回值：串口1接收数组指针
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
	DMA的双缓冲选择rc_rx_buf0，rc_rx_buf1
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
  函数名：Usart1_DMA_Reset
  描述  ：重启串口1和串口1DMA
  参数  ：无
  返回值：无
*/
void Usart1_DMA_Reset(void)
{	
	LL_USART_Enable(USART1);
	LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2); 
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 18);	
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
	LL_USART_Enable(USART1);
}

/*裁判系统，串口6中断回调*/
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
		  uart6_rx_length = 128 - LL_DMA_GetDataLength (DMA2, LL_DMA_STREAM_1);//已接收数据长度	 		
//	  Notify_Judge_Task(uart8_rx_length);; // /*通知裁判系统任务，处理裁判数据,judge_task.c*/		
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);		
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);	//待加judge_task.c						
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




