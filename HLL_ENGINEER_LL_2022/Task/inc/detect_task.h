#ifndef __DETECT_TASK_H
#define __DETECT_TASK_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t DetectTask_Handler;
void Detect_Task(void *pvParameters);
uint8_t Get_Module_Online_State(uint8_t id);
extern void Detect_Reload(uint8_t index);
void Classis_Reload(int8_t motor_index);
//void Gimbal_Reload(int8_t motor_index);
//void Shooter_Reload(void);
void Overturn_Reload(void);
void Stretch_Reload(void);
#endif
