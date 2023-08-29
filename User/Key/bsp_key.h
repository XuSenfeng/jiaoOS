#ifndef __KEY_H
#define	__KEY_H


#include "stm32f10x.h"
#include "jiao_os.h"
//  ���Ŷ���
#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    KEY1_GPIO_PORT    GPIOA			   
#define    KEY1_GPIO_PIN		 GPIO_Pin_0

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOC
#define    KEY2_GPIO_PORT    GPIOC		   
#define    KEY2_GPIO_PIN		  GPIO_Pin_13


 /** �������±��ú�
	*  ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	*  ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	1
#define KEY_OFF	0
#define             KEY1_Read()                          GPIO_ReadInputDataBit ( KEY1_GPIO_PORT, KEY1_GPIO_PIN )
#define             KEY2_Read()                          GPIO_ReadInputDataBit ( KEY2_GPIO_PORT, KEY2_GPIO_PIN )

void Key_GPIO_Config(void);
void Key_TouchEventHandler(int i);

#endif /* __KEY_H */

