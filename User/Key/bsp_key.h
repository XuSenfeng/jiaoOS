#ifndef __KEY_H
#define	__KEY_H


#include "stm32f10x.h"
#include "jiao_os.h"
//  引脚定义
#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    KEY1_GPIO_PORT    GPIOA			   
#define    KEY1_GPIO_PIN		 GPIO_Pin_0

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOC
#define    KEY2_GPIO_PORT    GPIOC		   
#define    KEY2_GPIO_PIN		  GPIO_Pin_13


 /** 按键按下标置宏
	*  按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
	*  若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
	*/
#define KEY_ON	1
#define KEY_OFF	0
#define             KEY1_Read()                          GPIO_ReadInputDataBit ( KEY1_GPIO_PORT, KEY1_GPIO_PIN )
#define             KEY2_Read()                          GPIO_ReadInputDataBit ( KEY2_GPIO_PORT, KEY2_GPIO_PIN )

void Key_GPIO_Config(void);
void Key_TouchEventHandler(int i);

#endif /* __KEY_H */

