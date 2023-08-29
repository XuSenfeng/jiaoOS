/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����Ӧ��bsp��ɨ��ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./key/bsp_key.h"  
extern struct Event_Flog EventFlog;
extern struct SHEET * Mouse_sht, * Windoes_sht;
extern Mouse_Message_Def Mouse_def;

/**
  * @brief  ���ð����жϺ��������ȼ�
  * @param  ��
  * @retval ��
  */
static void EXTI_NVIC_Config(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  ���ð����õ���I/O��,��ʼ����ص��ж�
  * @param  ��
  * @retval ��
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_NVIC_Config();
	/*���������˿ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
	
	//ѡ�񰴼�������
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
	// ���ð���������Ϊ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//ʹ�ýṹ���ʼ������
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
	
	//ѡ�񰴼�������
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
	//���ð���������Ϊ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//ʹ�ýṹ���ʼ������
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
	
	
	
	//�����Ÿ��õ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//��ʼ��KEY1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;	//����ΪEXTI0
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;	//ʹ��
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//����Ϊ�ж�
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�����ش���
	EXTI_Init(&EXTI_InitStruct);
	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	EXTI_InitStruct.EXTI_Line = EXTI_Line13;	//����ΪEXTI13
	EXTI_Init(&EXTI_InitStruct);
	

}
/**
  * @brief  ����������, �������������Լ��Ĵ���
  * @param  ��
  * @retval ��
  */
void Key_TouchEventHandler(int i)
{
	
		if(i==KEY1_DOWN)
		{
			//KAY1����,���������ô���
			
			printf("Key1 down\n");
		}
		if(i==KEY1_UP)
		{
			//KAY1̧��,���������ô���

			printf("Key1 up\n");
		}
		if(i==KEY2_DOWN)
		{	
			//KAY2����,���������ô���
			sheet_slide(Windoes_sht, Mouse_def.x, Mouse_def.y);
			printf("Key2 down\n");
		}
		if(i==KEY2_UP)
		{
			//KAY2̧��,���������ô���

			printf("Key2 up\n");
		}				
	
	
}

/*********************************************END OF FILE**********************/
