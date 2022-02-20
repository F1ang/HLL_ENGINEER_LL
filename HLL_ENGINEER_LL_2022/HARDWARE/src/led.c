/*
 * 文件名：led.c
 * 描述 ：板载LED的操作
 * 定义了函数用于初始化LED
 * 定义了函数实现LED流水灯
 */

#include "led.h"
/*
  函数名：Led_All_Flash
  描述  ：LED炫酷的闪烁
  参数  ：无
  返回值：无
*/
void Led_All_Flash(void)
{
	LED_GREEN_ON;
	LL_mDelay(100);//	delay_ms(100);
	LED_RED_ON;
	LL_mDelay(100);
	for(uint8_t i=0; i<8; i++)
	{
		LED_FLOW_ON(i);
		LL_mDelay(100);
	}
	
	for(uint8_t i=0; i<8; i++)
	{
		LED_FLOW_OFF(i);
		LL_mDelay(100);
	}
	LED_RED_OFF;
	LL_mDelay(100);
	LED_GREEN_OFF;
	LL_mDelay(100);
}

void Led_Flow_On(void)
{
	LED_GREEN_ON;
	LL_mDelay(30);
	LED_RED_ON;
	LL_mDelay(30);
	for(int i=0; i<8; i++)
	{
		LED_FLOW_ON(i);
		LL_mDelay(30);
	}
}

void Led_Flow_Off(void)
{
	for(uint8_t i=0; i<8; i++)
	{
		LED_FLOW_OFF(i);
		LL_mDelay(30);
	}
	LED_RED_OFF;
	LL_mDelay(30);
	LED_GREEN_OFF;
	LL_mDelay(30);
}
