#ifndef __CAN1_MOTOR_H
#define __CAN1_MOTOR_H
#include "can1_hll.h"
#include "motor.h"
#include "usart_hll.h"
const Motor_measure_t *Get_Chassis_Motor(void);
void Set_Chassis_Motors_Speed(float speed_fl, float speed_fr, float speed_bl, float speed_br);
#endif
