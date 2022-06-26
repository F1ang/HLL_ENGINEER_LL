/*
	1��ң����:
	S1(3,1)��1-����2-ң��
	S2(3,2)��1-̧С��2-̧�У�3-̧��     ��Ԯ(��ʱ������)
	S2(1,3)��1-���أ�2-����250��3-��С15cm ��4-���
	S2(3,1)��1-ƽ��2-��(�н������ص�1->��ת)   ���(��ʱ������) 1-�ɣ�2-��
  CH2-����ƽ�ƣ�CH1-����ͷ ��CH0-���ң�CH3-ǰ��
	
	2������:
	ģʽ��M��1-����2-ң��(��ѡ)
	
	̧����Q��1-���� 2-̧С  3-̧��    Q+CTRL������  1-���º�΢����
	 
	��Ԯצ��F��1-����� 2-���
	
	
	������E��1-���� 2-��С 3-���� 4-���	 E+CTRL��1-����
	  
	
	צ�ӣ�R��1-����0-ƽ    ---->1-�� 2-ƽ 3-�� 
	�����G��1-�죬0-��  ---->ֹͣY�� 
	��ȡ��C��1-�У�0-��


	���̣�W-ǰ��S-��A-��ƽ��,D-��ƽ�ƣ�  (���Ż�-SHIFT+W/S/A/D,���ٳ��)
	��Ļ��remote_controller.x-������    
	����ͷ��remote_controller.y-������:��ֵֹΪ0
	
	ע:
	V-̧��������remote_controller.press_l��2-����   remote_controller.press_r��3-�ҽ� 
	B-����������Z��2-��   X,3-��
*/

#include "remoter_task.h"  
#include "remoter.h"

#define S1_VALUE       remote_controller.rc.s1
#define S2_VALUE       remote_controller.rc.s2
#define OLD_S1_VALUE   last_time_rc.rc.s1
#define OLD_S2_VALUE   last_time_rc.rc.s2

#define S1_AT(a)       (S1_VALUE == (a))
#define S2_AT(a)       (S2_VALUE == (a))

#define S1_CHANGED_TO(a,b) ((OLD_S1_VALUE == (a)) && (S1_VALUE == (b)))
#define S2_CHANGED_TO(a,b) ((OLD_S2_VALUE == (a)) && (S2_VALUE == (b)))

//����
#define OLD_KEY_VALUE  last_time_rc.key.value
#define KEY_VALUE      			remote_controller.key.value
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
//flag
uint8_t up_flag;
uint8_t stretch_flag;
uint8_t chip_flag;
uint8_t overturn_flag;

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
		robot_mode.mode_up=1; //ģʽ 1���� 2̧С10cm  3̧��20cm             
	  robot_mode.mode_stretch=1;// 1���� 2����250 3��С15cm  4���        
	  robot_mode.mode_chip=1;//1�ɿ� 2��ȡ                                 
	  robot_mode.mode_overturn=1;//1ƽ 2�� --->1-�� 2-ƽ 3-��                             
		//robot_mode.action=1;//1�һ� 2��Դ��
		robot_mode.mode_rescue=1;//1����Ԯ2��Ԯ
		robot_mode.mode_revive=1;//1�����2����
		robot_mode.open=1;//1����2�������3����̧��
		//����
		robot_mode.mode_open_up=1;//1-������2����
		robot_mode.mode_open_stretch=1;//1-������2����
		robot_mode.mode_up_small_l=1;//1-���� 2-����΢�� 
		robot_mode.mode_up_small_r=1;//1-���� 2-����΢�� 
		robot_mode.mode_stretch_small=1;//1-���� 2-��΢�� 3-��΢��
	}  
	
	//��ʼ��
	usart1_base_init(); 
	vTaskDelay(200);  	 
	
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
			Detect_Reload(0);  
			
			/* ң�������� */
			Robot_Rc_Mode_Change_Control();
			
			/* ��Ӧ���̿��� */
			Switch_Mouse_Key_Change(&remote_controller, &last_time_rc, &robot_mode);
			
			/* ���汾��ң����״̬ */
			Rc_Data_Copy(&last_time_rc, &remote_controller);
			
		}
		
		//���ȴ��ź�����ʱ
		else
		{
			Rc_Data_Reset(&remote_controller);
			Rc_Data_Reset(&last_time_rc);
		}
				vTaskDelay(5);  
	}
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
	if(S1_CHANGED_TO(3,1))
	{
		robot_mode.open++;
		if(robot_mode.open>=4)robot_mode.open=1;
		Set_Beep_Time(robot_mode.open, 1000, 55);
	}
	//�����豸ѡ��
	if(S1_CHANGED_TO(3,2))
	{
		robot_mode.control_device++;
		if(robot_mode.control_device==3) robot_mode.control_device=1;
		Set_Beep_Time(robot_mode.control_device, 1000, 55);
	}
		if(robot_mode.control_device != 2)
	{
		return;
	}
	
// ��Ԯoŗ��
	if(S2_CHANGED_TO(3,2))
	{
//		robot_mode.mode_rescue++;
//		if(robot_mode.mode_rescue==3)robot_mode.mode_rescue=1;
		
//		if(up_flag==0)robot_mode.mode_up++;//˳������˳��,ʵ��
//		else robot_mode.mode_up--;
//		
//		if(robot_mode.mode_up==3)up_flag=1;
//		if(robot_mode.mode_up==1)up_flag=0;	
		
		robot_mode.mode_up+=1;
		if(robot_mode.mode_up>3)robot_mode.mode_up=1;
		Set_Beep_Time(robot_mode.mode_up, 1200, 50);
	}
	
