#include "motor_550.h"   
extern M550_Mileage motor550_l;
extern M550_Mileage motor550_r;
extern M550_Mileage motor550_s;
																													//p,i,d,limit_i,limit_out,is_i_spare,begin_i,stop_grow_i
//static Pid_Position_t motor550_l_speed_pid = NEW_POSITION_PID(0.7, 0.2, 0.1, 20, 450, 0, 50, 100);
//static Pid_Position_t motor550_l_angle_pid = NEW_POSITION_PID(0.03, 0, 0, 150, 450, 0, 50, 100); 

static Pid_Position_t motor550_l_speed_pid = NEW_POSITION_PID(10, 0.3, 0.08, 20, 400, 0, 50, 100); 
static Pid_Position_t motor550_l_angle_pid = NEW_POSITION_PID(0.05, 0.01, 0.08, 150, 400, 0, 50, 100); 

static Pid_Position_t motor550_r_speed_pid = NEW_POSITION_PID(10, 0.3, 0.08, 20, 400, 0, 50, 100); 
static Pid_Position_t motor550_r_angle_pid = NEW_POSITION_PID(0.05, 0.01, 0.08, 150, 400, 0, 50, 100); 
//8-12  0.25  0.08
static Pid_Position_t motor550_s_speed_pid = NEW_POSITION_PID(10, 0.3, 0.08, 20, 400, 0, 50, 100); 
static Pid_Position_t motor550_s_angle_pid = NEW_POSITION_PID(0.05, 0.01, 0.08, 150, 400, 0, 50, 100); 


void Set_550_Motors_Speed(float speed_550_l,float speed_550_r,float speed_550_s)
{
	int l_out;
	int r_out;
	int s_out;
	Pid_Position_Calc(&motor550_l_speed_pid, speed_550_l, motor550_l.speed_rpm);
	Pid_Position_Calc(&motor550_r_speed_pid, speed_550_r, motor550_r.speed_rpm);
	Pid_Position_Calc(&motor550_s_speed_pid, speed_550_s, motor550_s.speed_rpm);
	l_out=motor550_l_speed_pid.output;
	r_out=motor550_r_speed_pid.output;
	s_out=motor550_s_speed_pid.output;
	PI5_PWM_OUT(1500+l_out);
	PI6_PWM_OUT(1500+r_out);
	PI7_PWM_OUT(1500+s_out);

}
void Set_550_Motors_Angle(int angle_550_l,int angle_550_r,int angle_550_s)
{
	Pid_Position_Calc(&motor550_l_angle_pid, angle_550_l, motor550_l.total_angle);
	Pid_Position_Calc(&motor550_r_angle_pid, angle_550_r, motor550_r.total_angle);
	Pid_Position_Calc(&motor550_s_angle_pid, angle_550_s, motor550_s.total_angle);
	Set_550_Motors_Speed(motor550_l_angle_pid.output,motor550_r_angle_pid.output,motor550_s_angle_pid.output);
}
