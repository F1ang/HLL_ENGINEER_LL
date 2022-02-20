#include "judge_system.h"
//#include "crc_check.h"
#include "math2.h"
#include "remoter_task.h"
//#include "shooter_task.h"
#include "power_output.h"

/* 裁判系统调试宏定义 */
#if 0
	#define JUDGE_LOG DEBUG_LOG
	#define JUDGE_ERROR DEBUG_ERROR
#else
	#define JUDGE_LOG(format, arg...) /* DEBUG OFF */
	#define JUDGE_ERROR(err) /* DEBUG OFF */
#endif

#if 0
	#define JUDGE_ARRAY DEBUG_ARRAY
#else
	#define JUDGE_ARRAY(name, data, len)
#endif

//函数声明
void Find_All_A5(u8 *get_data, u16 data_len, u8 *r_position, u8 *r_a5_length);
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq);
u8 Check_Package_Crc16(u8 *get_data, u16 data_len);
uint16_t Analysis_Cmd_Id(u8 *get_data);
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len);

static uint8_t Analysis_Power_Heat_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Shoot_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Game_Robot_Status(u8 *data_package, uint16_t data_len);

//裁判系统数据定义
static Judge_data_t judge_data;

const Judge_data_t* Get_Judge_Data(void)
{
	return &judge_data;
}

/*
  函数名：Analysis_Judge_System
  描述  ：解析裁判系统数据
  参数  ：get_data需要解析的帧头数据，data_len数据长度
  返回值：0--解析失败 1--解析成功
*/
uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len)
{
	u8 a5_position[8]; //0xA5的位置
	u8 a5_number = 0;  //0xA5的个数（数据包个数）
	u16 data_length[8];  //每个data数据包的长度
	
	//寻找帧头
	Find_All_A5(get_data, data_len, a5_position, &a5_number);
	
	//解析所有数据包
	for(u8 i=0; i<a5_number; i++)
	{
		//解析帧头数据
		if( Analysis_Frame_Header(&get_data[ (a5_position[i]) ], &data_length[i], NULL) == 0)
		{
			JUDGE_LOG("Analysis No.%d frame header error.", i);
			continue;
		}
		
		//整包CRC16校验
		if( Check_Package_Crc16(&get_data[ (a5_position[i]) ], (data_length[i]+9)) == 0 )
		{
			JUDGE_LOG("CRC16 check No.%d fail.", i);
			continue;
		}
		
		//解析该数据包
		if( Analysis_Data(&get_data[ (a5_position[i]) ], data_length[i]) == 0)
		{
			JUDGE_LOG("Analysis No.%d data fail.", i);
			continue;
		}
		
//		DEBUG_PRINT("x%d len:%d p:%d id:%d\r\n", i, data_length[i], a5_position[i], Analysis_Cmd_Id( &get_data[ (a5_position[i]) ] ) );
		
		
	}
	
	return 1;
}


/*
  函数名：Find_All_A5
  描述  ：找到所有帧头的头
  参数  ：get_data需要解析的帧头数据，data_len数据长度，r_position 0xA5位置数组，r_a5_length返回数组长度的长度
  返回值：无
*/
void Find_All_A5(u8 *get_data, u16 data_len, u8 *r_position, u8 *r_a5_length)
{
	*r_a5_length = 0;
	
	for(u16 i=0; i<data_len; i++)
	{
		if(get_data[i] == 0xA5)
		{
			r_position[*r_a5_length] = i;
			
			(*r_a5_length) ++;
		}
	}
}

/*
  函数名：Analysis_Frame_Header
  描述  ：解析帧头数据
  参数  ：get_data需要解析的数据包，r_data_length返回数据帧中 data 的长度，r_seq返回seq值
  返回值：0--解析失败 1--解析成功
*/
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq)
{
	JUDGE_ARRAY("head", get_data, 5);
	
	//判断帧头的头是否为0xA5
	if(get_data[0] != 0xA5)
	{
		JUDGE_ERROR(501);
		return 0;
	}
	
	//帧头CRC8校验
	if( Verify_CRC8_Check_Sum(get_data, 5) == 0)
	{
		JUDGE_ERROR(502);
		return 0;
	}
	
	//解析data的长度
	if(r_data_length != NULL)
	{
		*r_data_length = get_data[1] | (get_data[2] << 8);
	}
	
	//解析seq
	if(r_seq != NULL)
	{
		*r_seq = get_data[3];
	}
	
	return 1;
}

/*
  函数名：Check_Package_Crc16
  描述  ：一个数据包进行CRC16校验
  参数  ：get_data需要解析的数据包， data_len包长度
  返回值：0--解析失败 1--解析成功
*/
u8 Check_Package_Crc16(u8 *get_data, u16 data_len)
{
	return Verify_CRC16_Check_Sum(get_data, data_len);  
}

