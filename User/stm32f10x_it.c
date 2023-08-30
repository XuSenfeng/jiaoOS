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

    /* ������PendSVC Handlerʱ����һ���������еĻ�������
       xPSR��PC��������ڵ�ַ����R14��R12��R3��R2��R1��R0��������βΣ�
       ��ЩCPU�Ĵ�����ֵ���Զ����浽�����ջ�У�ʣ�µ�r4~r11��Ҫ�ֶ����� */
    /* ��ȡ����ջָ�뵽r0 */
	mrs r0, psp
	isb

	ldr	r3, =pxCurrentTCB		/* ����pxCurrentTCB�ĵ�ַ��r3 */
	ldr	r2, [r3]                /* ����pxCurrentTCB��r2 */

	stmdb r0!, {r4-r11}			/* ��CPU�Ĵ���r4~r11��ֵ�洢��r0ָ��ĵ�ַ */
	str r0, [r2]                /* ������ջ���µ�ջ��ָ��洢����ǰ����TCB�ĵ�һ����Ա����ջ��ָ�� */				
                               

	stmdb sp!, {r3, r14}        /* ��R3��R14��ʱѹ���ջ����Ϊ�������ú���vTaskSwitchContext,
                                  ���ú���ʱ,���ص�ַ�Զ����浽R14��,����һ�����÷���,R14��ֵ�ᱻ����,�����Ҫ��ջ����;
                                  R3����ĵ�ǰ���������TCBָ��(pxCurrentTCB)��ַ,�������ú���õ�,���ҲҪ��ջ���� */
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY    /* �����ٽ�� */
	msr basepri, r0
	dsb
	isb
	bl vTaskSwitchContext       /* ���ú���vTaskSwitchContext��Ѱ���µ���������,ͨ��ʹ����pxCurrentTCBָ���µ�������ʵ�������л� */ 
	mov r0, #0                  /* �˳��ٽ�� */
	msr basepri, r0
	ldmia sp!, {r3, r14}        /* �ָ�r3��r14 */

	ldr r1, [r3]
	ldr r0, [r1] 				/* ��ǰ���������TCB��һ����������ջ��ջ��,����ջ��ֵ����R0*/
	ldmia r0!, {r4-r11}			/* ��ջ */
	msr psp, r0
	isb
	bx r14                      /* �쳣����ʱ,R14�б����쳣���ر�־,�������غ�����߳�ģʽ���Ǵ�����ģʽ��
                                   ʹ��PSP��ջָ�뻹��MSP��ջָ�룬������ bx r14ָ���Ӳ����֪��Ҫ���쳣���أ�
                                   Ȼ���ջ�����ʱ���ջָ��PSP�Ѿ�ָ�����������ջ����ȷλ�ã�
                                   ������������е�ַ����ջ��PC�Ĵ������µ�����Ҳ�ᱻִ�С�*/
	nop
}


//SVC�ж�
/**
  * @brief  SVC�ж�,��Ҫ���ڼ���������,��ȡ��ǰ�������е�����,֮�����,�л���
  * @param  ��
  * @retval None
  */
