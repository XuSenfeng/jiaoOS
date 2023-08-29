#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"

#define LED_R_GPIO_PIN					GPIO_Pin_5
#define LED_B_GPIO_PIN					GPIO_Pin_1
#define LED_G_GPIO_PIN					GPIO_Pin_0
#define LED_GPIO_PORT					GPIOB
#define LED_G_GPIO_CLOCK				RCC_APB2Periph_GPIOB

#define ON					0
#define OFF					1

#define LED_G(a)		if(a)GPIO_SetBits(LED_GPIO_PORT, LED_G_GPIO_PIN);\
								else GPIO_ResetBits(LED_GPIO_PORT, LED_G_GPIO_PIN);

#define LED_R(a)		if(a)GPIO_SetBits(LED_GPIO_PORT, LED_R_GPIO_PIN);\
								else GPIO_ResetBits(LED_GPIO_PORT, LED_R_GPIO_PIN);
								
#define LED_B(a)		if(a)GPIO_SetBits(LED_GPIO_PORT, LED_B_GPIO_PIN);\
								else GPIO_ResetBits(LED_GPIO_PORT, LED_B_GPIO_PIN);
								
/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		 {p->BRR=i;}	 //����͵�ƽ
#define digitalToggle(p,i) {p->ODR ^=i;} //�����ת״̬


/* �������IO�ĺ� */
#define LED1_TOGGLE		 digitalToggle(LED_GPIO_PORT,LED_R_GPIO_PIN)
#define LED1_OFF		   digitalHi(LED_GPIO_PORT,LED_R_GPIO_PIN)
#define LED1_ON			   digitalLo(LED_GPIO_PORT,LED_R_GPIO_PIN)

#define LED2_TOGGLE		 digitalToggle(LED_GPIO_PORT,LED_G_GPIO_PIN)
#define LED2_OFF		   digitalHi(LED_GPIO_PORT,LED_G_GPIO_PIN)
#define LED2_ON			   digitalLo(LED_GPIO_PORT,LED_G_GPIO_PIN)

#define LED3_TOGGLE		 digitalToggle(LED_GPIO_PORT,LED_B_GPIO_PIN)
#define LED3_OFF		   digitalHi(LED_GPIO_PORT,LED_B_GPIO_PIN)
#define LED3_ON			   digitalLo(LED_GPIO_PORT,LED_B_GPIO_PIN)

/* ������ɫ������߼��÷�ʹ��PWM�ɻ��ȫ����ɫ,��Ч������ */

//��
#define LED_RED  \
					LED1_ON;\
					LED2_OFF\
					LED3_OFF

//��
#define LED_GREEN		\
					LED1_OFF;\
					LED2_ON\
					LED3_OFF

//��
#define LED_BLUE	\
					LED1_OFF;\
					LED2_OFF\
					LED3_ON

					
//��(��+��)					
#define LED_YELLOW	\
					LED1_ON;\
					LED2_ON\
					LED3_OFF
//��(��+��)
#define LED_PURPLE	\
					LED1_ON;\
					LED2_OFF\
					LED3_ON

//��(��+��)
#define LED_CYAN \
					LED1_OFF;\
					LED2_ON\
					LED3_ON
					
//��(��+��+��)
#define LED_WHITE	\
					LED1_ON;\
					LED2_ON\
					LED3_ON
					
//��(ȫ���ر�)
#define LED_RGBOFF	\
					LED1_OFF;\
					LED2_OFF\
					LED3_OFF

						
void LED_GPIO_Config(void);



#endif
