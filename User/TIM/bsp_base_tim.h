#ifndef __BSP_BASE_TIM_H__
#define __BSP_BASE_TIM_H__


#include "stm32f10x.h"


#define            TOUCH_TIM                   TIM6
#define            TOUCH_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            TOUCH_TIM_CLK               RCC_APB1Periph_TIM6
#define            TOUCH_TIM_Period            1000-1
#define            TOUCH_TIM_Prescaler         720-1
#define            TOUCH_TIM_IRQ               TIM6_IRQn
#define            TOUCH_TIM_IRQHandler        TIM6_IRQHandler

#define            TIME_TIM                   TIM7
#define            TIME_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            TIME_TIM_CLK               RCC_APB1Periph_TIM7
#define            TIME_TIM_Period            1000-1
#define            TIME_TIM_Prescaler         144-1
#define            TIME_TIM_IRQ               TIM7_IRQn
#define            TIME_TIM_IRQHandler        TIM7_IRQHandler

void TOUCH_TIM_Init(void);

#endif
