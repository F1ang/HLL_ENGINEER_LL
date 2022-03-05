#include "judge_system.h"
//#include "crc_check.h"
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
		//����������
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

//��������������
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

//�����ӵ�ʣ����
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

