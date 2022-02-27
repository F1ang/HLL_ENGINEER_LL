#include "over_chip_task.h"

#include "motor_550.h"
#include "can1_motor.h"
#include "can2_motor.h"
extern M3508_Mileage overturn_motor_li;
extern u16 overturn_angle;
extern u16 overturn_angle_now;
extern u16 count;

TaskHandle_t OverchipTask_Handler;
extern Pid_Position_t motor_overturn_speed_pid;

extern M550_Mileage motor550_l;
extern M550_Mileage motor550_r;
extern M550_Mileage motor550_s;

void Overchip_Task(void *pvParameters)
{
	while(1)
	{
		vTaskDelay(400);
		printf("SPEED:%d  %d  %d  ANGLE:%d  %d  %d\r\n",motor550_l.speed_rpm,motor550_r.speed_rpm,motor550_s.speed_rpm\
		,motor550_l.total_angle,motor550_s.total_angle,motor550_r.total_angle);
		//PI5_PWM_OUT(2000);
		//PB0_PWM_OUT(200);
		//PB1_PWM_OUT(0);
		//Set_550_Motors_Speed(100,0,0);
//    Set_550_Motors_Angle(61200,0,0);		ต๗สิ
   
    vTaskDelay(5);
	}
}

