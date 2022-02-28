#ifndef _CAN_1_H
#define _CAN_1_H	 
//#include "stdio.h"
#include "sys.h" 
//#include "EightBitsHl.h"  
#define CAN1_RX0_INT_ENABLE 1 //1开启can接收中断

/*声明两个结构体，一定要记住头文件里结构体只能声明不能定义！*/	
typedef struct //NBMotor全称(No Brush Motor)
{
  u16 Rotor_Mechanical_Angle;
	s16 Rotor_Speed;
	s16 Torque_Current;
	u16 temperature;
}Chassis_Motor;



/*初始化CAN1*/
int Can1_Init(void);//初始化调试串口
void Can1_Filter(void);//过滤器

/*发送数据*/
u8 Motor_Can_Send(u32 Distinguish_ID,u8 len ,u8 *data);//发送CAN1(未封装)
u8 CAN1_Tx_Staus(u8 FIFOBOX);//获取发送状态(是否发送完毕)

u8 Can1_Send_4Msg(u32 all_id,s16 *data); 

/*接收数据*/
void CAN1_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat);//接受CAN1(未封装)
u8 CAN1_Msg_Pend(u8 fifox);//得到在FIFO0/FIFO1中接收到的报文个数(接受到报文开始解析数据)




#endif


