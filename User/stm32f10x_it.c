/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_usart.h"
#include "stm32f10x.h"
#include "bsp_base_tim.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "jiao_os.h"
#include "./task/jiao_task.h"
#include "./time/jiao_time.h"
#include "jiao_os.h"
extern struct TIMER * task_exchang_timer, *timer2;

extern struct Event_Flog EventFlog;
extern struct TIMERCTL timerctl;
uint32_t time=0;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
__asm void xPortPendSVHandler( void )
{
	extern pxCurrentTCB;
	extern vTaskSwitchContext;

	PRESERVE8

    /* 当进入PendSVC Handler时，上一个任务运行的环境即：
       xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0（任务的形参）
       这些CPU寄存器的值会自动保存到任务的栈中，剩下的r4~r11需要手动保存 */
    /* 获取任务栈指针到r0 */
	mrs r0, psp
	isb

	ldr	r3, =pxCurrentTCB		/* 加载pxCurrentTCB的地址到r3 */
	ldr	r2, [r3]                /* 加载pxCurrentTCB到r2 */

	stmdb r0!, {r4-r11}			/* 将CPU寄存器r4~r11的值存储到r0指向的地址 */
	str r0, [r2]                /* 将任务栈的新的栈顶指针存储到当前任务TCB的第一个成员，即栈顶指针 */				
                               

	stmdb sp!, {r3, r14}        /* 将R3和R14临时压入堆栈，因为即将调用函数vTaskSwitchContext,
                                  调用函数时,返回地址自动保存到R14中,所以一旦调用发生,R14的值会被覆盖,因此需要入栈保护;
                                  R3保存的当前激活的任务TCB指针(pxCurrentTCB)地址,函数调用后会用到,因此也要入栈保护 */
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY    /* 进入临界段 */
	msr basepri, r0
	dsb
	isb
	bl vTaskSwitchContext       /* 调用函数vTaskSwitchContext，寻找新的任务运行,通过使变量pxCurrentTCB指向新的任务来实现任务切换 */ 
	mov r0, #0                  /* 退出临界段 */
	msr basepri, r0
	ldmia sp!, {r3, r14}        /* 恢复r3和r14 */

	ldr r1, [r3]
	ldr r0, [r1] 				/* 当前激活的任务TCB第一项保存了任务堆栈的栈顶,现在栈顶值存入R0*/
	ldmia r0!, {r4-r11}			/* 出栈 */
	msr psp, r0
	isb
	bx r14                      /* 异常发生时,R14中保存异常返回标志,包括返回后进入线程模式还是处理器模式、
                                   使用PSP堆栈指针还是MSP堆栈指针，当调用 bx r14指令后，硬件会知道要从异常返回，
                                   然后出栈，这个时候堆栈指针PSP已经指向了新任务堆栈的正确位置，
                                   当新任务的运行地址被出栈到PC寄存器后，新的任务也会被执行。*/
	nop
}


//SVC中断
/**
  * @brief  SVC中断,主要用于加载任务函数,获取当前正在运行的任务,之后加载,切换到
  * @param  无
  * @retval None
  */
__asm void vPortSVCHandler( void )
{
    extern pxCurrentTCB;
    
    PRESERVE8

	ldr	r3, =pxCurrentTCB	/* 加载pxCurrentTCB的地址到r3 */
	ldr r1, [r3]			/* 加载pxCurrentTCB到r1 */
	ldr r0, [r1]			/* 加载pxCurrentTCB指向的值到r0，目前r0的值等于第一个任务堆栈的栈顶 */
	ldmia r0!, {r4-r11}		/* 以r0为基地址，将栈里面的内容加载到r4~r11寄存器，同时r0会递增 */
	msr psp, r0				/* 将r0的值，即任务的栈指针更新到psp */
	isb
	mov r0, #0              /* 设置r0的值为0 */
	msr	basepri, r0         /* 设置basepri寄存器的值为0，即所有的中断都没有被屏蔽 */
	orr r14, #0xd           /* 当从SVC中断服务退出前,通过向r14寄存器最后4位按位或上0x0D，
                               使得硬件在退出时使用进程堆栈指针PSP完成出栈操作并返回后进入线程模式、返回Thumb状态 */
    
	bx r14                  /* 异常返回，这个时候栈中的剩下内容将会自动加载到CPU寄存器：
                               xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0（任务的形参）
                               同时PSP的值也将更新，即指向任务栈的栈顶 */
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
}
#if IT_Config
void DEBUG_USART_IRQHandler(void)
{
	uint8_t ucTemp;
	if(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) != RESET)
	{
		ucTemp = USART_ReceiveData(DEBUG_USARTx);
		USART_SendData(DEBUG_USARTx, ucTemp);
	}
}
#endif
/**
  * @brief  中断函数,触摸屏按下的时候触发
  * @param  无
  * @retval None
  */
