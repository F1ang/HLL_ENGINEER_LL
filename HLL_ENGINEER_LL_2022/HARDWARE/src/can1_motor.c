#include "can1_motor.h"
static Motor_measure_t chassis_motor[4];
//PID控制
//p i d      max_err_integral（积分项）  max_out   is_integral_spare（1/0积分分离） begin_integral（闭环死区） stop_grow_integral
static Pid_Position_t motor_fl_speed_pid = NEW_POSITION_PID(8, 0.0, 3,   2000, 16000,1, 500, 1000);//8 0 2
static Pid_Position_t motor_fr_speed_pid = NEW_POSITION_PID(8, 0.0, 3,   2000, 16000,1, 500, 1000);
static Pid_Position_t motor_bl_speed_pid = NEW_POSITION_PID(8, 0.0, 3,   2000, 16000,1, 500, 1000);
static Pid_Position_t motor_br_speed_pid = NEW_POSITION_PID(8, 0.0, 3,   2000, 16000,1, 500, 1000);

//PD控制
//static Pid_Position_t motor_fl_speed_pid = NEW_POSITION_PID(11, 0, 5,   2000, 16000,  0, 1000, 500);
//static Pid_Position_t motor_fr_speed_pid = NEW_POSITION_PID(11, 0, 5,   2000, 16000,  0, 1000, 500);
//static Pid_Position_t motor_bl_speed_pid = NEW_POSITION_PID(11, 0, 5,   2000, 16000,  0, 1000, 500);
//static Pid_Position_t motor_br_speed_pid = NEW_POSITION_PID(11, 0, 5,   2000, 16000,  0, 1000, 500);

static s16 motor_ampere[4]={0,0,0,0};

/* CAN1 send and receive ID */
typedef enum 
{
	CAN_CHASSIS_ALL_ID = 0x200,
	CAN_3508_M1_ID = 0x201,
	CAN_3508_M2_ID = 0x202,
	CAN_3508_M3_ID = 0x203,
	CAN_3508_M4_ID = 0x204,
	
} can1_msg_id_e;


const Motor_measure_t *Get_Chassis_Motor(void)
{
  return chassis_motor;
}


//计算底盘4个电机速度PID，并输出
void Set_Chassis_Motors_Speed(float speed_fl, float speed_fr, float speed_bl, float speed_br)
{
	for(int i=0;i<4;i++){motor_ampere[i]=0;}//每一次都清空电流，防止意外
	motor_ampere[0]=Pid_Position_Calc(&motor_fl_speed_pid, speed_fl, chassis_motor[0].speed_rpm);
	motor_ampere[1]=Pid_Position_Calc(&motor_fr_speed_pid, speed_fr, chassis_motor[1].speed_rpm);
	motor_ampere[2]=Pid_Position_Calc(&motor_bl_speed_pid, speed_bl, chassis_motor[2].speed_rpm);
	motor_ampere[3]=Pid_Position_Calc(&motor_br_speed_pid, speed_br, chassis_motor[3].speed_rpm);
	
	//debug
	//printf("%d,%d\n",speed_fl,motor_ampere[2]);
	
	
	Can1_Send_4Msg(CAN_CHASSIS_ALL_ID,motor_ampere);
}

#if CAN1_RX0_INT_ENABLE	//如果不采用轮询的方式（can口数据在每次使用前调用）
//中断服务函数	
void CAN1_RX0_IRQHandler(void)
{
	CAN1->IER&=~(1<<1);  //关闭FIFO消息挂起中断
	
	u8 rxbuf[8];
	u32 id;
	u8 ide,rtr,len; 
	if(CAN1_Msg_Pend(0)==0&&CAN1_Msg_Pend(1)==0){printf("Can1未接收到数据\r\n");}      
 	CAN1_Rx_Msg(0,&id,&ide,&rtr,&len,rxbuf);  //使用FIFO0接收邮箱，读取数据放在rxbuf中
	if(ide!=0||rtr!=0){printf("can1接收数据异常\r\n");}
	
	switch(id)
	{
		case 1:Calculate_Motor_Data(&chassis_motor[0],rxbuf);break;
		case 2:Calculate_Motor_Data(&chassis_motor[1],rxbuf);break;
		case 3:Calculate_Motor_Data(&chassis_motor[2],rxbuf);break;
		case 4:Calculate_Motor_Data(&chassis_motor[3],rxbuf);break;	
		default:break;
	}
	
	CAN1->RF0R &=~(1<<5);//释放FIFO0输出邮箱
	CAN1->IER|=1<<1;     //打开FIFO消息挂起中断
   
}


#endif
