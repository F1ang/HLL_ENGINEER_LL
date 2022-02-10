#include "can2_motor.h"

/* 定义变量 */
M3508_Mileage overturn_motor_li;
static Pid_Position_t motor_overturn_speed_pid = NEW_POSITION_PID(46, 0.9, 0.04, 5000, 10000, 0, 1000, 500); //翻转电机速度PID
static Pid_Position_t motor_overturn_angle_pid = NEW_POSITION_PID(0.1, 0, 0.01, 100, 1900, 0, 1, 50); //翻转电机角度PID

static s16 motor_ampere[4]={0,0,0,0};
/* 函数声明 */
extern u16 overturn_angle;


/* CAN2 send and receive ID wheel_left */
typedef enum
{
	CAN_OVERTURN_ALL_ID = 0x200,
	CAN_OVERTURN_ID = 0x201,
}can2_msg_id_e;


//计算一个翻转电机
void Set_Overturn_Motors_Speed(float speed_overturn)
{
	for(int i=0;i<4;i++){motor_ampere[i]=0;}//每一次都清空电流，防止意外
	motor_ampere[0]=Pid_Position_Calc(&motor_overturn_speed_pid, speed_overturn, overturn_motor_li.speed_rpm);
	MotorM3508_Send_Ampere(motor_ampere);
}
void Set_Overturn_Motors_Angle(int angle_overturn)
{
	Pid_Position_Calc(&motor_overturn_angle_pid, angle_overturn, overturn_motor_li.total_angle);
	Set_Overturn_Motors_Speed(motor_overturn_angle_pid.output);
}
#if CAN1_RX0_INT_ENABLE	//如果不采用轮询的方式（can口数据在每次使用前调用）
//中断服务函数			    
void CAN1_RX0_IRQHandler(void)
{
	u8 rxbuf[8];
	u32 id;
	u8 ide,rtr,len;     
 	CAN1_Rx_Msg(0,&id,&ide,&rtr,&len,rxbuf);
  switch(id)
	{
		case 1:get_moto_measure(&overturn_motor_li,rxbuf);break;	
	}		
}
#endif

