#ifndef __LISTEN_H
#define __LISTEN_H

#include "stm32f4xx.h"

typedef struct
{
	uint16_t count;
	uint16_t reload_cnt;

	uint8_t time_out_flag;
	uint8_t old_time_out_flag;
	
	void (*Module_Online_Hook)(void);
	void (*Module_Offline_Hook)(void);
}Module_status_t;

void Module_Status_Init(Module_status_t* self, uint8_t cnt_num, void On_Hook(void), void Off_Hook(void));
void Module_Status_Listen(Module_status_t* self);
void Module_Status_Reload(Module_status_t* self);

#endif
