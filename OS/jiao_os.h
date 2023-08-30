/****************************************
*����ļ����ڽ���ϵͳ����ز����Լ���ʼ��	*
*										*
*										*
*										*
*										*
*****************************************/

#ifndef __JIAO_OS_H__
#define __JIAO_OS_H__



#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_xpt2046_lcd.h"
#include "jiao_dasktop.h"
#include "bsp_base_tim.h"
#include "jiao_sheet.h"
#include "./Key/bsp_key.h"
//#include "./task/jiao_task.h"
#include "jiao_FIFO.h"
#include "./time/jiao_time.h"
/*���ڼ�¼�����ж��в������¼�*/
/*
�滮
+ 1-2		������
+ 3-4		Key1
+ 5-6		Key2
+ 7-		��ʱ��
....
*/

#define TOUCH_DOWN			1
#define TOUCH_UP			2
#define KEY1_DOWN			3
#define KEY1_UP				4
#define KEY2_DOWN			5
#define KEY2_UP				6
#define TIM0_FLAG			7
#define TIM1_FLAG			8
#define TIM2_FLAG			9
#define TIM3_FLAG			10
#define TIM4_FLAG			11


void JIAO_OS_Init(void);

#endif