/*
  函数名：Analysis_Cmd_Id
  描述  ：解析命令ID数据
  参数  ：get_data需要解析的数据包
  返回值：cmd_id
*/
uint16_t Analysis_Cmd_Id(u8 *get_data)
{
	return get_data[5] + (get_data[6]<<8);
}

/*
  函数名：Analysis_Data
  描述  ：解析data数据
  参数  ：get_data需要解析的数据包, data_len data的长度
  返回值：0--解析失败 1--解析成功
*/
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len)
{
	uint16_t cmd_id = get_data[5] | (get_data[6]<<8);
	
	switch (cmd_id)
	{
		//实时热量数据
		case Power_Heat_Data:
			return Analysis_Power_Heat_Data(&get_data[7], data_len);
			//break;
		
		//实时功率热量数据
		case Shoot_Data:
			return Analysis_Shoot_Data(&get_data[7], data_len);
			//break;
		
		//实时比赛机器人状态
		case Game_Robot_Status:
			return Analysis_Game_Robot_Status(&get_data[7], data_len);
			//break;
		
	}
	
	return 0x0A;
}

//解析实时功率热量数据
static uint8_t Analysis_Power_Heat_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 16)
	{
		JUDGE_ERROR(503);
		return 0;
	}
	
	judge_data.power_heat_data.chassis_power = Hex4_To_Float1(&data_package[4]);
	judge_data.power_heat_data.chassis_power_buffer = U8_Array_To_U16(&data_package[8]);
	judge_data.power_heat_data.shooter_id1_17mm_cooling_heat = U8_Array_To_U16(&data_package[10]);
	
	//DEBUG_SHOWDATA2("chassis_power", judge_data.power_heat_data.chassis_power);

	return 1;
}

//解析实时射击信息
static uint8_t Analysis_Shoot_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 7)
	{
		JUDGE_ERROR(504);
		return 0;
	}
	
	// judge_data.shoot_data.bullet_type = data_package[0];
	// judge_data.shoot_data.shooter_id = data_package[1];
	// judge_data.shoot_data.bullet_freq = data_package[2];
	// judge_data.shoot_data.bullet_speed = Hex4_To_Float1(&data_package[3]);
	
	memcpy(&judge_data.shoot_data, data_package, 7);
	//Shooter_Friction_Speed_Limit();
	//DEBUG_SHOWDATA2("bullet_speed", judge_data.shoot_data.bullet_speed);
	
	return 1;
}

//解析比赛机器人状态
static uint8_t Analysis_Game_Robot_Status(u8 *data_package, uint16_t data_len)
{
	if(data_len != 27)
	{
		JUDGE_ERROR(505);
		return 0;
	}
	
	judge_data.game_robot_status.robot_id = data_package[0];

	//机器人 1 号 17mm 枪口每秒冷却值
	judge_data.game_robot_status.shooter_id1_17mm_cooling_rate = U8_Array_To_U16(&data_package[6]);

	//机器人 1 号 17mm 枪口热量上限
	judge_data.game_robot_status.shooter_id1_17mm_cooling_limit = U8_Array_To_U16(&data_package[8]);

	//机器人 1 号 17mm 枪口上限速度 单位 m/s
	judge_data.game_robot_status.shooter_id1_17mm_speed_limit = U8_Array_To_U16(&data_package[10]);

	//底盘功率上限
	judge_data.game_robot_status.chassis_power_limit = U8_Array_To_U16(&data_package[24]);
	
	judge_data.game_robot_status.mains_power_shooter_output = (data_package[26] & 0x04) >> 2;

	//模拟发射机构断电
	if(judge_data.game_robot_status.mains_power_shooter_output)
	{
		POWER1_CTRL_ON;
		POWER2_CTRL_ON;
	}
	else
	{
		POWER1_CTRL_OFF;  
		POWER2_CTRL_OFF;
		//Fric_Reset(); //重置摩擦轮
	}

	//DEBUG_SHOWDATA1("sout", judge_data.game_robot_status.mains_power_shooter_output);
	//DEBUG_SHOWDATA1("gggglllll", judge_data.game_robot_status.chassis_power_limit);
	
	return 1;
}

//判断1号17mm发射机构是否超热量
u8 Is_Id1_17mm_Excess_Heat(const Judge_data_t* judge_data)
{
	//DEBUG_PRINT("sx:%d, dqrl:%d \r\n", judge_data->game_robot_status.shooter_id1_17mm_cooling_limit, judge_data->power_heat_data.shooter_id1_17mm_cooling_heat);
	if(judge_data->game_robot_status.shooter_id1_17mm_cooling_limit == 65535)
	{
		return 0;
	}
	if(judge_data->game_robot_status.shooter_id1_17mm_cooling_limit <= (judge_data->power_heat_data.shooter_id1_17mm_cooling_heat + 15 ))
	{
		return 1;
	}
	return 0;
}

