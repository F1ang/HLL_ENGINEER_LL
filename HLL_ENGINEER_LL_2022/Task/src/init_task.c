#include "init_task.h"



void All_Init(void)
{	
	/*Ӳ����ʼ��*/
	base_init();
	
	/* ����24V������Ƴ�ʼ�� */
	Power_Ctrl_Init();
	INFO_PRINT("\r\nAll peripheral init.\r\n");
}
