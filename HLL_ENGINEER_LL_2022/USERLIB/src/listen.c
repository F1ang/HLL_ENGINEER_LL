#include "listen.h"

#include "stdio.h"

void Module_Status_Init(Module_status_t* self, uint8_t cnt_num, void On_Hook(void), void Off_Hook(void))
{
	self->count = 0;
	self->reload_cnt = cnt_num;
	
	self->time_out_flag = 1;
	self->old_time_out_flag = 1;
	
	self->Module_Online_Hook = On_Hook;
	self->Module_Offline_Hook = Off_Hook;
}

void Module_Status_Listen(Module_status_t* self)
{
	
	self->old_time_out_flag = self->time_out_flag;
	
	if(self->count > 0)
	{
		self->count--;
		self->time_out_flag=0;
		if(self->Module_Online_Hook!=NULL && self->old_time_out_flag == 1)
		{
			self->Module_Online_Hook();
		}
	}
	else if(self->count == 0)
	{
		self->time_out_flag=1;
		if(self->Module_Offline_Hook!=NULL && self->old_time_out_flag == 0)
		{
			self->Module_Offline_Hook();
		}
	}
	
}

void Module_Status_Reload(Module_status_t* self)
{
	self->count = self->reload_cnt;
	//self->time_out_flag=0;
}
