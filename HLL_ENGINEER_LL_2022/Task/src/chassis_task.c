#include "chassis_task.h"
#include "remoter_task.h"
#include "can1_motor.h"
#include "can2_motor.h"
#include "remoter.h"
#include "math2.h"
#include "judge_system.h"
#include "detect_task.h"

#define CHASSIS_SPEED_ZERO 0
#define OUTPUT_LIMIT(data, limit) Int16_Constrain(data, -limit, limit)

//��ر�������
TaskHandle_t ChassisTask_Handler;

const static Rc_ctrl_t* remoter_control;
const static Robot_mode_t* chassis_robot_mode;

static float chassis_motor_dynamic_rate = 10;
int real_motor_speed[4];

void Chassis_Task(void *pvParameters)
{
	int16_t motor_speed[4] = {0, 0, 0, 0};
	remoter_control = Get_Remote_Control_Point();  //ң��������  
	chassis_robot_mode = Get_Robot_Mode_Point();  //������ģʽ����
//	judge_data = Get_Judge_Data();
	vTaskDelay(200);
	
	while(1)
	{
		//�������
		if(chassis_robot_mode->control_device == 1)
		{					
					motor_speed[0] = remoter_control->virtual_rocker.ch2 + remoter_control->virtual_rocker.ch3 + remoter_control->virtual_rocker.ch0/16.0f;
					motor_speed[1] = remoter_control->virtual_rocker.ch2 - remoter_control->virtual_rocker.ch3  + remoter_control->virtual_rocker.ch0/16.0f;
					motor_speed[2] = -remoter_control->virtual_rocker.ch2 + remoter_control->virtual_rocker.ch3 + remoter_control->virtual_rocker.ch0/16.0f;
					motor_speed[3] = -remoter_control->virtual_rocker.ch2 - remoter_control->virtual_rocker.ch3 + remoter_control->virtual_rocker.ch0/16.0f;
					
					motor_speed[0] *= chassis_motor_dynamic_rate;
					motor_speed[1] *= chassis_motor_dynamic_rate;
					motor_speed[2] *= chassis_motor_dynamic_rate;
					motor_speed[3] *= chassis_motor_dynamic_rate;	
		}		
		//ң����ģʽ
		else if(chassis_robot_mode->control_device == 2)
		{
					
					motor_speed[0] = remoter_control->rc.ch2 + remoter_control->rc.ch3 + remoter_control->rc.ch0;
					motor_speed[1] = remoter_control->rc.ch2 - remoter_control->rc.ch3 + remoter_control->rc.ch0;
					motor_speed[2] = -remoter_control->rc.ch2 + remoter_control->rc.ch3 + remoter_control->rc.ch0;
					motor_speed[3] = -remoter_control->rc.ch2 - remoter_control->rc.ch3 + remoter_control->rc.ch0;
					
					motor_speed[0] *= 11;
					motor_speed[1] *= 11;
					motor_speed[2] *= 11;
					motor_speed[3] *= 11;
			
		}
		
		
		Ramp_Calc_Int(&real_motor_speed[0],50,motor_speed[0]);
		Ramp_Calc_Int(&real_motor_speed[1],50,motor_speed[1]);
		Ramp_Calc_Int(&real_motor_speed[2],50,motor_speed[2]);
		Ramp_Calc_Int(&real_motor_speed[3],50,motor_speed[3]);

		//���̵���ٶ�����
		Set_Chassis_Motors_Speed(real_motor_speed[0], real_motor_speed[1], real_motor_speed[2], real_motor_speed[3]);
    vTaskDelay(5); //��ʱ5ms��Ҳ����1000��ʱ�ӽ���	
		
	}
	
	//vTaskDelete(NULL);
}


