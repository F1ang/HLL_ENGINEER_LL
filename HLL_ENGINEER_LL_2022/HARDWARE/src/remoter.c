/*
 * 文件名：remoter.c
 * 描述 ：处理遥控器数据
 * 内含串口1DMA接收完成中断服务函数
 * 对遥控器原始数据的处理在DMA接收中断中完成，每次接收需手动使能接收冲断
 * 若接收出错将重启DMA服务
 * 定义了局部结构体变量remote_controller用于储存遥控器数据
*/

/*
		键鼠:WSAD虚拟摇杆解析处理
		摄像头Y轴解析处理
*/

#include "remoter.h"

#include "remoter_task.h"
#include "math2.h"
#define RC_CH_VALUE_MIN ((uint16_t)364)
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)

#define RC_SW_UP ((uint8_t)1)
#define RC_SW_MID ((uint8_t)3)
#define RC_SW_DOWN ((uint8_t)2)

#define ROCKER_DATA_CHECK(data) ((data > 660) || (data < -660))
#define SWITCH_DATA_CHECK(data) (!((data == 1) || (data == 2) || (data == 3)))

#define RC_KEY               rc->key.value
#define KEY_PRESSED(key)     (RC_KEY & key)

#define VIRTUAL_ROCKER_STEP1 5
#define VIRTUAL_ROCKER_STEP2 10

/*
  函数名：Parse_Remoter_Data
	描述  ：处理原始数据
  参数  ：遥控器原始数据数组，遥控器数据结构体变量
  返回值：无
*/
void Parse_Remoter_Data(volatile const uint8_t *sbus_buf, Rc_ctrl_t *rc_ctrl)
{	
	rc_ctrl->rc.ch0 = (sbus_buf[0] | (sbus_buf[1] << 8)) & 0x07ff;        //!< Channel 0
	rc_ctrl->rc.ch1 = ((sbus_buf[1] >> 3) | (sbus_buf[2] << 5)) & 0x07ff; //!< Channel 1
	rc_ctrl->rc.ch2 = ((sbus_buf[2] >> 6) | (sbus_buf[3] << 2) |          //!< Channel 2
											 (sbus_buf[4] << 10)) &
											0x07ff;
	rc_ctrl->rc.ch3 = ((sbus_buf[4] >> 1) | (sbus_buf[5] << 7)) & 0x07ff; //!< Channel 3
	rc_ctrl->rc.s1 = ((sbus_buf[5] >> 4) & 0x0003);                  //!< Switch left
	rc_ctrl->rc.s2 = ((sbus_buf[5] >> 4) & 0x000C) >> 2;                       //!< Switch right
	rc_ctrl->mouse.x = sbus_buf[6] | (sbus_buf[7] << 8);                    //!< Mouse X axis
	rc_ctrl->mouse.y = sbus_buf[8] | (sbus_buf[9] << 8);                    //!< Mouse Y axis
	rc_ctrl->mouse.z = sbus_buf[10] | (sbus_buf[11] << 8);                  //!< Mouse Z axis
	rc_ctrl->mouse.press_l = sbus_buf[12];                                  //!< Mouse Left Is Press ?
	rc_ctrl->mouse.press_r = sbus_buf[13];                                  //!< Mouse Right Is Press ?
	rc_ctrl->key.value = sbus_buf[14] | (sbus_buf[15] << 8);                    //!< KeyBoard value
	rc_ctrl->rc.ch4 = sbus_buf[16] | (sbus_buf[17] << 8);                 //NULL

	rc_ctrl->rc.ch0 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch1 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch2 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch3 -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch4 -= RC_CH_VALUE_OFFSET;        //上770 下-550
	
	/* 将WASD按键数据解析为虚拟遥感 */
	Wasd_Key_To_Virtual_Rocker(rc_ctrl);

	/* 鼠标x、y轴限幅 */
	rc_ctrl->mouse.x = Int16_Limit(rc_ctrl->mouse.x, -1200, 1200);
	rc_ctrl->mouse.y = Int16_Limit(rc_ctrl->mouse.y, -1200, 1200);
	
	/*DEBUG*/
	//DEBUG_PRINT("ch1:%d ch2:%d ch3:%d ch4:%d\r\n", rc_ctrl->virtual_rocker.ch0, rc_ctrl->virtual_rocker.ch1, rc_ctrl->virtual_rocker.ch2, rc_ctrl->virtual_rocker.ch3);
	//DEBUG_PRINT("mx:%d, my:%d, mz:%d\r\n", rc_ctrl->mouse.x, rc_ctrl->mouse.y, rc_ctrl->mouse.z);

}

