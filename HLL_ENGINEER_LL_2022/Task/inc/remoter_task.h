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
	uint8_t control_device; //�����豸 1����ģʽ 2ң����ģʽ
	uint8_t mode_up; //ģʽ 1���� 2̧��
	uint8_t mode_stretch;// 1���� 2���
	uint8_t mode_chip;//1�ɿ� 2��ȡ
	uint8_t mode_overturn;//��ת���ɿ�����λһ������
	uint8_t mode_revive;//1����� 2����
	uint8_t mode_rescue;//1����Ԯ 2��Ԯ
}Robot_mode_t;


extern Robot_mode_t robot_mode;  //������ģʽ
extern TaskHandle_t RemoterTask_Handler;

void Remoter_Task(void *pvParameters);
void Rc_Data_Update(void);

void Clear_Shoot_Key(void);
void Fric_Reset(void);

const Rc_ctrl_t *Get_Remote_Control_Point(void);
Robot_mode_t *Get_Robot_Mode_Point(void);


#endif

