/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./key/bsp_key.h"  
extern struct Event_Flog EventFlog;
extern struct SHEET * Mouse_sht, * Windoes_sht;
extern Mouse_Message_Def Mouse_def;

/**
  * @brief  配置按键中断函数的优先级
  * @param  无
  * @retval 无
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
  * @brief  配置按键用到的I/O口,初始化相关的中断
  * @param  无
  * @retval 无
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_NVIC_Config();
	/*开启按键端口的时钟*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
	
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
	// 设置按键的引脚为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//使用结构体初始化按键
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
	
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
	//设置按键的引脚为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//使用结构体初始化按键
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
	
	
	
	//打开引脚复用的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//初始化KEY1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;	//设置为EXTI0
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;	//使能
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//设置为中断
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上升沿触发
	EXTI_Init(&EXTI_InitStruct);
	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	EXTI_InitStruct.EXTI_Line = EXTI_Line13;	//设置为EXTI13
	EXTI_Init(&EXTI_InitStruct);
	

}
/**
  * @brief  按键处理函数, 可以在这里添自己的处理
  * @param  无
  * @retval 无
  */
void Key_TouchEventHandler(int i)
{
	
		if(i==KEY1_DOWN)
		{
			//KAY1按下,在这里设置处理
			
			printf("Key1 down\n");
		}
		if(i==KEY1_UP)
		{
			//KAY1抬起,在这里设置处理

			printf("Key1 up\n");
		}
		if(i==KEY2_DOWN)
		{	
			//KAY2按下,在这里设置处理
			sheet_slide(Windoes_sht, Mouse_def.x, Mouse_def.y);
			printf("Key2 down\n");
		}
		if(i==KEY2_UP)
		{
			//KAY2抬起,在这里设置处理

			printf("Key2 up\n");
		}				
	
	
}

/*********************************************END OF FILE**********************/