/*
	函数名：Remoter_Data_Check
	描述  ：校验遥控器数据
	参数  ：无
  返回值：数据出错返回1 数据无误返回0
*/
uint8_t Remoter_Data_Check(Rc_ctrl_t *remote_controller)
{
	if (ROCKER_DATA_CHECK(remote_controller->rc.ch0))
	{
		return 1;
	}
	if (ROCKER_DATA_CHECK(remote_controller->rc.ch1))
	{
		return 2;
	}
	if (ROCKER_DATA_CHECK(remote_controller->rc.ch2))
	{
		return 3;
	}
	if (ROCKER_DATA_CHECK(remote_controller->rc.ch3))
	{
		return 4;
	}
	if (SWITCH_DATA_CHECK(remote_controller->rc.s1))
	{
		return 5;
	}
	if (SWITCH_DATA_CHECK(remote_controller->rc.s2))
	{
		return 6;
	}
	return 0;
}

//将rc2复制给rc1
void Rc_Data_Copy(Rc_ctrl_t* rc1, Rc_ctrl_t* rc2)
{
	rc1->key.value = rc2->key.value;
	
	rc1->mouse.press_l = rc2->mouse.press_l;
	rc1->mouse.press_r = rc2->mouse.press_r;
	rc1->mouse.x = rc2->mouse.x;
	rc1->mouse.y = rc2->mouse.y;
	rc1->mouse.z = rc2->mouse.z;
	
	rc1->rc.ch0 = rc2->rc.ch0;
	rc1->rc.ch1 = rc2->rc.ch1;
	rc1->rc.ch2 = rc2->rc.ch2;
	rc1->rc.ch3 = rc2->rc.ch3;
	rc1->rc.ch4 = rc2->rc.ch4;
	rc1->rc.s1 = rc2->rc.s1;
	rc1->rc.s2 = rc2->rc.s2;
}

//将rc重置
void Rc_Data_Reset(Rc_ctrl_t* rc)
{
	rc->key.value = 0;
	
	rc->mouse.press_l = 0;
	rc->mouse.press_r = 0;
	rc->mouse.x = 0;
	rc->mouse.y = 0;
	rc->mouse.z = 0;
	
	rc->rc.ch0 = 0;
	rc->rc.ch1 = 0;
	rc->rc.ch2 = 0;
	rc->rc.ch3 = 0;
	rc->rc.ch4 = 0;
	rc->rc.s1 = 3;
	rc->rc.s2 = 3;
}

