#include "function.h"
#include "Motor.h"
#include "can2_motor.h"
#include "remoter_task.h"
#include "can2_hll.h"
extern Robot_mode_t robot_mode;
extern u8 overturn_seq;

extern M3508_Mileage overturn_motor_li;
extern M550_Mileage motor550_l;
extern M550_Mileage motor550_r; 

u16 count;

s32 all_angle;
extern u16 overturn_angle;
u16 overturn_angle_last;
u16 overturn_angle_now;

u8 over_chip_flag=0;
float overturn_speed;
s32 overturn_total_tar;
s32 stretch_total_tar;
void function_init(void)
{
	overturn_total_tar=0;
	motor550_l.total_angle=0;
	motor550_l.total_angle=0;
	overturn_motor_li.total_angle=0;
	overturn_motor_li.offest_angle=overturn_motor_li.mechanical_angle;
   	
	over_chip_flag=1;  								                            //夹取归位
	                                                              //伸出归位
	                                                              //抬升归位

}
void overturn_task_first(void)
{
	overturn_total_tar=all_angle;
	if(overturn_motor_li.speed_rpm<10&&overturn_motor_li.speed_rpm>-10)over_chip_flag=1;
	
}

void overturn_task_second(void)
{ 
	overturn_total_tar=0;
	if(overturn_motor_li.speed_rpm<10&&overturn_motor_li.speed_rpm>-10)
	{
		overturn_seq=1;
		robot_mode.mode_overturn=1;
		overturn_motor_li.total_angle=0;
	}

	
}



