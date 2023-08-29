#include "bsp_led.h"
/**
  * @brief  初始化LED的GPIO
  * @param  无
  * @retval None
  */
void LED_GPIO_Config()
{
	//开启时钟
	RCC_APB2PeriphClockCmd(LED_G_GPIO_CLOCK, ENABLE);
	//初始化引脚
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

