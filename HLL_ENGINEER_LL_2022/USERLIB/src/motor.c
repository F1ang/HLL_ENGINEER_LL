#include "motor.h"

void Calculate_Motor_Data(Motor_measure_t* motor, u8*Data)
{
	motor->mechanical_angle = (uint16_t)(Data[0] << 8 | Data[1]);
	motor->speed_rpm = (uint16_t)(Data[2] << 8 | Data[3]);
	motor->actual_torque_current = (uint16_t)(Data[4] << 8 | Data[5]);
	motor->temperate = Data[6];
}

/*********************************************************************
 * @brief 里程计计算距离
 * 
 * @param Mileage 	里程计记录数据用的结构体
 * @param RxMessage can接收到的数据 
*********************************************************************/
void get_moto_measure(M3508_Mileage *Mileage,u8*Data)
{
	int delta=0;
	Mileage->Last_mechanical_angle = Mileage->mechanical_angle;
	Mileage->mechanical_angle = (uint16_t)(Data[0] << 8 | Data[1]);
	Mileage->speed_rpm = (uint16_t)(Data[2] << 8 | Data[3]);
	Mileage->speed_rpm = Mileage->speed_rpm;	
	
	Mileage->actual_torque_current = (uint16_t)(Data[4] << 8 | Data[5]);//转矩电流
	Mileage->temperate = Data[6];//温度
	//way_1
	if(Mileage->speed_rpm > 0 ) //电机正转
	{
		if((Mileage->mechanical_angle - Mileage->Last_mechanical_angle) >= 50)
		{
		  delta = Mileage->mechanical_angle - Mileage->Last_mechanical_angle;
		}
		else if ((Mileage->mechanical_angle - Mileage->Last_mechanical_angle) <= -50)
		{
		  delta = Mileage->mechanical_angle + 8192 - Mileage->Last_mechanical_angle;
			Mileage->round_cnt++;
		}
		else;
	}
  else if(Mileage->speed_rpm < 0) //电机反转
	{
		if ((Mileage->mechanical_angle - Mileage->Last_mechanical_angle) <= -50)
		{
			delta =Mileage->mechanical_angle - Mileage->Last_mechanical_angle;
		}
		else if((Mileage->mechanical_angle - Mileage->Last_mechanical_angle) >= 50)
		{
			delta = Mileage->mechanical_angle - 8192 - Mileage->Last_mechanical_angle;
			Mileage->round_cnt--;
		}
		else;
	}
	else
		delta=0;
	Mileage->total_angle = Mileage->round_cnt * 8192 + Mileage->mechanical_angle - Mileage->offest_angle;

////way_2
//	if((Mileage->mechanical_angle - Mileage->Last_mechanical_angle)> 4096)
//			Mileage->round_cnt--;
//	else if ((Mileage->mechanical_angle - Mileage->Last_mechanical_angle)< -4096)
//			Mileage->round_cnt++;
//	//记录real_all_angle--->error:way_1可能delta会出错
//	Mileage->total_angle = Mileage->round_cnt * 8192 + Mileage->mechanical_angle - Mileage->offest_angle;
//	delta=Mileage->Last_mechanical_angle-Mileage->total_angle;


}
