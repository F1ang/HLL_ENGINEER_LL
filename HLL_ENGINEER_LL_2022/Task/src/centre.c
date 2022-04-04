/*
1����������LL��δ����
2��������RTOS�����ʼ��Ҳ�ɷ�����
3��RTOS�������м伶APIҲ�ɷ�����


APB1_peripheral:45M  APB1_timer:90M 
APB2_peripheral:90M  APB2_timer:180M
cortex system timer:180M

*/

static void Print_Logo_2_Com(void);
#include "centre.h"    

//���в�������
void base_init()
{
	//pump
	Pump_Init();
	//led
	Led_Init();
	
	
//ע�������жϵ�ȫ���Ƶ���������ʼ��
//	//1��buzzer
//	Buzzer_Init();	
//	//2��DBUS
//	usart1_base_init();    
//	//3������ϵͳ
//	usart6_base_init();
//	
//	//4����ʼ��CAN1
//	Can1_Init();
//	//5����ʼ��CAN2
//	Can2_Init();
	
	
	//��ɫ���pwm->servo_2,3
	tim3_base_init();
	
	//tim8-ch1~ch4 servo_1
	tim8_base_init();	
	
	//encoder:x=2,4,5
	timx_base_init();	
	
	//tim7:updata
	tim7_base_init();
	
	//servo_4,5
	tim9_base_init();
	
	/* ����24V������Ƴ�ʼ�� */
	Power_Ctrl_Init();
	INFO_PRINT("\r\nAll peripheral init.\r\n");
	//LOGO
	Print_Logo_2_Com();
	INFO_PRINT("\r\nHLL_2022_ENGINEER_LL\r\n");

}

static void Print_Logo_2_Com(void)
{
	INFO_PRINT("\r\n");
	INFO_PRINT("        *===*   *===*    *===*          *===*\r\n");
	INFO_PRINT("       /   /   /   /    /   /          /   /\r\n");
	INFO_PRINT("      /   *===*   /    /   /          /   /\r\n");
	INFO_PRINT("     /           /    /   /          /   /\r\n");
	INFO_PRINT("    /   *===*   /    /   *======*   /   *======*\r\n");
	INFO_PRINT("   /   /   /   /    /          /   /          /\r\n");
	INFO_PRINT("  *===*   *===*    *==========*   *==========*\r\n");
	INFO_PRINT("\r\n");
}


