#ifndef __JUDGE_SYSTEM_H
#define __JUDGE_SYSTEM_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include "usart_hll.h"
#include "crc_check.h"
#include "sys.h"

// uart 通信配置，波特率 115200, 数据位 8, 停止位 1，检验位无，流控制无。
// --------------------------------------------------------------------------------
//| FrameHeader(5-Byte) | CmdID(2-Byte) | Data(n-Byte) | FrameTail(2-Byte, CRC16) |
// --------------------------------------------------------------------------------
//crc8 generator polynomial:G(x)=x8+x5+x4+1


/* 命令ID */
typedef enum
{
	Buff_Data=0x0204,          //机器人增益：0x0204。发送频率：1Hz
	Bullet_Remain_Data=0x0208, //子弹剩余发射数：0x0208。发送频率：10Hz 周期发送
	RFID_Data=0x0209,           //机器人 RFID 状态：0x0209。发送频率：1Hz

  Robot_HP=0x0003,         //机器人血量数据：0x0003。发送频率：1Hz
	Robot_Status=0x0201,				//比赛机器人状态：0x0201。发送频率：10Hz
	Power_Heat=0x0202						// 实时功率热量数据：0x0202。发送频率：50Hz
}Cmd_ID;

typedef struct 
{
  __packed struct
  {
    uint16_t bullet_remaining_num_17mm;
    uint16_t bullet_remaining_num_42mm;
    uint16_t coin_remaining_num;
  }ext_bullet_remaining_t;
	
	__packed struct
  {
    uint32_t rfid_status;
  }ext_rfid_status_t;
	
  __packed struct
  {
    uint8_t power_rune_buff;
  }ext_buff_t;

  __packed struct
  {
    uint16_t red_1_robot_HP;
    uint16_t red_2_robot_HP; 
    uint16_t red_3_robot_HP; 
    uint16_t red_4_robot_HP; 
    uint16_t red_5_robot_HP; 
    uint16_t red_7_robot_HP; 
    uint16_t red_outpost_HP;
    uint16_t red_base_HP; 
    uint16_t blue_1_robot_HP; 
    uint16_t blue_2_robot_HP; 
    uint16_t blue_3_robot_HP; 
    uint16_t blue_4_robot_HP; 
    uint16_t blue_5_robot_HP; 
    uint16_t blue_7_robot_HP; 
    uint16_t blue_outpost_HP;
    uint16_t blue_base_HP;
  }ext_game_robot_HP_t;
	
	 __packed struct
	{
		uint8_t robot_id;
		uint8_t robot_level;
		uint16_t remain_HP;
		uint16_t max_HP;
		uint16_t shooter_id1_17mm_cooling_rate;
		uint16_t shooter_id1_17mm_cooling_limit;
		uint16_t shooter_id1_17mm_speed_limit;
		uint16_t shooter_id2_17mm_cooling_rate;
		uint16_t shooter_id2_17mm_cooling_limit;
		uint16_t shooter_id2_17mm_speed_limit;
		uint16_t shooter_id1_42mm_cooling_rate;
		uint16_t shooter_id1_42mm_cooling_limit;
		uint16_t shooter_id1_42mm_speed_limit;
		uint16_t chassis_power_limit;
		uint8_t mains_power_gimbal_output : 1;
		uint8_t mains_power_chassis_output : 1;
		uint8_t mains_power_shooter_output : 1;
	}ext_game_robot_status_t;
	
	 __packed struct
	{
		uint16_t chassis_volt; 
		uint16_t chassis_current; 
		float chassis_power; 
		uint16_t chassis_power_buffer; 
		uint16_t shooter_id1_17mm_cooling_heat;
		uint16_t shooter_id2_17mm_cooling_heat;
		uint16_t shooter_id1_42mm_cooling_heat;
	}ext_power_heat_data_t;
	
	
}Judge_data_t;

extern Judge_data_t* Get_Judge_Data(void);

extern uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len);

extern u8 Is_Id1_17mm_Excess_Heat(const Judge_data_t* judge_data);

#endif
