#include "bsp_led.h"
/**
  * @brief  ��ʼ��LED��GPIO
  * @param  ��
  * @retval None
  */
void LED_GPIO_Config()
{
	//����ʱ��
	RCC_APB2PeriphClockCmd(LED_G_GPIO_CLOCK, ENABLE);
	//��ʼ������
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = LED_G_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_B_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED_R_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
}

