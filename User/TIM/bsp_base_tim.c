#include "bsp_base_tim.h"
/**
  * @brief  ��ʼ����ʱ��6, �������д������ж�ʵ��
  * @param  ��
  * @retval None
  */
static void TOUCH_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseStructure;
	//��ʼ������ʱ�ӵ�ʱ���ź�
	TOUCH_TIM_APBxClock_FUN(TOUCH_TIM_CLK, ENABLE);
	TOUCH_TIM_APBxClock_FUN(TIME_TIM_CLK, ENABLE);
	
	TIM_BaseStructure.TIM_Period = TOUCH_TIM_Period;
	TIM_BaseStructure.TIM_Prescaler = TOUCH_TIM_Prescaler;
	/****������ʱ��û��****/
	TIM_BaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStructure.TIM_RepetitionCounter = 0;
	/********************/
	
	TIM_TimeBaseInit(TOUCH_TIM, &TIM_BaseStructure);
	//���ü�ʱʱ��
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
  * @brief  ���ô����������ȼ�
  * @param  ��
  * @retval None
  */
static void TOUCH_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TOUCH_TIM_IRQ ;	
		// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	  // ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	
	//��ʱʱ������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		
	// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIME_TIM_IRQ ;	
	// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	 // ������ռ���ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  ��ʼ����ʱ��
  * @param  ��
  * @retval None
  */
void TOUCH_TIM_Init(void)
{
	TOUCH_TIM_Config();
	TOUCH_TIM_NVIC_Config();
}