void EXTI4_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		printf("触摸中断\n");
		//使能时钟用来进行消抖
		TIM_Cmd(TOUCH_TIM, ENABLE);
		//关闭自己的中断
		EXTI_InitTypeDef EXTI_InitStruct;
		EXTI_InitStruct.EXTI_Line = EXTI_Line4;	//设置为EXTI0
		EXTI_InitStruct.EXTI_LineCmd = DISABLE;	//关闭中断
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//设置为中断
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上升沿触发
		EXTI_Init(&EXTI_InitStruct);

		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
/**
  * @brief  中断函数,Key1按下的时候触发
  * @param  无
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		
		if(KEY1_Read()==KEY_ON)
			fifo8_put(&EventFlog.System_Flags, KEY1_DOWN);
		if(KEY1_Read()==KEY_OFF)
			fifo8_put(&EventFlog.System_Flags, KEY1_UP);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
/**
  * @brief  中断函数,Key2按下的时候触发
  * @param  无
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		if(KEY2_Read()==KEY_ON)
			fifo8_put(&EventFlog.System_Flags, KEY2_DOWN);
		if(KEY2_Read()==KEY_OFF)
			fifo8_put(&EventFlog.System_Flags, KEY2_UP);
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  时钟中断函数,触摸屏按下的时候触发
  * @param  无
  * @retval None
  */
void  TOUCH_TIM_IRQHandler (void)
{
	if ( TIM_GetITStatus( TOUCH_TIM, TIM_IT_Update) != RESET ) 
	{	
		//判断现在的时钟触摸屏状态
		if(XPT2046_PENIRQ_Read() == Bit_RESET)
		{
			fifo8_put(&EventFlog.System_Flags, TOUCH_DOWN);
		}else
		{
			fifo8_put(&EventFlog.System_Flags, TOUCH_UP);

			TIM_Cmd(TOUCH_TIM, DISABLE);
			EXTI_InitTypeDef EXTI_InitStruct;
			EXTI_InitStruct.EXTI_Line = EXTI_Line4;	//设置为EXTI0
			EXTI_InitStruct.EXTI_LineCmd = ENABLE;	//关闭中断
			EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//设置为中断
			EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上升沿触发
			EXTI_Init(&EXTI_InitStruct);
		}

		TIM_ClearITPendingBit(TOUCH_TIM , TIM_FLAG_Update);  		 
	}		 	
}


/**
  * @brief  时钟中断函数,用于记录时间
  * @param  无
  * @retval None
  */
void  TIME_TIM_IRQHandler (void)
{
	struct TIMER *timer;
	int exchange_flog=0;
	if ( TIM_GetITStatus( TIME_TIM, TIM_IT_Update) != RESET ) 
	{	

		timerctl.count++;
		if (timerctl.next > timerctl.count) {
			TIM_ClearITPendingBit(TIME_TIM , TIM_FLAG_Update);

			return;
		}
		timer = timerctl.t0; /* 获取链表的起始位置 */
		for (;;) {
			/* 寻找下一个时序的位置 */
			if (timer->timeout > timerctl.count) {
				break;
			}
			/* 找到要处理的定时器 */
			timer->flags = TIMER_FLAGS_ALLOC;
			fifo8_put(timer->fifo, timer->data);
			if(timer == task_exchang_timer)
			{
				//是任务的切换使用的计时器
				exchange_flog=1;
			}
			timer = timer->next; /* 设置时间为下一个 */
			printf("时间到\n");


		}
		timerctl.t0 = timer;
		timerctl.next = timer->timeout;	
		TIM_ClearITPendingBit(TIME_TIM , TIM_FLAG_Update); 
		if(exchange_flog){
			timer_settime(task_exchang_timer, 100);
			taskYIELD();

		}
	}		 	
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
