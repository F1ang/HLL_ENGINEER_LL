#ifndef __REMOTER_TASK_H
#define __REMOTER_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "remoter.h"  
#include "led.h"
#include "buzzer_task.h"

typedef struct
{
	uint8_t control_device; //控制设备 1键鼠模式 2遥控器模式
	uint8_t mode_up; //模式 1放下 2抬升
	uint8_t mode_stretch;// 1缩回 2伸出
	uint8_t mode_chip;//1松开 2夹取
	uint8_t mode_overturn;//翻转、松开、回位一套流程
	uint8_t mode_revive;//1不伸出 2复活
	uint8_t mode_rescue;//1不救援 2救援
}Robot_mode_t;


extern Robot_mode_t robot_mode;  //机器人模式
extern TaskHandle_t RemoterTask_Handler;

void Remoter_Task(void *pvParameters);
void Rc_Data_Update(void);

void Clear_Shoot_Key(void);
void Fric_Reset(void);

const Rc_ctrl_t *Get_Remote_Control_Point(void);
Robot_mode_t *Get_Robot_Mode_Point(void);


#endif

