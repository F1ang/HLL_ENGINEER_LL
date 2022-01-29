#include "power_output.h"

#include "stm32f4xx.h"

//24V电源输出配置
void Power_Ctrl_Init(void)
{
	POWER1_CTRL_ON;
	POWER2_CTRL_ON;
	POWER3_CTRL_ON;
	POWER4_CTRL_ON;
}

