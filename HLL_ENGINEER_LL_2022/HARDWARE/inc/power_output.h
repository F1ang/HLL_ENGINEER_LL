#ifndef __POWER_OUTPUT_H
#define __POWER_OUTPUT_H
#include "main.h"
#include "gpio.h"
#include "usart.h"
#define POWER1_CTRL_ON LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_2)
#define POWER2_CTRL_ON LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_3)
#define POWER3_CTRL_ON LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_4)
#define POWER4_CTRL_ON LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_5)

#define POWER1_CTRL_OFF LL_GPIO_ResetOutputPin(GPIOH, GPIO_Pin_2)
#define POWER2_CTRL_OFF LL_GPIO_ResetOutputPin(GPIOH, GPIO_Pin_3)
#define POWER3_CTRL_OFF LL_GPIO_ResetOutputPin(GPIOH, GPIO_Pin_4)
#define POWER4_CTRL_OFF LL_GPIO_ResetOutputPin(GPIOH, GPIO_Pin_5)

void Power_Ctrl_Init(void);

#endif
