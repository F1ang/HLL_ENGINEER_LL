#ifndef __PUMP_H
#define __PUMP_H

#include "main.h"
#include "gpio.h"

#define PUMP_ON     LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_10)			//RESET
#define PUMP_OFF    LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_10)
void Pump_Init(void);
#endif
