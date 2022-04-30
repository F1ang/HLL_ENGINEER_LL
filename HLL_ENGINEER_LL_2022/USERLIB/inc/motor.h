#ifndef __MOTOR_H
#define __MOTOR_H
#include "pid.h"
#include "sys.h"
#include "usart_hll.h"
typedef struct
{
	uint16_t mechanical_angle;  //ת�ӻ�е�Ƕ�
	int16_t speed_rpm;  //ת��ת��
	int16_t actual_torque_current;  //ʵ��ת�ص���
	uint8_t temperate;  //����¶�
}Motor_measure_t;


typedef struct // M3508��̼�ר�ýṹ�� 
{
  uint16_t mechanical_angle;  //ת�ӻ�е�Ƕ�
	int16_t speed_rpm;  //ת��ת��
	int16_t actual_torque_current;  //ʵ��ת�ص���
	uint8_t temperate;  //����¶�
  u16 Last_mechanical_angle; //��һ����еת��

  u16 offest_angle;           //�������ʱ�����ƫ�Ƕ�
  s32 round_cnt;              //���ת��Ȧ��
  s32 total_angle;            //���ת�����ܽǶ�

 /* ��ʱû�и㶮��Щ��������������� */
  u8 buf_idx;                 //IDX��¼�����ã�
  u16 fited_angle;
  u32 MSG_CNT;

}M3508_Mileage;

typedef struct // M550��̼�ר�ýṹ�� 
{
  uint16_t delta;	
  uint16_t offest_angle;
  int16_t count;      //Ȧ��
  s32 last_total_angle; 
	uint16_t last_speed_rmp;

	int16_t speed_rpm;  //ת��ת��
  s32 total_angle;    //���ת�����ܽǶ�
}M550_Mileage;


void Calculate_Motor_Data(Motor_measure_t* motor, u8*Data);
void get_moto_measure(M3508_Mileage *Mileage,u8*Data);
#endif
