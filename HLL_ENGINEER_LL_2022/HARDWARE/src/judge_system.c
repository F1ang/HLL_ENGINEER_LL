#include "judge_system.h"
#include "crc_check.h"
#include "math2.h"
#include "remoter_task.h"
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

static uint8_t Analysis_Buff_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Bullet_Remain_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_RFID_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Robot_HP_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Robot_Status_Data(u8 *data_package, uint16_t data_len);
static uint8_t Analysis_Power_Heat_Data(u8 *data_package, uint16_t data_len);
//����ϵͳ���ݶ���
static Judge_data_t judge_data;

Judge_data_t* Get_Judge_Data(void)   
{
	return &judge_data;
}

/*
  ��������Analysis_Judge_System
  ����  ����������ϵͳ����
  ����  ��get_data��Ҫ������֡ͷ���ݣ�data_len���ݳ���
  ����ֵ��0--����ʧ�� 1--�����ɹ�
  frame_header(5-byte)+cmd_id (2-byte)+data (n-byte)+frame_tail (2-byte��CRC16������У��)
*/
uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len)
{
	u8 a5_position[8]; //0xA5��λ��
	u8 a5_number = 0;  //0xA5�ĸ��������ݰ�������
	u16 data_length[8];  //ÿ�����ݰ��ĳ���
	
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
  ����  ��get_data��Ҫ���������ݰ�, data_len ���ݶεĳ���
  ����ֵ��0--����ʧ�� 1--�����ɹ�
*/
uint8_t Analysis_Data(u8 *get_data, uint16_t data_len)
{
	uint16_t cmd_id = get_data[5] | (get_data[6]<<8);
	//printf("cmd_id=%lx\r\n",(long)cmd_id);  
	switch (cmd_id)
	{
		//����������
		case Buff_Data:
			return Analysis_Buff_Data(&get_data[7], data_len);
		//�����ӵ�ʣ����	
		case Bullet_Remain_Data:
			return Analysis_Bullet_Remain_Data(&get_data[7], data_len);
		//����RFID	
		case RFID_Data:
			return Analysis_RFID_Data(&get_data[7], data_len);	
		//������Ѫ��
		case Robot_HP:
			return	Analysis_Robot_HP_Data(&get_data[7],data_len);
		//������״̬
		case Robot_Status:
			return Analysis_Robot_Status_Data(&get_data[7],data_len);
		//ʵʱ����
		case Power_Heat:
			return Analysis_Power_Heat_Data(&get_data[7],data_len);
	}
	return 0x0A;
}

//��������������
static uint8_t Analysis_Buff_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 1)
	{
		JUDGE_ERROR(501);
		return 0;
	}
	judge_data.ext_buff_t.power_rune_buff=data_package[0];
	return 1;
}

//�����ӵ�ʣ����
static uint8_t Analysis_Bullet_Remain_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 6)
	{
		JUDGE_ERROR(502);
		return 0;
	}
	judge_data.ext_bullet_remaining_t.bullet_remaining_num_17mm=U8_Array_To_U16(&data_package[0]);
	judge_data.ext_bullet_remaining_t.bullet_remaining_num_42mm=U8_Array_To_U16(&data_package[2]);
	judge_data.ext_bullet_remaining_t.coin_remaining_num=U8_Array_To_U16(&data_package[4]);
	return 1;
}
//����RFID
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
//����HP
static uint8_t Analysis_Robot_HP_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 32)
	{
		JUDGE_ERROR(504);
		return 0;
	}
	judge_data.ext_game_robot_HP_t.red_1_robot_HP=U8_Array_To_U16(&data_package[0]);
	judge_data.ext_game_robot_HP_t.red_2_robot_HP=U8_Array_To_U16(&data_package[2]);
	judge_data.ext_game_robot_HP_t.red_3_robot_HP=U8_Array_To_U16(&data_package[4]);
	judge_data.ext_game_robot_HP_t.red_4_robot_HP=U8_Array_To_U16(&data_package[6]);
	judge_data.ext_game_robot_HP_t.red_5_robot_HP=U8_Array_To_U16(&data_package[8]);
	judge_data.ext_game_robot_HP_t.red_7_robot_HP=U8_Array_To_U16(&data_package[10]);
	judge_data.ext_game_robot_HP_t.red_outpost_HP=U8_Array_To_U16(&data_package[12]);
	judge_data.ext_game_robot_HP_t.red_base_HP=U8_Array_To_U16(&data_package[14]);

	judge_data.ext_game_robot_HP_t.blue_1_robot_HP=U8_Array_To_U16(&data_package[16]);
	judge_data.ext_game_robot_HP_t.blue_2_robot_HP=U8_Array_To_U16(&data_package[18]);
	judge_data.ext_game_robot_HP_t.blue_3_robot_HP=U8_Array_To_U16(&data_package[20]);
	judge_data.ext_game_robot_HP_t.blue_4_robot_HP=U8_Array_To_U16(&data_package[22]);
	judge_data.ext_game_robot_HP_t.blue_5_robot_HP=U8_Array_To_U16(&data_package[24]);
	judge_data.ext_game_robot_HP_t.blue_7_robot_HP=U8_Array_To_U16(&data_package[26]);
	judge_data.ext_game_robot_HP_t.blue_outpost_HP=U8_Array_To_U16(&data_package[28]);
	judge_data.ext_game_robot_HP_t.blue_base_HP=U8_Array_To_U16(&data_package[30]);
	return 1;
}

//����������״̬
static uint8_t Analysis_Robot_Status_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 27)
	{
		JUDGE_ERROR(504);
		return 0;
	}
	judge_data.ext_game_robot_status_t.robot_id=data_package[0];
	judge_data.ext_game_robot_status_t.robot_level=data_package[1];
	judge_data.ext_game_robot_status_t.remain_HP=U8_Array_To_U16(&data_package[2]);
	judge_data.ext_game_robot_status_t.max_HP=U8_Array_To_U16(&data_package[4]);
	judge_data.ext_game_robot_status_t.chassis_power_limit=U8_Array_To_U16(&data_package[24]);
	//printf("remain_HP=%d\r\n",	judge_data.ext_game_robot_status_t.remain_HP);
	return 1;
}

//������������
static uint8_t Analysis_Power_Heat_Data(u8 *data_package, uint16_t data_len)
{
	if(data_len != 16)
	{
		JUDGE_ERROR(505);
		return 0;
	}
	judge_data.ext_power_heat_data_t.chassis_power=U8_Array_To_U16(&data_package[4]);
	//printf("chassis_power=%f\r\n",judge_data.ext_power_heat_data_t.chassis_power);  
	return 1;
}







