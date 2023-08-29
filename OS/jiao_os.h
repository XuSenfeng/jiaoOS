/****************************************
*这个文件用于进行系统的相关操作以及初始化	*
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
#include "./task/jiao_task.h"
#include "jiao_FIFO.h"
#include "./time/jiao_time.h"
/*用于记录各种中断中产生的事件*/
/*
规划
+ 1-2		触摸屏
+ 3-4		Key1
+ 5-6		Key2
+ 7-		定时器
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
struct Event_Flog{
	//按键是否按下
	//uint8_t Key1_num;
	//uint8_t Key2_num;
	//触摸屏是否按下
	//uint8_t Touch_num;
	struct FIFO8 System_Flags;
	uint8_t system_flags_buf[16];

};

void JIAO_OS_Init(void);

#endif
