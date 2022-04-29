#include "pump.h"

//继电器->气泵->气缸->夹取实现   PF10=Q1  S1 3->1  
void Pump_Init(void)
{
	
	PUMP_OFF;   
	REVIVE_OFF;   
}

