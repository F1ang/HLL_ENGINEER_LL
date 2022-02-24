#ifndef __BUZZER_H
#define __BUZZER_H

#include "main.h"
#include "gpio.h"

#include "tim_hll.h"  
#define BUZZER_ON(pwm) {TIM12->ARR=pwm;LL_TIM_OC_SetCompareCH1(TIM12,pwm/2);}
#define BUZZER_OFF() LL_TIM_OC_SetCompareCH1(TIM12,0);

void Buzzer_Init(void);


#endif
