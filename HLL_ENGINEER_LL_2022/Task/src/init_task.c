#include "init_task.h"



void All_Init(void)
{	
	/*硬件初始化*/
	base_init();
	
	/* 板载24V输出控制初始化 */
	Power_Ctrl_Init();
	INFO_PRINT("\r\nAll peripheral init.\r\n");
}
