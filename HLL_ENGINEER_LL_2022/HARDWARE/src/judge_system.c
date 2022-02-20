#include "judge_system.h"
//#include "crc_check.h"
#include "math2.h"
#include "remoter_task.h"
//#include "shooter_task.h"
#include "power_output.h"

/* ����ϵͳ���Ժ궨�� */
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

//��������
void Find_All_A5(u8 *get_data, u16 data_len, u8 *r_position, u8 *r_a5_length);
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq);
u8 Check_Package_Crc16(u8 *get_data, u16 data_len);
uint16_t Analysis_Cmd_Id(u8 *get_data);
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len);

static uint8_t Analysis_Power_Heat_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Shoot_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Game_Robot_Status(u8 *data_package, uint16_t data_len);

//����ϵͳ���ݶ���
static Judge_data_t judge_data;

const Judge_data_t* Get_Judge_Data(void)
{
	return &judge_data;
}

/*
  ��������Analysis_Judge_System
  ����  ����������ϵͳ����
  ����  ��get_data��Ҫ������֡ͷ���ݣ�data_len���ݳ���
  ����ֵ��0--����ʧ�� 1--�����ɹ�
*/
uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len)
{
	u8 a5_position[8]; //0xA5��λ��
	u8 a5_number = 0;  //0xA5�ĸ��������ݰ�������
	u16 data_length[8];  //ÿ��data���ݰ��ĳ���
	
	//Ѱ��֡ͷ
	Find_All_A5(get_data, data_len, a5_position, &a5_number);
	
	//�����������ݰ�
	for(u8 i=0; i<a5_number; i++)
	{
		//����֡ͷ����
		if( Analysis_Frame_Header(&get_data[ (a5_position[i]) ], &data_length[i], NULL) == 0)
		{
			JUDGE_LOG("Analysis No.%d frame header error.", i);
			continue;
		}
		
		//����CRC16У��
		if( Check_Package_Crc16(&get_data[ (a5_position[i]) ], (data_length[i]+9)) == 0 )
		{
			JUDGE_LOG("CRC16 check No.%d fail.", i);
			continue;
		}
		
		//���������ݰ�
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
  ��������Find_All_A5
  ����  ���ҵ�����֡ͷ��ͷ
  ����  ��get_data��Ҫ������֡ͷ���ݣ�data_len���ݳ��ȣ�r_position 0xA5λ�����飬r_a5_length�������鳤�ȵĳ���
  ����ֵ����
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
  ��������Analysis_Frame_Header
  ����  ������֡ͷ����
  ����  ��get_data��Ҫ���������ݰ���r_data_length��������֡�� data �ĳ��ȣ�r_seq����seqֵ
  ����ֵ��0--����ʧ�� 1--�����ɹ�
*/
uint8_t Analysis_Frame_Header(u8 *get_data, u16 *r_data_length, u8 *r_seq)
{
	JUDGE_ARRAY("head", get_data, 5);
	
	//�ж�֡ͷ��ͷ�Ƿ�Ϊ0xA5
	if(get_data[0] != 0xA5)
	{
		JUDGE_ERROR(501);
		return 0;
	}
	
	//֡ͷCRC8У��
	if( Verify_CRC8_Check_Sum(get_data, 5) == 0)
	{
		JUDGE_ERROR(502);
		return 0;
	}
	
	//����data�ĳ���
	if(r_data_length != NULL)
	{
		*r_data_length = get_data[1] | (get_data[2] << 8);
	}
	
	//����seq
	if(r_seq != NULL)
	{
		*r_seq = get_data[3];
	}
	
	return 1;
}

/*
  ��������Check_Package_Crc16
  ����  ��һ�����ݰ�����CRC16У��
  ����  ��get_data��Ҫ���������ݰ��� data_len������
  ����ֵ��0--����ʧ�� 1--�����ɹ�
*/
u8 Check_Package_Crc16(u8 *get_data, u16 data_len)
{
	return Verify_CRC16_Check_Sum(get_data, data_len);  
}

/*
  ��������Analysis_Cmd_Id
  ����  ����������ID����
  ����  ��get_data��Ҫ���������ݰ�
  ����ֵ��cmd_id
*/
uint16_t Analysis_Cmd_Id(u8 *get_data)
{
	return get_data[5] + (get_data[6]<<8);
}

/*
  ��������Analysis_Data
  ����  ������data����
  ����  ��get_data��Ҫ���������ݰ�, data_len data�ĳ���
  ����ֵ��0--����ʧ�� 1--�����ɹ�
*/
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len)
{
	uint16_t cmd_id = get_data[5] | (get_data[6]<<8);
	
	switch (cmd_id)
	{
		//ʵʱ��������
		case Power_Heat_Data:
			return Analysis_Power_Heat_Data(&get_data[7], data_len);
			//break;
		
		//ʵʱ������������
		case Shoot_Data:
			return Analysis_Shoot_Data(&get_data[7], data_len);
			//break;
		
		//ʵʱ����������״̬
		case Game_Robot_Status:
			return Analysis_Game_Robot_Status(&get_data[7], data_len);
			//break;
		
	}
	
	return 0x0A;
}

//����ʵʱ������������
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

//����ʵʱ�����Ϣ
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

//��������������״̬
static uint8_t Analysis_Game_Robot_Status(u8 *data_package, uint16_t data_len)
{
	if(data_len != 27)
	{
		JUDGE_ERROR(505);
		return 0;
	}
	
	judge_data.game_robot_status.robot_id = data_package[0];

	//������ 1 �� 17mm ǹ��ÿ����ȴֵ
	judge_data.game_robot_status.shooter_id1_17mm_cooling_rate = U8_Array_To_U16(&data_package[6]);

	//������ 1 �� 17mm ǹ����������
	judge_data.game_robot_status.shooter_id1_17mm_cooling_limit = U8_Array_To_U16(&data_package[8]);

	//������ 1 �� 17mm ǹ�������ٶ� ��λ m/s
	judge_data.game_robot_status.shooter_id1_17mm_speed_limit = U8_Array_To_U16(&data_package[10]);

	//���̹�������
	judge_data.game_robot_status.chassis_power_limit = U8_Array_To_U16(&data_package[24]);
	
	judge_data.game_robot_status.mains_power_shooter_output = (data_package[26] & 0x04) >> 2;

	//ģ�ⷢ������ϵ�
	if(judge_data.game_robot_status.mains_power_shooter_output)
	{
		POWER1_CTRL_ON;
		POWER2_CTRL_ON;
	}
	else
	{
		POWER1_CTRL_OFF;  
		POWER2_CTRL_OFF;
		//Fric_Reset(); //����Ħ����
	}

	//DEBUG_SHOWDATA1("sout", judge_data.game_robot_status.mains_power_shooter_output);
	//DEBUG_SHOWDATA1("gggglllll", judge_data.game_robot_status.chassis_power_limit);
	
	return 1;
}

//�ж�1��17mm��������Ƿ�����
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
