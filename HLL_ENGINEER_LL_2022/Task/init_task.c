#include "init_task.h"



void All_Init(void)
{
	base_init();
	Power_Ctrl_Init();
	INFO_LOG("\r\nAll tasks creat.\r\n");
}
