#ifndef __MOTOR_H
#define __MOTOR_H
#include "pid.h"
#include "sys.h"
#include "usart_hll.h"
typedef struct
{
	uint16_t mechanical_angle;  //转子机械角度
	int16_t speed_rpm;  //转子转速
	int16_t actual_torque_current;  //实际转矩电流
	uint8_t temperate;  //电机温度
}Motor_measure_t;


typedef struct // M3508里程计专用结构体 
{
  uint16_t mechanical_angle;  //转子机械角度
	int16_t speed_rpm;  //转子转速
	int16_t actual_torque_current;  //实际转矩电流
	uint8_t temperate;  //电机温度
  u16 Last_mechanical_angle; //上一个机械转角

  u16 offest_angle;           //电机启动时候的零偏角度
  s32 round_cnt;              //电机转动圈数
  s32 total_angle;            //电机转动的总角度

 /* 暂时没有搞懂这些东西是用来干嘛的 */
  u8 buf_idx;                 //IDX记录（不用）
  u16 fited_angle;
  u32 MSG_CNT;

}M3508_Mileage;

typedef struct // M550里程计专用结构体 
{
  uint16_t delta;	
  uint16_t offest_angle;
  int16_t count;      //圈数
  s32 last_total_angle; 
	uint16_t last_speed_rmp;

	int16_t speed_rpm;  //转子转速
  s32 total_angle;    //电机转动的总角度
}M550_Mileage;


void Calculate_Motor_Data(Motor_measure_t* motor, u8*Data);
void get_moto_measure(M3508_Mileage *Mileage,u8*Data);
#endif