__asm void vPortSVCHandler( void )
{
    extern pxCurrentTCB;
    
    PRESERVE8

	ldr	r3, =pxCurrentTCB	/* ����pxCurrentTCB�ĵ�ַ��r3 */
	ldr r1, [r3]			/* ����pxCurrentTCB��r1 */
	ldr r0, [r1]			/* ����pxCurrentTCBָ���ֵ��r0��Ŀǰr0��ֵ���ڵ�һ�������ջ��ջ�� */
	ldmia r0!, {r4-r11}		/* ��r0Ϊ����ַ����ջ��������ݼ��ص�r4~r11�Ĵ�����ͬʱr0����� */
	msr psp, r0				/* ��r0��ֵ���������ջָ����µ�psp */
	isb
	mov r0, #0              /* ����r0��ֵΪ0 */
	msr	basepri, r0         /* ����basepri�Ĵ�����ֵΪ0�������е��ж϶�û�б����� */
	orr r14, #0xd           /* ����SVC�жϷ����˳�ǰ,ͨ����r14�Ĵ������4λ��λ����0x0D��
                               ʹ��Ӳ�����˳�ʱʹ�ý��̶�ջָ��PSP��ɳ�ջ���������غ�����߳�ģʽ������Thumb״̬ */
    
	bx r14                  /* �쳣���أ����ʱ��ջ�е�ʣ�����ݽ����Զ����ص�CPU�Ĵ�����
                               xPSR��PC��������ڵ�ַ����R14��R12��R3��R2��R1��R0��������βΣ�
                               ͬʱPSP��ֵҲ�����£���ָ������ջ��ջ�� */
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
  * @brief  �жϺ���,���������µ�ʱ�򴥷�
  * @param  ��
  * @retval None
  */
void EXTI4_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		printf("�����ж�\n");
		//ʹ��ʱ��������������
		TIM_Cmd(TOUCH_TIM, ENABLE);
		//�ر��Լ����ж�
		EXTI_InitTypeDef EXTI_InitStruct;
		EXTI_InitStruct.EXTI_Line = EXTI_Line4;	//����ΪEXTI0
		EXTI_InitStruct.EXTI_LineCmd = DISABLE;	//�ر��ж�
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//����Ϊ�ж�
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�����ش���
		EXTI_Init(&EXTI_InitStruct);

		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
/**
  * @brief  �жϺ���,Key1���µ�ʱ�򴥷�
  * @param  ��
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
  * @brief  �жϺ���,Key2���µ�ʱ�򴥷�
  * @param  ��
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
  * @brief  ʱ���жϺ���,���������µ�ʱ�򴥷�
  * @param  ��
  * @retval None
  */
void  TOUCH_TIM_IRQHandler (void)
{
	if ( TIM_GetITStatus( TOUCH_TIM, TIM_IT_Update) != RESET ) 
	{	
		//�ж����ڵ�ʱ�Ӵ�����״̬
		if(XPT2046_PENIRQ_Read() == Bit_RESET)
		{
			fifo8_put(&EventFlog.System_Flags, TOUCH_DOWN);
		}else
		{
			fifo8_put(&EventFlog.System_Flags, TOUCH_UP);

			TIM_Cmd(TOUCH_TIM, DISABLE);
			EXTI_InitTypeDef EXTI_InitStruct;
			EXTI_InitStruct.EXTI_Line = EXTI_Line4;	//����ΪEXTI0
			EXTI_InitStruct.EXTI_LineCmd = ENABLE;	//�ر��ж�
			EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//����Ϊ�ж�
			EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�����ش���
			EXTI_Init(&EXTI_InitStruct);
		}

		TIM_ClearITPendingBit(TOUCH_TIM , TIM_FLAG_Update);  		 
	}		 	
}
extern struct TASKCTL *taskctl;

/**
  * @brief  ʱ���жϺ���,���ڼ�¼ʱ��
  * @param  ��
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
		timer = timerctl.t0; /* ��ȡ�������ʼλ�� */
		for (;;) {
			/* Ѱ����һ��ʱ���λ�� */
			if (timer->timeout > timerctl.count) {
				break;
			}
			/* �ҵ�Ҫ����Ķ�ʱ�� */
			timer->flags = TIMER_FLAGS_ALLOC;
			if(timer == task_exchang_timer)
			{
				//��������л�ʹ�õļ�ʱ��
				exchange_flog=1;
			}else{
				fifo8_put(timer->fifo, timer->data);
			}
			timer = timer->next; /* ����ʱ��Ϊ��һ�� */

		}

		timerctl.t0 = timer;
		timerctl.next = timer->timeout;	
		TIM_ClearITPendingBit(TIME_TIM , TIM_FLAG_Update); 
		if(exchange_flog){
			timer_settime(task_exchang_timer, 10);
			printf("�л�����%d   %d\n", taskctl->running, taskctl->now);
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
