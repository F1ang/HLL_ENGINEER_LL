#ifndef BSP_USART_H
#define BSP_USART_H

#include <stdio.h> 
#include "main.h"
#include "usart.h"
#include "remoter_task.h"

#include "judge_task.h"
#include "buzzer_task.h"
#define DEBUG 1
#define INFORMATION 1

#if DEBUG
	#define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
	#define DEBUG_LOG(format, arg...) printf("<DEBUG> " format "\r\n", ##arg)
	#define DEBUG_SHOWDATA1(name, data) printf("<DEBUG> %s = %d\r\n",name,data)
	#define DEBUG_SHOWDATA2(name, data) printf("<DEBUG> %s = %.2f\r\n",name,data)
	#define DEBUG_ERROR(err) printf("<ERROR> error:%d\r\n",err)
	#define DEBUG_ARRAY(name, data, len) { printf("<DEBUG>%s : {"); for(u16 __tmp_i=0; __tmp_i<len; __tmp_i++) printf("%d ", data[__tmp_i]);  printf("}\r\n"); }
#else
	#define DEBUG_PRINT(format, ...)
	#define DEBUG_LOG(format, arg...)
	#define DEBUG_SHOWDATA1(name, data)
	#define DEBUG_SHOWDATA2(name, data)
	#define DEBUG_ERROR(err)
	#define DEBUG_ARRAY(name, data, len)
#endif

#if INFORMATION
	#define INFO_PRINT(format, ...) printf(format, ##__VA_ARGS__)
	#define INFO_LOG(format, arg...) printf("<INFO> " format "\r\n", ##arg)
#else
	#define INFO_PRINT(format, ...)
	#define INFO_LOG(format, arg...)
#endif
	
	
void USART_RxIdleCallback(void);
const uint8_t* Get_Rc_Bufferx(uint8_t bufx);
uint8_t Get_Rc_Available_Bufferx(void);
void Usart1_DMA_Reset(void);

void USART6_RxIdleCallback(void);
const uint8_t* Get_Judge_Buf(void);
uint8_t Get_Judge_Buf_Len(void);

void usart1_base_init(void);
void usart6_base_init(void);


#endif

