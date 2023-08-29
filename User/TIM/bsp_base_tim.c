#include "bsp_base_tim.h"
/**
  * @brief  初始化定时器6, 用来进行触摸屏中断实现
  * @param  无
  * @retval None
  */
static void TOUCH_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseStructure;
	//初始化两个时钟的时钟信号
	TOUCH_TIM_APBxClock_FUN(TOUCH_TIM_CLK, ENABLE);
	TOUCH_TIM_APBxClock_FUN(TIME_TIM_CLK, ENABLE);
	
	TIM_BaseStructure.TIM_Period = TOUCH_TIM_Period;
	TIM_BaseStructure.TIM_Prescaler = TOUCH_TIM_Prescaler;
	/****基础定时器没有****/
	TIM_BaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStructure.TIM_RepetitionCounter = 0;
	/********************/
	
	TIM_TimeBaseInit(TOUCH_TIM, &TIM_BaseStructure);
	//设置计时时钟
	TIM_BaseStructure.TIM_Period = TIME_TIM_Period;
	TIM_BaseStructure.TIM_Prescaler = TIME_TIM_Prescaler;
	TIM_TimeBaseInit(TIME_TIM, &TIM_BaseStructure);

	TIM_ClearFlag(TOUCH_TIM, TIM_FLAG_Update);	
	TIM_ITConfig(TOUCH_TIM, TIM_FLAG_Update, ENABLE);	
	
	TIM_ClearFlag(TIME_TIM, TIM_FLAG_Update);	
	TIM_ITConfig(TIME_TIM, TIM_FLAG_Update, ENABLE);	
	TIM_Cmd(TIME_TIM, ENABLE);
}
/**
  * @brief  设置触摸屏的优先级
  * @param  无
  * @retval None
  */
static void TOUCH_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TOUCH_TIM_IRQ ;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	  // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	
	//计时时钟设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		
	// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIME_TIM_IRQ ;	
	// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	 // 设置抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  初始化定时器
  * @param  无
  * @retval None
  */
void TOUCH_TIM_Init(void)
{
	TOUCH_TIM_Config();
	TOUCH_TIM_NVIC_Config();
}
