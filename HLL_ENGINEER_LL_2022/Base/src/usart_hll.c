/*
	1、DBUS-串口1，remote_task二值信号量任务通知
	2、裁判系统-串口8，Notify_Judge_Task二值信号量
	3、printf-usart3
	4、usart6保留
*/
#include "usart_hll.h"
//1、DBUS
#define RC_RX_BUF_NUM 36
uint8_t UartRxflag = 0;            							//收到一帧标志
static uint8_t rc_rx_buf0[RC_RX_BUF_NUM];
static uint8_t rc_rx_buf1[RC_RX_BUF_NUM];
static volatile uint16_t usart1_dma_rxd_data_len;	///< USART1 DMA 已经接收到的数据长度

//2、裁判系统
static uint8_t uart6_rx_buf[128];//usart6
static uint8_t uart6_rx_length = 0;

static uint8_t uart8_rx_buf[128];//usart8
static uint8_t uart8_rx_length = 0;

//3、printf-usart3
int fputc(int ch, FILE *f)											
{
	LL_USART_TransmitData8(USART3,ch);
	while(!LL_USART_IsActiveFlag_TC(USART3))
	{
	}
	return ch;
}



//DBUS_LL，补充配置
/*当DMAy Streamx被禁用时，配置双缓冲模式和当前内存目标。*/
void usart1_base_init(void)
{
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)(&USART1->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)rc_rx_buf0);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, RC_RX_BUF_NUM);

		///< 开启 DMA 双缓冲
	LL_DMA_SetMemory1Address(DMA2, LL_DMA_STREAM_2, (uint32_t)(rc_rx_buf1));
	LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM0);
	LL_DMA_EnableDoubleBufferMode(DMA2, LL_DMA_STREAM_2);
	
//	LL_USART_EnableIT_RXNE(USART1);
	LL_USART_ClearFlag_IDLE(USART1);	
	LL_USART_EnableIT_IDLE(USART1);  //注：法一：IDLE+RXNE，法二：BUFF+IDLE（改选） ，方法三:BUFF+DMA(选)
	
	LL_USART_EnableDMAReq_RX(USART1);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_2);

}	

/*DBUS，串口1中断回调*/
void USART_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(USART1))			
		{		
			//停止接收
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2);			
			//清标志位			
			LL_USART_ClearFlag_IDLE(USART1);
			LL_DMA_ClearFlag_TC2(DMA2);			
			///< 获取该帧的数据长度
			usart1_dma_rxd_data_len = RC_RX_BUF_NUM - LL_DMA_GetDataLength(DMA2, LL_DMA_STREAM_2);
			//判断哪个BUFF不为CPU使用
			if (LL_DMA_GetCurrentTargetMem(DMA2, LL_DMA_STREAM_2) == LL_DMA_CURRENTTARGETMEM1)
			{
				LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM0);
			}
			else
			{
				LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_2, LL_DMA_CURRENTTARGETMEM1);
			}
			//重设接收
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, RC_RX_BUF_NUM);
			
			Rc_Data_Update(); 
	
			//debug
			LED_RED_ON;	
			
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);	
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
	LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, RC_RX_BUF_NUM);	
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
	LL_USART_Enable(USART1);
}



//裁判系统，补充配置  ，注：RX采用DMA,TX采用轮询
void uart8_base_init(void)
{
	//RX
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_6, (uint32_t)(&UART8->DR));
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_6, (uint32_t)uart8_rx_buf);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_6, 128);
		
	LL_DMA_ClearFlag_TC6(DMA1);		
	LL_USART_ClearFlag_IDLE(UART8); 
	LL_USART_EnableIT_IDLE(UART8);   //注：使用IDLE+BUFF的DMA模式
	LL_USART_EnableDMAReq_RX(UART8);
	LL_DMA_EnableStream(DMA1,LL_DMA_STREAM_6);
}
/*裁判系统，串口8中断回调*/
void UART8_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(UART8))			
		{			
			LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);
			LL_USART_ClearFlag_IDLE(UART8);
			LL_DMA_ClearFlag_TC6(DMA1);//add1			
			//已接收数据长度
		  	uart8_rx_length = 128 - LL_DMA_GetDataLength (DMA1, LL_DMA_STREAM_6);
			//重设传输长度
			LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_6, 128);
			//debug
			LED_RED_OFF;
			// 通知裁判系统任务	
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

//TX轮询发送:UI
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









//裁判系统，补充配置:保留
void usart6_base_init(void)
{
	LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_1, (uint32_t)(&USART6->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_1, (uint32_t)uart6_rx_buf);
	
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);
//	LL_USART_EnableIT_RXNE(USART6);   //使能串口1的中断
	LL_USART_EnableIT_IDLE(USART6);   //使能串口1的空闲中断   注：使用IDLE+RXNE的DMA模式
	
	LL_USART_EnableDMAReq_RX(USART6);
	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_1);
}
/*裁判系统，串口6中断回调*/
void USART6_RxIdleCallback(void)
{
		if(LL_USART_IsActiveFlag_IDLE(USART6))			
		{			
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_1);
			LL_USART_ClearFlag_IDLE(USART6); 									
			
			//已接收数据长度
		  uart6_rx_length = 128 - LL_DMA_GetDataLength (DMA2, LL_DMA_STREAM_1);
			
			//重设传输长度
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, 128);
			// 通知裁判系统任务	
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



