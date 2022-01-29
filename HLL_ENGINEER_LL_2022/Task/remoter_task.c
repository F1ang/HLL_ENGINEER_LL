#include "remoter_task.h"
#include "remoter.h"
////#include "judge_system.h"
////#include "buzzer_task.h"
////#include "shooter_task.h"
////#include "detect_task.h"
////#include "Motor.h"
#define S1_VALUE       remote_controller.rc.s1
#define S2_VALUE       remote_controller.rc.s2
#define OLD_S1_VALUE   last_time_rc.rc.s1
#define OLD_S2_VALUE   last_time_rc.rc.s2
#define KEY_VALUE      remote_controller.key.value
#define OLD_KEY_VALUE  last_time_rc.key.value

#define S1_AT(a)       (S1_VALUE == (a))
#define S2_AT(a)       (S2_VALUE == (a))

#define S1_CHANGED_TO(a,b) ((OLD_S1_VALUE == (a)) && (S1_VALUE == (b)))
#define S2_CHANGED_TO(a,b) ((OLD_S2_VALUE == (a)) && (S2_VALUE == (b)))

#define KEY_CLICKED(key)   (RC_KEY_PRESSED(KEY_VALUE,key) && (!RC_KEY_PRESSED(OLD_KEY_VALUE,key)))

/* �������� */
static void Robot_Rc_Mode_Change_Control(void);
void Switch_Mouse_Key_Change(Rc_ctrl_t* rc, Rc_ctrl_t* last_rc, Robot_mode_t* robot_mode);

/* ���� */
TaskHandle_t RemoterTask_Handler;
Rc_ctrl_t remote_controller;  //���λ�ȡ��ң��������
static Rc_ctrl_t last_time_rc;  //��һ�ε�ң��������
Robot_mode_t robot_mode;  //������ģʽ
static const uint8_t* rc_rx_buf[2];  //����1����ԭʼ��������ָ��
static SemaphoreHandle_t rc_data_update_semaphore;  //����1DMA�����ź���
////static const Judge_data_t* judge_data;
uint8_t up_flag;
uint8_t stretch_flag;
uint8_t chip_flag;
uint8_t overturn_flag;
////extern u8 angle_start;
////extern Motor_measure_t overturn_motor;
////extern M3508_Mileage overturn_motor_li;
//extern uint16_t overturn_angle;    
//extern uint8_t overturn_seq;
//extern uint16_t overturn_angle_now;
void Remoter_Task(void *pvParameters)
{
	static uint8_t rx_available_bufx;

	//��ֵ�ź�����ʼ��
	rc_data_update_semaphore = xSemaphoreCreateBinary();
	
	//ȡ��ң����DBUSԭʼ����ָ��
	rc_rx_buf[0] = Get_Rc_Bufferx(0);
	rc_rx_buf[1] = Get_Rc_Bufferx(1);  
	
//	judge_data = Get_Judge_Data();

	//����ң��������
	Rc_Data_Reset(&remote_controller);
	Rc_Data_Reset(&last_time_rc);  
	
	//��ʼ��������ģʽ
	{
		robot_mode.control_device=2;  //�ٿ��豸ѡ�� 1 ����  2ң����
		robot_mode.mode_up=1; //ģʽ 1���� 2̧С10cm 3 ̧��20cm             //s2
	  robot_mode.mode_stretch=1;// 1���� 2����250 3��С15cm  4���        //ch1
	  robot_mode.mode_chip=1;//1�ɿ� 2��ȡ                                 //s1 
	  robot_mode.mode_overturn=1;//1ƽ 2��                                //ch4
		//robot_mode.action=1;//1�һ� 2��Դ��
	}  
	
	vTaskDelay(200);  
	
	//��������1DMA��������ж�
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);

	while(1)
	{
		
		//�ȴ��ź�������ʱʱ��50ms
		if( xSemaphoreTake(rc_data_update_semaphore, 500) == pdTRUE )
		{
			/* ��ȡ��ǰң��������ԭʼ���� */
			rx_available_bufx = Get_Rc_Available_Bufferx();

			/* ����ң�������� */
			Parse_Remoter_Data(rc_rx_buf[rx_available_bufx], &remote_controller);

			/* ���ң�������ݣ��Ƿ�Ϸ�������Ϸ���ȡ���� */
			if(Remoter_Data_Check(&remote_controller))
			{
				//���ֵ�ǰң�������ݲ���
				Rc_Data_Copy(&remote_controller, &last_time_rc);
				//���ô���1��DMA
				Usart1_DMA_Reset();
				
				DEBUG_ERROR(100);
			}
			
			/* ����ң����״̬ */
//			Detect_Reload(0);
			
			/* ������ģʽ�任��Ӧ */
			Robot_Rc_Mode_Change_Control();
			
			/* ��Ӧ���̿��� */
//			Switch_Mouse_Key_Change(&remote_controller, &last_time_rc, &robot_mode);
			
			/* ���汾��ң����״̬ */
			Rc_Data_Copy(&last_time_rc, &remote_controller);
			
		}
		
		//���ȴ��ź�����ʱ
		else
		{
			Rc_Data_Reset(&remote_controller);
			Rc_Data_Reset(&last_time_rc);
		}
		
	}
	
	//vTaskDelete(NULL);
}

