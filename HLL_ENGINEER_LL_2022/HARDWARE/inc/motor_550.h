#ifndef __MOTOR_550_H
#define __MOTOR_550_H

#include "main.h"
#include "pid.h"
#include "math2.h"
#include "detect_task.h"
#include "tim_hll.h"
#include "motor.h"
void Set_550_Motors_Angle(int angle_550_l,int angle_550_r,int angle_550_s);
void Set_550_Motors_Speed(float speed_550_l,float speed_550_r,float speed_550_s);
#endif
