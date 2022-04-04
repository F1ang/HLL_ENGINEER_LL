#ifndef BSP_TIM_H
#define BSP_TIM_H

#include "tim.h" 
#include  "main.h"     

#include "encoder.h"
#include "motor.h"
#ifndef __TIMER8_H
#define __TIMER8_H
//tim8
#define PI5_PWM_OUT(ccr)  TIM8->CCR1 = ccr;
#define PI6_PWM_OUT(ccr)  TIM8->CCR2 = ccr;
#define PI7_PWM_OUT(ccr)  TIM8->CCR3 = ccr;
#define PI2_PWM_OUT(ccr)  TIM8->CCR4 = ccr;
//tim3
#define PB0_PWM_OUT(ccr)  TIM3->CCR3 = ccr;
#define PB1_PWM_OUT(ccr)  TIM3->CCR4 = ccr;
//tim9
#define PE5_PWM_OUT(ccr)  TIM9->CCR1 = ccr;
#define PE6_PWM_OUT(ccr)  TIM9->CCR2 = ccr;
extern void tim8_base_init(void);
extern void tim3_base_init(void);
extern void tim9_base_init(void);
extern void timx_base_init(void);
extern void tim7_base_init(void);
extern void tim12_base_init(void);
#endif




#endif

