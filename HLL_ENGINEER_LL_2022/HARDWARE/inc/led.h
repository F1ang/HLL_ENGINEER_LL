#ifndef __LED_H
#define __LED_H

#include "main.h"
#include "gpio.h"

#define LED_RED PEout(11)
#define LED_GREEN PFout(14)
#define LED_FLOW(led_num) PGout(led_num)

#define LED_RED_ON     		LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_11)
#define LED_RED_OFF    		LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_11)
#define LED_RED_TOGGLE 		LL_GPIO_TogglePin(GPIOE,LL_GPIO_PIN_11)

#define LED_GREEN_ON    	LL_GPIO_ResetOutputPin(GPIOF, LL_GPIO_PIN_14)
#define LED_GREEN_OFF    	LL_GPIO_SetOutputPin(GPIOF, LL_GPIO_PIN_14)
#define LED_GREEN_TOGGLE 	LL_GPIO_TogglePin(GPIOF,LL_GPIO_PIN_14)

#define LED_FLOW_ON(led_num) 			LL_GPIO_ResetOutputPin(GPIOG, (LL_GPIO_PIN_8 >> led_num))
#define LED_FLOW_OFF(led_num) 		LL_GPIO_SetOutputPin(GPIOG, (LL_GPIO_PIN_8 >> led_num))
#define LED_FLOW_TOGGLE(led_num)  LL_GPIO_TogglePin(GPIOG,(LL_GPIO_PIN_8 >> led_num))

void Led_Init(void);
void Led_All_Flash(void);
void Led_Flow_On(void);
void Led_Flow_Off(void);

#endif
