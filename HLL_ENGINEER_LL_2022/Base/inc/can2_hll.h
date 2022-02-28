#ifndef _CAN_2_H
#define _CAN_2_H	 
#include "sys.h" 
#include "pid.h" 
#include "motor.h"
#define CAN2_RX0_INT_ENABLE 1  //开启can2接收中断
/* can2电机用于功能实现，用到了角度环和里程计，所以先比较与CAN1多加特定参数 */

/*初始化CAN1*/
int Can2_Init(void);//初始化调试串口
void Can2_Filter(void);//过滤器

/*发送数据*/
u8 Motor_Can2_Send(u32 Distinguish_ID,u8 len ,u8 *data);//发送CAN2(未封装)
u8 CAN2_Tx_Staus(u8 FIFOBOX);//获取发送状态(是否发送完毕)
u8 MotorM3508_Send_Ampere(u32 all_id,s16 *data);//给予M3508的电机电流

/*接收数据*/
void CAN2_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat);//接受CAN2(未封装)
u8 CAN2_Msg_Pend(u8 fifox);//得到在FIFO0/FIFO1中接收到的报文个数(接受到报文开始解析数据)



#endif


