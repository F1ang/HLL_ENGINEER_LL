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

/* 函数声明 */
static void Robot_Rc_Mode_Change_Control(void);
void Switch_Mouse_Key_Change(Rc_ctrl_t* rc, Rc_ctrl_t* last_rc, Robot_mode_t* robot_mode);

/* 变量 */
TaskHandle_t RemoterTask_Handler;
Rc_ctrl_t remote_controller;  //本次获取的遥控器数据
static Rc_ctrl_t last_time_rc;  //上一次的遥控器数据
Robot_mode_t robot_mode;  //机器人模式
static const uint8_t* rc_rx_buf[2];  //串口1接收原始数据数组指针
static SemaphoreHandle_t rc_data_update_semaphore;  //串口1DMA接收信号量
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

	//二值信号量初始化
	rc_data_update_semaphore = xSemaphoreCreateBinary();
	
	//取得遥控器DBUS原始数据指针
	rc_rx_buf[0] = Get_Rc_Bufferx(0);
	rc_rx_buf[1] = Get_Rc_Bufferx(1);  
	
//	judge_data = Get_Judge_Data();

	//重置遥控器数据
	Rc_Data_Reset(&remote_controller);
	Rc_Data_Reset(&last_time_rc);  
	
	//初始化机器人模式
	{
		robot_mode.control_device=2;  //操控设备选择 1 键鼠  2遥控器
		robot_mode.mode_up=1; //模式 1放下 2抬小10cm 3 抬大20cm             //s2
	  robot_mode.mode_stretch=1;// 1缩回 2伸中250 3伸小15cm  4伸大        //ch1
	  robot_mode.mode_chip=1;//1松开 2夹取                                 //s1 
	  robot_mode.mode_overturn=1;//1平 2翻                                //ch4
		//robot_mode.action=1;//1兑换 2资源岛
	}  
	
	vTaskDelay(200);  
	
	//开启串口1DMA接收完成中断
	LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);

	while(1)
	{
		
		//等待信号量，超时时间50ms
		if( xSemaphoreTake(rc_data_update_semaphore, 500) == pdTRUE )
		{
			/* 获取当前遥控器可用原始数据 */
			rx_available_bufx = Get_Rc_Available_Bufferx();

			/* 解析遥控器数据 */
			Parse_Remoter_Data(rc_rx_buf[rx_available_bufx], &remote_controller);

			/* 检测遥控器数据，是否合法如果不合法采取操作 */
			if(Remoter_Data_Check(&remote_controller))
			{
				//保持当前遥控器数据不变
				Rc_Data_Copy(&remote_controller, &last_time_rc);
				//重置串口1及DMA
				Usart1_DMA_Reset();
				
				DEBUG_ERROR(100);
			}
			
			/* 更新遥控器状态 */
//			Detect_Reload(0);
			
			/* 机器人模式变换响应 */
			Robot_Rc_Mode_Change_Control();
			
			/* 响应键盘控制 */
//			Switch_Mouse_Key_Change(&remote_controller, &last_time_rc, &robot_mode);
			
			/* 保存本次遥控器状态 */
			Rc_Data_Copy(&last_time_rc, &remote_controller);
			
		}
		
		//若等待信号量超时
		else
		{
			Rc_Data_Reset(&remote_controller);
			Rc_Data_Reset(&last_time_rc);
		}
		
	}
	
	//vTaskDelete(NULL);
}

/*
  函数名：Rc_Data_Update
	描述  ：在遥控器串口DMA接收函数时调用，用于通知此任务（remoter task）遥控器数据接收完毕
  参数  ：无
  返回值：无
*/
void Rc_Data_Update(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	// 释放二值信号量，发送接收到新数据标志，供前台程序查询
	xSemaphoreGiveFromISR(rc_data_update_semaphore, &pxHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

/*
  函数名：Get_Remote_Control_Point
	描述  ：获取遥控器数据
  参数  ：无
  返回值：Rc_ctrl_t结构体变量
*/
const Rc_ctrl_t *Get_Remote_Control_Point(void)
{
    return &remote_controller;
}

/*
  函数名：Get_Robot_Mode_Point
	描述  ：获取机器人模式数据
  参数  ：无
  返回值：Robot_mode_t结构体变量
*/
Robot_mode_t *Get_Robot_Mode_Point(void)
{
    return &robot_mode;
}

/* 响应遥控器按键切换模式 */
static void Robot_Rc_Mode_Change_Control(void)
{
	//抬升
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
	/* 操控设备选择 */
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
	
	/* 抬升模式 */       
	if(S2_CHANGED_TO(3,2))
	{
		robot_mode.mode_up++;
		if(robot_mode.mode_up==4) robot_mode.mode_up=1;
//		Set_Beep_Time(robot_mode.mode_up, 1200, 50);
	}
  
	if(S2_CHANGED_TO(3,1))//伸出
	{
		stretch_flag=1;
		robot_mode.mode_stretch++;
		if(robot_mode.mode_stretch==5) robot_mode.mode_stretch=1;
//		Set_Beep_Time(robot_mode.mode_stretch, 1200, 50);
	}
	
	if(S1_CHANGED_TO(3,1))//夹取
	{
		chip_flag=1;
		robot_mode.mode_chip++;
		if(robot_mode.mode_chip==3) robot_mode.mode_chip=1;
//		Set_Beep_Time(robot_mode.mode_chip, 1200, 50);
		//INFO_LOG("夹取\r\n");
	}
	
	if(S1_CHANGED_TO(3,2)&&robot_mode.mode_overturn==1)//action
	{
		//angle_start=0;
		//overturn_angle=overturn_motor.mechanical_angle;
		robot_mode.mode_overturn=2;
//		Set_Beep_Time(robot_mode.mode_overturn, 1200, 50);
	}

}

///* 响应键盘切换模式 */
//void Switch_Mouse_Key_Change(Rc_ctrl_t* rc, Rc_ctrl_t* last_rc, Robot_mode_t* robot_mode)
//{
//	//判断是不是键鼠模式
//	if(robot_mode->control_device != 1)
//	{
//		return;
//	}
//	if(remote_controller.mouse.press_l)//伸出
//	{
//		stretch_flag=1;
//		robot_mode->mode_stretch++;
//		if(robot_mode->mode_stretch==3) robot_mode->mode_stretch=1;
//		Set_Beep_Time(robot_mode->mode_stretch, 1200, 50);
//	}
//	if(remote_controller.mouse.press_r)//夹取
//	{
//		chip_flag=1;
//		robot_mode->mode_chip++;
//		if(robot_mode->mode_chip==3) robot_mode->mode_chip=1;
//		Set_Beep_Time(robot_mode->mode_chip, 1200, 50);
//	}
//  if(KEY_CLICKED(KEY_E)&&robot_mode->mode_overturn==1)//翻转
//	{
//		overturn_angle=overturn_motor_li.mechanical_angle;
//		overturn_angle_now=overturn_angle;
//		robot_mode->mode_overturn=2;
//		Set_Beep_Time(robot_mode->mode_overturn, 1200, 50);
//	}
//}

