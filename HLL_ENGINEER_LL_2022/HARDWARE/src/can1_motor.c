#include "can1_motor.h"
static Motor_measure_t chassis_motor[4];

static Pid_Position_t motor_fl_speed_pid = NEW_POSITION_PID(11, 0, 5, 2000, 16000, 0, 1000, 500);
static Pid_Position_t motor_fr_speed_pid = NEW_POSITION_PID(11, 0, 5, 2000, 16000, 0, 1000, 500);
static Pid_Position_t motor_bl_speed_pid = NEW_POSITION_PID(11, 0, 5, 2000, 16000, 0, 1000, 500);
static Pid_Position_t motor_br_speed_pid = NEW_POSITION_PID(11, 0, 5, 2000, 16000, 0, 1000, 500);

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


//�������4������ٶ�PID�������
void Set_Chassis_Motors_Speed(float speed_fl, float speed_fr, float speed_bl, float speed_br)
{
	for(int i=0;i<4;i++){motor_ampere[i]=0;}//ÿһ�ζ���յ�������ֹ����
	motor_ampere[0]=Pid_Position_Calc(&motor_fl_speed_pid, speed_fl, chassis_motor[0].speed_rpm);
	motor_ampere[1]=Pid_Position_Calc(&motor_fr_speed_pid, speed_fr, chassis_motor[1].speed_rpm);
	motor_ampere[2]=Pid_Position_Calc(&motor_bl_speed_pid, speed_bl, chassis_motor[2].speed_rpm);
	motor_ampere[3]=Pid_Position_Calc(&motor_br_speed_pid, speed_br, chassis_motor[3].speed_rpm);
	

//	printf("chassis_motor[0]=%d\r\n",chassis_motor[0].speed_rpm);
//	printf("chassis_motor[1]=%d\r\n",chassis_motor[1].speed_rpm);
//	printf("chassis_motor[2]=%d\r\n",chassis_motor[2].speed_rpm);
//	printf("chassis_motor[3]=%d\r\n",chassis_motor[3].speed_rpm);
	
	
	
	Can1_Send_4Msg(CAN_CHASSIS_ALL_ID,motor_ampere);
}

#if CAN1_RX0_INT_ENABLE	//�����������ѯ�ķ�ʽ��can��������ÿ��ʹ��ǰ���ã�
//�жϷ�����	
void CAN1_RX0_IRQHandler(void)
{
	CAN1->IER&=~(1<<1);  //�ر�FIFO��Ϣ�����ж�
//	//debug
//	LED_RED_OFF;
	
	u8 rxbuf[8];
	u32 id;
	u8 ide,rtr,len; 
	if(CAN1_Msg_Pend(0)==0&&CAN1_Msg_Pend(1)==0){printf("Can1δ���յ�����\r\n");}      
 	CAN1_Rx_Msg(0,&id,&ide,&rtr,&len,rxbuf);  //ʹ��FIFO0�������䣬��ȡ���ݷ���rxbuf��
	if(ide!=0||rtr!=0){printf("can1���������쳣\r\n");}
//	//dubug
//	printf("ID=%d\r\n",id);
	
	switch(id)
	{
		case 1:Calculate_Motor_Data(&chassis_motor[0],rxbuf);break;
		case 2:Calculate_Motor_Data(&chassis_motor[1],rxbuf);break;
		case 3:Calculate_Motor_Data(&chassis_motor[2],rxbuf);break;
		case 4:Calculate_Motor_Data(&chassis_motor[3],rxbuf);break;	
		default:break;
	}
	
	CAN1->RF0R &=~(1<<5);//�ͷ�FIFO0�������
	CAN1->IER|=1<<1;     //��FIFO��Ϣ�����ж�
  
////debug
//	LED_RED_ON;  
}


#endif
