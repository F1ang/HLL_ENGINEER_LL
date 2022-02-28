#ifndef _CAN_2_H
#define _CAN_2_H	 
#include "sys.h" 
#include "pid.h" 
#include "motor.h"
#define CAN2_RX0_INT_ENABLE 1  //����can2�����ж�
/* can2������ڹ���ʵ�֣��õ��˽ǶȻ�����̼ƣ������ȱȽ���CAN1����ض����� */

/*��ʼ��CAN1*/
int Can2_Init(void);//��ʼ�����Դ���
void Can2_Filter(void);//������

/*��������*/
u8 Motor_Can2_Send(u32 Distinguish_ID,u8 len ,u8 *data);//����CAN2(δ��װ)
u8 CAN2_Tx_Staus(u8 FIFOBOX);//��ȡ����״̬(�Ƿ������)
u8 MotorM3508_Send_Ampere(u32 all_id,s16 *data);//����M3508�ĵ������

/*��������*/
void CAN2_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat);//����CAN2(δ��װ)
u8 CAN2_Msg_Pend(u8 fifox);//�õ���FIFO0/FIFO1�н��յ��ı��ĸ���(���ܵ����Ŀ�ʼ��������)



#endif