void Wasd_Key_To_Virtual_Rocker(Rc_ctrl_t* rc)
{
	 if(KEY_PRESSED(KEY_A)) DEBUG_PRINT("A\r\n");
	 if(KEY_PRESSED(KEY_W)) DEBUG_PRINT("W\r\n");
	 if(KEY_PRESSED(KEY_S)) DEBUG_PRINT("S\r\n");
	 if(KEY_PRESSED(KEY_D)) DEBUG_PRINT("D\r\n");

   if(KEY_PRESSED(KEY_Z)) DEBUG_PRINT("z\r\n");
	 if(KEY_PRESSED(KEY_X)) DEBUG_PRINT("x\r\n");
	 if(KEY_PRESSED(KEY_C)) DEBUG_PRINT("c\r\n");
	 if(KEY_PRESSED(KEY_V)) DEBUG_PRINT("v\r\n");

	 if(KEY_PRESSED(KEY_F)) DEBUG_PRINT("F\r\n");
	 if(KEY_PRESSED(KEY_G)) DEBUG_PRINT("G\r\n");
	
	/*只按下W时*/
	if(KEY_PRESSED(KEY_W) && !(KEY_PRESSED(KEY_S)))
	{
		if(rc->virtual_rocker.ch3 < 0)
		{
			rc->virtual_rocker.ch3 += VIRTUAL_ROCKER_STEP2;
		}
		if(rc->virtual_rocker.ch3 < 660)
		{
			rc->virtual_rocker.ch3 += VIRTUAL_ROCKER_STEP1;
		}
	}
	/*只按下S时*/
	else if(KEY_PRESSED(KEY_S) && !(KEY_PRESSED(KEY_W)))
	{
		if(rc->virtual_rocker.ch3 > 0)
		{
			rc->virtual_rocker.ch3 -= VIRTUAL_ROCKER_STEP2;
		}
		if(rc->virtual_rocker.ch3 > -660)
		{
			rc->virtual_rocker.ch3 -= VIRTUAL_ROCKER_STEP1;
		}
	}
	/*W、S都未按下时*/
	else
	{
		if(rc->virtual_rocker.ch3 < 0)
		{
			rc->virtual_rocker.ch3 += VIRTUAL_ROCKER_STEP2;
			if(rc->virtual_rocker.ch3 > 0)
				rc->virtual_rocker.ch3 = 0;
		}
		else if(rc->virtual_rocker.ch3 > 0)
		{
			rc->virtual_rocker.ch3 -= VIRTUAL_ROCKER_STEP2;
			if(rc->virtual_rocker.ch3 < 0)
				rc->virtual_rocker.ch3 = 0;
		}
	}
																												/***左右平移***/
	//CH2
	/*只按下D时*/
	if(KEY_PRESSED(KEY_A) && !(KEY_PRESSED(KEY_D)))
	{
		if(rc->virtual_rocker.ch2 < 0)
		{
			rc->virtual_rocker.ch2 += VIRTUAL_ROCKER_STEP2;
		}
		if(rc->virtual_rocker.ch2 < 660)
		{
			rc->virtual_rocker.ch2 += VIRTUAL_ROCKER_STEP1;
		}
	}
	/*只按下A时*/
	else if(KEY_PRESSED(KEY_D) && !(KEY_PRESSED(KEY_A)))
	{
		if(rc->virtual_rocker.ch2 > 0)
		{
			rc->virtual_rocker.ch2 -= VIRTUAL_ROCKER_STEP2;
		}
		if(rc->virtual_rocker.ch2 > -660)
		{
			rc->virtual_rocker.ch2 -= VIRTUAL_ROCKER_STEP1;
		}
	}
	/*A、D都未按下时,斜波函数*/
	else
	{
		if(rc->virtual_rocker.ch2 < 0)
		{
			rc->virtual_rocker.ch2 += VIRTUAL_ROCKER_STEP2;
			if(rc->virtual_rocker.ch2 > 0)
				rc->virtual_rocker.ch2 = 0;
		}
		else if(rc->virtual_rocker.ch2 > 0)
		{
			rc->virtual_rocker.ch2 -= VIRTUAL_ROCKER_STEP2;
			if(rc->virtual_rocker.ch2 < 0)
				rc->virtual_rocker.ch2 = 0;
		}
	}
																					/***左右平移_END***/
		//左右转 CH0
		if(rc->mouse.x!=0)
		{
			//if(rc->mouse.x<=2||rc->mouse.x>=-2)rc->mouse.x=0;
			rc->virtual_rocker.ch0=rc->mouse.x*6;
			if(rc->virtual_rocker.ch0>660)rc->virtual_rocker.ch0=660;
			if(rc->virtual_rocker.ch0<-660)rc->virtual_rocker.ch0=-660;
		}
		else rc->virtual_rocker.ch0=rc->virtual_rocker.ch0;

		//摄像头
		if(robot_mode.mode_revive==1)
		{
			if(rc->mouse.y!=0)
			{
				rc->virtual_rocker.ch1+=rc->mouse.y;
				if(rc->virtual_rocker.ch1>660)rc->virtual_rocker.ch1=660;
				if(rc->virtual_rocker.ch1<-660)rc->virtual_rocker.ch1=-660;
			}
			else rc->virtual_rocker.ch1=rc->virtual_rocker.ch1;
		}	
		else {}
		
}
