/*
	1、遥控器:
	S1(3,1)，1-键鼠，2-遥控
	S2(3,2)，1-抬小，2-抬中，3-抬大     救援(暂时调试用)
	S2(1,3)，1-缩回，2-伸中250，3-伸小15cm ，4-伸大
	S2(3,1)，1-平，2-翻(夹紧和缩回到1->翻转)   复活卡(暂时调试用) 1-松，2-夹
  CH2-左右平移，CH1-摄像头 ，CH0-左右，CH3-前后
	
	2、键鼠:
	模式：M，1-键鼠，2-遥控(可选)
	
	抬升：Q，1-放下 2-抬小  3-抬大    Q+CTRL，放下  1-放下和微调关
	 
	救援爪：F，1-不伸出 2-伸出
	
	
	伸缩：E，1-缩回 2-伸小 3-伸中 4-伸大	 E+CTRL，1-缩回
	  
	
	爪子：R，1-翻，0-平    ---->1-低 2-平 3-翻 
	复活卡：G，1-伸，0-缩  ---->停止Y轴 
	夹取：C，1-夹，0-松


	底盘：W-前，S-后，A-左平移,D-右平移，  (可优化-SHIFT+W/S/A/D,加速冲出)
	屏幕：remote_controller.x-左右移    
	摄像头：remote_controller.y-上下移:静止值为0
	
	注:
	V-抬升开环：remote_controller.press_l，2-左升   remote_controller.press_r，3-右降 
	B-伸缩开环：Z，2-伸   X,3-缩
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

//键鼠
#define OLD_KEY_VALUE  last_time_rc.key.value
#define KEY_VALUE      			remote_controller.key.value
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
//flag
uint8_t up_flag;
uint8_t stretch_flag;
uint8_t chip_flag;
uint8_t overturn_flag;

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
		robot_mode.mode_up=1; //模式 1放下 2抬小10cm  3抬大20cm             
	  robot_mode.mode_stretch=1;// 1缩回 2伸中250 3伸小15cm  4伸大        
	  robot_mode.mode_chip=1;//1松开 2夹取                                 
	  robot_mode.mode_overturn=1;//1平 2翻 --->1-低 2-平 3-翻                             
		//robot_mode.action=1;//1兑换 2资源岛
		robot_mode.mode_rescue=1;//1不救援2救援
		robot_mode.mode_revive=1;//1不伸出2复活
		robot_mode.open=1;//1正常2开环伸出3开环抬升
		//键鼠
		robot_mode.mode_open_up=1;//1-正常，2开环
		robot_mode.mode_open_stretch=1;//1-正常，2开环
		robot_mode.mode_up_small_l=1;//1-正常 2-左升微调 
		robot_mode.mode_up_small_r=1;//1-正常 2-右升微调 
		robot_mode.mode_stretch_small=1;//1-正常 2-伸微调 3-缩微调
	}  
	
	//初始化
	usart1_base_init(); 
	vTaskDelay(200);  	 
	
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
			Detect_Reload(0);  
			
			/* 遥控器控制 */
			Robot_Rc_Mode_Change_Control();
			
			/* 响应键盘控制 */
			Switch_Mouse_Key_Change(&remote_controller, &last_time_rc, &robot_mode);
			
			/* 保存本次遥控器状态 */
			Rc_Data_Copy(&last_time_rc, &remote_controller);
			
		}
		
		//若等待信号量超时
		else
		{
			Rc_Data_Reset(&remote_controller);
			Rc_Data_Reset(&last_time_rc);
		}
				vTaskDelay(5);  
	}
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
	if(S1_CHANGED_TO(3,1))
	{
		robot_mode.open++;
		if(robot_mode.open>=4)robot_mode.open=1;
		Set_Beep_Time(robot_mode.open, 1000, 55);
	}
	//操作设备选择
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
	
// 救援or抬升
	if(S2_CHANGED_TO(3,2))
	{
//		robot_mode.mode_rescue++;
//		if(robot_mode.mode_rescue==3)robot_mode.mode_rescue=1;
		
//		if(up_flag==0)robot_mode.mode_up++;//顺序升，顺序降,实现
//		else robot_mode.mode_up--;
//		
//		if(robot_mode.mode_up==3)up_flag=1;
//		if(robot_mode.mode_up==1)up_flag=0;	
		
		robot_mode.mode_up+=1;
		if(robot_mode.mode_up>3)robot_mode.mode_up=1;
		Set_Beep_Time(robot_mode.mode_up, 1200, 50);
	}
	