////����
//	if(S2_CHANGED_TO(1,3))
//	{
//		robot_mode.mode_stretch++;
//		if(robot_mode.mode_stretch>4)robot_mode.mode_stretch=1;
//		Set_Beep_Time(robot_mode.mode_stretch, 1200, 50);
//	}
	
//��� or ��ת or ��ȡ
	if(S2_CHANGED_TO(3,1))
	{
//		robot_mode.mode_revive++;
//		if(robot_mode.mode_revive==3)robot_mode.mode_revive=1;
//		robot_mode.mode_chip++;
//		if(robot_mode.mode_chip>2) robot_mode.mode_chip=0;
		
//		if(robot_mode.mode_chip==1) robot_mode.mode_overturn=1;//��->ƽ
		
		robot_mode.mode_overturn+=1;
		if(robot_mode.mode_overturn>2) robot_mode.mode_overturn=1;//�н������ص�1->��
  	Set_Beep_Time(robot_mode.mode_overturn, 1200, 50);
		
//		//1-�� 2-ƽ 3-��
//		if(robot_mode.mode_overturn>=3)overturn_flag=1;
//		else if(robot_mode.mode_overturn<=1)overturn_flag=0;
//		if (overturn_flag==0)robot_mode.mode_overturn+=1;
//		else robot_mode.mode_overturn-=1;
//  	Set_Beep_Time(robot_mode.mode_overturn, 1200, 50);
	}
	
}

/* ��Ӧ�����л�ģʽ */
void Switch_Mouse_Key_Change(Rc_ctrl_t* rc, Rc_ctrl_t* last_rc, Robot_mode_t* robot_mode)
{
	//�ж��ǲ��Ǽ���ģʽ
	if(robot_mode->control_device != 1)
	{
		return;
	}
										/***̧��***/
	//������ջ�
	if(KEY_CLICKED(KEY_V))
	{
		robot_mode->mode_open_up+=1;
		if(robot_mode->mode_open_up>2)robot_mode->mode_open_up=1;
	}
	if(KEY_CLICKED(KEY_B))
	{
		robot_mode->mode_open_stretch+=1;
		if(robot_mode->mode_open_stretch>2)robot_mode->mode_open_stretch=1;
	}
	
	//1���� 2̧С 3 ̧��
	if(KEY_CLICKED(KEY_Q)&!KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_up+=1;
		if(robot_mode->mode_up>3)robot_mode->mode_up=1;
	}
	//����
	else if(KEY_CLICKED(KEY_Q)&KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_up=1;
		robot_mode->mode_up_small_l=1;
		robot_mode->mode_up_small_r=1;
	}
	
	//������
	if(rc->mouse.press_l==1)
	{
		robot_mode->mode_up_small_l=2;
	}
	else robot_mode->mode_up_small_l=1;
	//������
	if(rc->mouse.press_r==1)  
	{		
		robot_mode->mode_up_small_r=2;
	}
	else robot_mode->mode_up_small_r=1;
	
	
										/***��Ԯ����***/
	//1-���� 2-��Ԯ
	if(KEY_CLICKED(KEY_F)==1)
	{
		robot_mode->mode_revive++;
		if(robot_mode->mode_revive==3)robot_mode->mode_revive=1;
	}

										/***����***/
	//1-���� 2-��С 3-���� 4-���	  E+CTRL��1-����
	if(KEY_CLICKED(KEY_E)&!KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_stretch++;
		if(robot_mode->mode_stretch>4)robot_mode->mode_stretch=1;
		Set_Beep_Time(robot_mode->mode_stretch, 1200, 50);
	}
	else if(KEY_CLICKED(KEY_E)&KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_stretch_small=1;
		robot_mode->mode_stretch=1;
	}
	
	//Z��2-��   C,3-��
	if(KEY_CLICKED(KEY_Z)&!KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_stretch_small=2;
		Set_Beep_Time(robot_mode->mode_stretch, 1200, 50);
	}
	if(KEY_CLICKED(KEY_C)&!KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_stretch_small=3;
		Set_Beep_Time(robot_mode->mode_stretch, 1200, 50);
	}
	else if(!KEY_CLICKED(KEY_Z)&!KEY_CLICKED(KEY_CTRL))
		robot_mode->mode_stretch_small=1;
	
										/***��ת***/
	//1-ƽ 2-��
	if(KEY_CLICKED(KEY_R))
	{
		robot_mode->mode_overturn++;
		if(robot_mode->mode_overturn>2) robot_mode->mode_overturn=1;
		Set_Beep_Time(robot_mode->mode_chip, 1200, 50);
		
		//1-�� 2-ƽ 3-��
//		if(robot_mode->mode_overturn>=3)overturn_flag=1;
//		else if(robot_mode->mode_overturn<=1)overturn_flag=0;
//		if (overturn_flag==0)robot_mode->mode_overturn+=1;
//		else robot_mode->mode_overturn-=1;
//  	Set_Beep_Time(robot_mode->mode_overturn, 1200, 50);
	}
										/***���***/
	//1-���죬2-��
	if(KEY_CLICKED(KEY_G))
	{
		robot_mode->mode_rescue++;
		if(robot_mode->mode_rescue>2) robot_mode->mode_rescue=1;
		Set_Beep_Time(robot_mode->mode_rescue, 1200, 50);
	}
											/***��ȡ***/
	//1-�� 2-��
	if(KEY_CLICKED(KEY_C))
	{
		robot_mode->mode_chip++;
		if(robot_mode->mode_chip>2) robot_mode->mode_chip=1;
		Set_Beep_Time(robot_mode->mode_chip, 1200, 50);
	}
	
											/***��Ļ***/
	//remoter.cʵ��
	
}