/*
  ��������Rc_Data_Update
	����  ����ң��������DMA���պ���ʱ���ã�����֪ͨ������remoter task��ң�������ݽ������
  ����  ����
  ����ֵ����
*/
void Rc_Data_Update(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	// �ͷŶ�ֵ�ź��������ͽ��յ������ݱ�־����ǰ̨�����ѯ
	xSemaphoreGiveFromISR(rc_data_update_semaphore, &pxHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

/*
  ��������Get_Remote_Control_Point
	����  ����ȡң��������
  ����  ����
  ����ֵ��Rc_ctrl_t�ṹ�����
*/
const Rc_ctrl_t *Get_Remote_Control_Point(void)
{
    return &remote_controller;
}

/*
  ��������Get_Robot_Mode_Point
	����  ����ȡ������ģʽ����
  ����  ����
  ����ֵ��Robot_mode_t�ṹ�����
*/
Robot_mode_t *Get_Robot_Mode_Point(void)
{
    return &robot_mode;
}

/* ��Ӧң���������л�ģʽ */
static void Robot_Rc_Mode_Change_Control(void)
{
	//̧��
	if(S2_CHANGED_TO(3,2))
	{
		robot_mode.mode_up=2;
		
//		Set_Beep_Time(robot_mode.mode_up, 1000, 55);
	}
	if(S2_CHANGED_TO(3,1))
	{
		robot_mode.mode_up=3;
//		Set_Beep_Time(robot_mode.mode_up, 1000, 55);
	}
	if(S2_CHANGED_TO(1,2))
	{
		robot_mode.mode_up=1;
//		Set_Beep_Time(robot_mode.mode_up, 1000, 55);
	}
	if(S2_CHANGED_TO(2,3))
	{
		robot_mode.mode_up=1;
//		Set_Beep_Time(robot_mode.mode_up, 1000, 55);
	}
	if(S1_CHANGED_TO(3,1))
	{
		robot_mode.mode_chip=2;
		//chip_flag=1;
//		Set_Beep_Time(robot_mode.mode_chip, 1000, 55);
	}
	if(S1_CHANGED_TO(1,3))
	{
		robot_mode.mode_chip=1;
		//chip_flag=1;
//		Set_Beep_Time(robot_mode.mode_chip, 1000, 55);
	}
	/* �ٿ��豸ѡ�� */
//	if(S2_CHANGED_TO(3,2))
//	{
//		robot_mode.control_device++;
//		if(robot_mode.control_device==3) robot_mode.control_device=1;
//		Set_Beep_Time(robot_mode.control_device, 1000, 55);
//	}
//	
//		if(robot_mode.control_device != 2)
//	{
//		return;
//	}
	
	/* ̧��ģʽ */       
	if(S2_CHANGED_TO(3,2))
	{
		robot_mode.mode_up++;
		if(robot_mode.mode_up==4) robot_mode.mode_up=1;
//		Set_Beep_Time(robot_mode.mode_up, 1200, 50);
	}
  
	if(S2_CHANGED_TO(3,1))//���
	{
		stretch_flag=1;
		robot_mode.mode_stretch++;
		if(robot_mode.mode_stretch==5) robot_mode.mode_stretch=1;
//		Set_Beep_Time(robot_mode.mode_stretch, 1200, 50);
	}
	
	if(S1_CHANGED_TO(3,1))//��ȡ
	{
		chip_flag=1;
		robot_mode.mode_chip++;
		if(robot_mode.mode_chip==3) robot_mode.mode_chip=1;
//		Set_Beep_Time(robot_mode.mode_chip, 1200, 50);
		//INFO_LOG("��ȡ\r\n");
	}
	
	if(S1_CHANGED_TO(3,2)&&robot_mode.mode_overturn==1)//action
	{
		//angle_start=0;
		//overturn_angle=overturn_motor.mechanical_angle;
		robot_mode.mode_overturn=2;
//		Set_Beep_Time(robot_mode.mode_overturn, 1200, 50);
	}

}

///* ��Ӧ�����л�ģʽ */
//void Switch_Mouse_Key_Change(Rc_ctrl_t* rc, Rc_ctrl_t* last_rc, Robot_mode_t* robot_mode)
//{
//	//�ж��ǲ��Ǽ���ģʽ
//	if(robot_mode->control_device != 1)
//	{
//		return;
//	}
//	if(remote_controller.mouse.press_l)//���
//	{
//		stretch_flag=1;
//		robot_mode->mode_stretch++;
//		if(robot_mode->mode_stretch==3) robot_mode->mode_stretch=1;
//		Set_Beep_Time(robot_mode->mode_stretch, 1200, 50);
//	}
//	if(remote_controller.mouse.press_r)//��ȡ
//	{
//		chip_flag=1;
//		robot_mode->mode_chip++;
//		if(robot_mode->mode_chip==3) robot_mode->mode_chip=1;
//		Set_Beep_Time(robot_mode->mode_chip, 1200, 50);
//	}
//  if(KEY_CLICKED(KEY_E)&&robot_mode->mode_overturn==1)//��ת
//	{
//		overturn_angle=overturn_motor_li.mechanical_angle;
//		overturn_angle_now=overturn_angle;
//		robot_mode->mode_overturn=2;
//		Set_Beep_Time(robot_mode->mode_overturn, 1200, 50);
//	}
//}