////伸缩
//	if(S2_CHANGED_TO(1,3))
//	{
//		robot_mode.mode_stretch++;
//		if(robot_mode.mode_stretch>4)robot_mode.mode_stretch=1;
//		Set_Beep_Time(robot_mode.mode_stretch, 1200, 50);
//	}
	
//复活卡 or 翻转 or 夹取
	if(S2_CHANGED_TO(3,1))
	{
//		robot_mode.mode_revive++;
//		if(robot_mode.mode_revive==3)robot_mode.mode_revive=1;
//		robot_mode.mode_chip++;
//		if(robot_mode.mode_chip>2) robot_mode.mode_chip=0;
		
//		if(robot_mode.mode_chip==1) robot_mode.mode_overturn=1;//松->平
		
		robot_mode.mode_overturn+=1;
		if(robot_mode.mode_overturn>2) robot_mode.mode_overturn=1;//夹紧和缩回到1->翻
  	Set_Beep_Time(robot_mode.mode_overturn, 1200, 50);
		
//		//1-低 2-平 3-翻
//		if(robot_mode.mode_overturn>=3)overturn_flag=1;
//		else if(robot_mode.mode_overturn<=1)overturn_flag=0;
//		if (overturn_flag==0)robot_mode.mode_overturn+=1;
//		else robot_mode.mode_overturn-=1;
//  	Set_Beep_Time(robot_mode.mode_overturn, 1200, 50);
	}
	
}

/* 响应键盘切换模式 */
void Switch_Mouse_Key_Change(Rc_ctrl_t* rc, Rc_ctrl_t* last_rc, Robot_mode_t* robot_mode)
{
	//判断是不是键鼠模式
	if(robot_mode->control_device != 1)
	{
		return;
	}
										/***抬升***/
	//开环或闭环
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
	
	//1放下 2抬小 3 抬大
	if(KEY_CLICKED(KEY_Q)&!KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_up+=1;
		if(robot_mode->mode_up>3)robot_mode->mode_up=1;
	}
	//放下
	else if(KEY_CLICKED(KEY_Q)&KEY_CLICKED(KEY_CTRL))
	{
		robot_mode->mode_up=1;
		robot_mode->mode_up_small_l=1;
		robot_mode->mode_up_small_r=1;
	}
	
	//开环升
	if(rc->mouse.press_l==1)
	{
		robot_mode->mode_up_small_l=2;
	}
	else robot_mode->mode_up_small_l=1;
	//开环降
	if(rc->mouse.press_r==1)  
	{		
		robot_mode->mode_up_small_r=2;
	}
	else robot_mode->mode_up_small_r=1;
	
	
										/***救援钩子***/
	//1-不救 2-救援
	if(KEY_CLICKED(KEY_F)==1)
	{
		robot_mode->mode_revive++;
		if(robot_mode->mode_revive==3)robot_mode->mode_revive=1;
	}

										/***伸缩***/
	//1-缩回 2-伸小 3-伸中 4-伸大	  E+CTRL，1-缩回
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
	
	//Z，2-伸   C,3-缩
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
	
										/***翻转***/
	//1-平 2-翻
	if(KEY_CLICKED(KEY_R))
	{
		robot_mode->mode_overturn++;
		if(robot_mode->mode_overturn>2) robot_mode->mode_overturn=1;
		Set_Beep_Time(robot_mode->mode_chip, 1200, 50);
		
		//1-低 2-平 3-翻
//		if(robot_mode->mode_overturn>=3)overturn_flag=1;
//		else if(robot_mode->mode_overturn<=1)overturn_flag=0;
//		if (overturn_flag==0)robot_mode->mode_overturn+=1;
//		else robot_mode->mode_overturn-=1;
//  	Set_Beep_Time(robot_mode->mode_overturn, 1200, 50);
	}
										/***复活卡***/
	//1-不伸，2-伸
	if(KEY_CLICKED(KEY_G))
	{
		robot_mode->mode_rescue++;
		if(robot_mode->mode_rescue>2) robot_mode->mode_rescue=1;
		Set_Beep_Time(robot_mode->mode_rescue, 1200, 50);
	}
											/***夹取***/
	//1-松 2-夹
	if(KEY_CLICKED(KEY_C))
	{
		robot_mode->mode_chip++;
		if(robot_mode->mode_chip>2) robot_mode->mode_chip=1;
		Set_Beep_Time(robot_mode->mode_chip, 1200, 50);
	}
	
											/***屏幕***/
	//remoter.c实现
	
}

