#include "judge_system.h"
//#include "crc_check.h"
#include "math2.h"
#include "remoter_task.h"
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

static uint8_t Analysis_Buff_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Bullet_Remain_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_RFID_Data(u8 *data_package, uint16_t data_len);

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
		//机器人增益
		case Buff_Data:
			return Analysis_Buff_Data(&get_data[7], data_len);
		 // break;
		case Bullet_Remain_Data:
			return Analysis_Bullet_Remain_Data(&get_data[7], data_len);
		 // break;
		case RFID_Data:
			return Analysis_RFID_Data(&get_data[7], data_len);
		 // break;
		
	}
	
	return 0x0A;
}

//解析机器人增益
static uint8_t Analysis_Buff_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 1)
	{
		JUDGE_ERROR(503);
		return 0;
	}
	judge_data.ext_buff_t.power_rune_buff=data_package[0];
	return 1;
}

//解析子弹剩余量
static uint8_t Analysis_Bullet_Remain_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 6)
	{
		JUDGE_ERROR(503);
		return 0;
	}
	judge_data.ext_bullet_remaining_t.bullet_remaining_num_17mm=U8_Array_To_U16(&data_package[0]);
	judge_data.ext_bullet_remaining_t.bullet_remaining_num_42mm=U8_Array_To_U16(&data_package[2]);
	judge_data.ext_bullet_remaining_t.coin_remaining_num=U8_Array_To_U16(&data_package[4]);
	return 1;
}
//解析RFID
static uint8_t Analysis_RFID_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 4)
	{
		JUDGE_ERROR(503);
		return 0;
	}
	judge_data.ext_rfid_status_t.rfid_status=U8_Array_To_U32(&data_package[0]);
	return 1;
}

