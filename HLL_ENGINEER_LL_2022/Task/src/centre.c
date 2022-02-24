/*
1、集中配置LL库未配置
2、后续的RTOS任务初始化也可放在这
3、RTOS的任务中间级API也可放在这


APB1_peripheral:45M  APB1_timer:90M 
APB2_peripheral:90M  APB2_timer:180M
cortex system timer:180M

*/

static void Print_Logo_2_Com(void);
#include "centre.h"    

//集中补充配置
void base_init()
{
	//buzzer
	Buzzer_Init();
	//pump
	Pump_Init();
	//led
	Led_Init();
	//DBUS
	usart1_base_init();    
	//裁判系统
	usart6_base_init();
	
	//初始化CAN1
	Can1_Init();
	//初始化CAN2
	Can2_Init();
	
	
	//绿色电调pwm
	tim3_base_init();
	
	//tim8-ch1~ch4
	tim8_base_init();
	
	//encoder:x=2,4,5
	timx_base_init();
	
	//tim7:updata
	tim7_base_init();
	
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


