#ifndef BSP_CAN_H
#define BSP_CAN_H
#include "main.h"
#include "can.h"
/*电机ID结构体*/
typedef enum
{
	CAN_CHASSIS_RX_ID = 0x200,   //2006、3508电机接收标识符
	CAN_3508M1_ID = 0x201,
	CAN_3508M2_ID = 0x202,
	CAN_3508M3_ID = 0x203,
	CAN_3508M4_ID = 0x204,
	CAN_3508M5_ID = 0x205,
	CAN_3508M6_ID = 0x206,
	CAN_3508M7_ID = 0x207,
	CAN_3508M8_ID = 0x208,
	
	
	CAN_6020_RX_ID = 0x1FF,     //6020电机接收标识符
	CAN_6020M1_ID = 0x205,
	CAN_6020M2_ID = 0x206,
	CAN_6020M3_ID = 0x207,
	CAN_6020M4_ID = 0x208,
}CAN_MSG_ID;




void CAN1_FILTER_CONFIG(CAN_HandleTypeDef* hcan);
void CAN2_FILTER_CONFIG(CAN_HandleTypeDef* hcan);

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

void SET_CAN1Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);
void SET_CAN1Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);
void SET_CAN2Ahead_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);
void SET_CAN2Back_MOTOR_CURRENT(CAN_HandleTypeDef* hcan, int16_t K1, int16_t K2, int16_t K3, int16_t K4);
#endif


