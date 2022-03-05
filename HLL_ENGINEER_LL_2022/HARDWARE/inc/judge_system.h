#ifndef __JUDGE_SYSTEM_H
#define __JUDGE_SYSTEM_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include "usart_hll.h"
#include "crc_check.h"
#include "sys.h"

// uart ͨ�����ã������� 115200, ����λ 8, ֹͣλ 1������λ�ޣ��������ޡ�
// --------------------------------------------------------------------------------
//| FrameHeader(5-Byte) | CmdID(2-Byte) | Data(n-Byte) | FrameTail(2-Byte, CRC16) |
// --------------------------------------------------------------------------------
//crc8 generator polynomial:G(x)=x8+x5+x4+1


/* ����ID */
typedef enum
{
	Buff_Data=0x0204,          //���������棺0x0204������Ƶ�ʣ�1Hz
	Bullet_Remain_Data=0x0208, //�ӵ�ʣ�෢������0x0208������Ƶ�ʣ�10Hz ���ڷ���
	RFID_Data=0x0209           //������ RFID ״̬��0x0209������Ƶ�ʣ�1Hz
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

	
}Judge_data_t;

extern const Judge_data_t* Get_Judge_Data(void);

extern uint8_t Analysis_Judge_System(u8 *get_data, u16 data_len);

extern u8 Is_Id1_17mm_Excess_Heat(const Judge_data_t* judge_data);

#endif
