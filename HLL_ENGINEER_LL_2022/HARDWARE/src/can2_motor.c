#include "can2_motor.h"

/* ������� */
M3508_Mileage overturn_motor_li;
																						//p i d  max_err_integral max_out  is_integral_spare begin_integral stop_grow_integral
static Pid_Position_t motor_overturn_speed_pid = NEW_POSITION_PID(2.5, 0, 0.02, 800, 14000, 0, 200, 500); 			//��ת����ٶ�PID
static Pid_Position_t motor_overturn_angle_pid = NEW_POSITION_PID(0.1, 0.05, 0, 600, 8100, 0, 200, 500);				//��ת����Ƕ�PID

static s16 motor_ampere[4]={0,0,0,0};
/* �������� */
extern u16 overturn_angle;


/* CAN2 send and receive ID wheel_left */
typedef enum
{
	CAN_OVERTURN_ALL_ID = 0x200,
	CAN_OVERTURN_ID = 0x201,
}can2_msg_id_e;


//����һ����ת���
void Set_Overturn_Motors_Speed(float speed_overturn)
{
	for(int i=0;i<4;i++){motor_ampere[i]=0;}//ÿһ�ζ���յ�������ֹ���� ,rpm(תÿ����)
	motor_ampere[0]=Pid_Position_Calc(&motor_overturn_speed_pid, speed_overturn, overturn_motor_li.speed_rpm);
	MotorM3508_Send_Ampere(0x200,motor_ampere);
}
void Set_Overturn_Motors_Angle(int angle_overturn)
{
	Pid_Position_Calc(&motor_overturn_angle_pid, angle_overturn, overturn_motor_li.total_angle);
	Set_Overturn_Motors_Speed(motor_overturn_angle_pid.output);
}
#if CAN2_RX0_INT_ENABLE	//�����������ѯ�ķ�ʽ��can��������ÿ��ʹ��ǰ���ã�
//�жϷ�����
void CAN2_RX0_IRQHandler(void)
{
	CAN2->IER&=~(1<<1);  //�ر�FIFO��Ϣ�����ж�

	u8 rxbuf[8];
	u32 id;
	u8 ide,rtr,len; 
	if(CAN2_Msg_Pend(0)==0&&CAN2_Msg_Pend(1)==0){printf("Can1δ���յ�����\r\n");}    
 	CAN2_Rx_Msg(0,&id,&ide,&rtr,&len,rxbuf);  //ʹ��FIFO0�������䣬��ȡ���ݷ���rxbuf��
	if(ide!=0||rtr!=0){printf("can1���������쳣\r\n");}
	switch(id)
	{
		case 1:get_moto_measure(&overturn_motor_li,rxbuf);break;
	}
	
	CAN2->RF0R &=~(1<<5);//�ͷ�FIFO0�������
	CAN2->IER|=1<<1;     //��FIFO��Ϣ�����ж�
    
}
#endif

