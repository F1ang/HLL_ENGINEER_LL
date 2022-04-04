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
	//pump
	Pump_Init();
	//led
	Led_Init();
	
	
//注：关于中断的全部移到相关任务初始化
//	//1、buzzer
//	Buzzer_Init();	
//	//2、DBUS
//	usart1_base_init();    
//	//3、裁判系统
//	usart6_base_init();
//	
//	//4、初始化CAN1
//	Can1_Init();
//	//5、初始化CAN2
//	Can2_Init();
	
	
	//绿色电调pwm->servo_2,3
	tim3_base_init();
	
	//tim8-ch1~ch4 servo_1
	tim8_base_init();	
	
	//encoder:x=2,4,5
	timx_base_init();	
	
	//tim7:updata
	tim7_base_init();
	
	//servo_4,5
	tim9_base_init();
	
	/* 板载24V输出控制初始化 */
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


