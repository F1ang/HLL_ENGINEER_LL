#ifndef __PUMP_H
#define __PUMP_H

#include "main.h"
#include "gpio.h"

#define PUMP_ON     LL_GPIO_ResetOutputPin(GPIOF, LL_GPIO_PIN_10)			//RESET
#define PUMP_OFF    LL_GPIO_SetOutputPin(GPIOF, LL_GPIO_PIN_10)

#define REVIVE_ON     LL_GPIO_ResetOutputPin(GPIOI, LL_GPIO_PIN_9)			//SAVE
#define REVIVE_OFF    LL_GPIO_SetOutputPin(GPIOI, LL_GPIO_PIN_9)

void Pump_Init(void);


#endif
