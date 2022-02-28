#ifndef _CAN_1_H
#define _CAN_1_H	 
//#include "stdio.h"
#include "sys.h" 
//#include "EightBitsHl.h"  
#define CAN1_RX0_INT_ENABLE 1 //1����can�����ж�

/*���������ṹ�壬һ��Ҫ��סͷ�ļ���ṹ��ֻ���������ܶ��壡*/	
typedef struct //NBMotorȫ��(No Brush Motor)
{
  u16 Rotor_Mechanical_Angle;
	s16 Rotor_Speed;
	s16 Torque_Current;
	u16 temperature;
}Chassis_Motor;



/*��ʼ��CAN1*/
int Can1_Init(void);//��ʼ�����Դ���
void Can1_Filter(void);//������

/*��������*/
u8 Motor_Can_Send(u32 Distinguish_ID,u8 len ,u8 *data);//����CAN1(δ��װ)
u8 CAN1_Tx_Staus(u8 FIFOBOX);//��ȡ����״̬(�Ƿ������)

u8 Can1_Send_4Msg(u32 all_id,s16 *data); 

/*��������*/
void CAN1_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat);//����CAN1(δ��װ)
u8 CAN1_Msg_Pend(u8 fifox);//�õ���FIFO0/FIFO1�н��յ��ı��ĸ���(���ܵ����Ŀ�ʼ��������)




#endif


