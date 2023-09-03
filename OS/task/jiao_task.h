#ifndef __JIAO_TASK_H__
#define __JIAO_TASK_H__

#include "jiao_list.h"
#include <stdio.h>
//#include "jiao_FIFO.h"
#include "bsp_led.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "jiao_os.h"
//����һЩ�����ʼ��ʹ�õĲ���

//��������ֵ�������
#define configMAX_TASK_NAME_LEN		            ( 16 )
//���������
#define MAX_TASKS		            ( 5 )
#define portINITIAL_XPSR			        ( 0x01000000 )
#define portSTART_ADDRESS_MASK				( ( StackType_t ) 0xfffffffeUL )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191   /* ����λ��Ч��������0xb0��������11 */




//���������,�����������ʹ��,���������ʼ����Ҫ����Ϣ
typedef struct tskTaskControlBlock
{
	volatile uint32_t    *pxTopOfStack;    /* ջ�� */
    
    uint32_t             *pxStack;         /* ����ջ��ʼ��ַ */
	                                          /* �������ƣ��ַ�����ʽ */
	char                    pcTaskName[ configMAX_TASK_NAME_LEN ];  
} tskTCB;

//һ������,�������������
struct TASK {
	int flags;			//��¼��ǰ�����״̬,�������������,Ϊ1��ʾ������û������,2��ʾ������
	tskTCB * tss;		//��һ���¼����Ŀ��ƿ�
	int priority;
};

//����Ŀ���ģ��
struct TASKCTL {
	int running; /* �������е���������� */
	int now; /* �������е����� */
	struct TASK *tasks[MAX_TASKS];
	struct TASK tasks0[MAX_TASKS];
};

//һЩ�������¶���
typedef tskTCB TCB_t;
typedef void * TaskHandle_t;
typedef void (*TaskFunction_t)( void * );
typedef uint32_t StackType_t;
typedef long BaseType_t;
//�ں˿��ƼĴ���
#define portNVIC_SYSPRI2_REG				( * ( ( volatile uint32_t * ) 0xe000ed20 ) )
//���ȼ�����
#define configKERNEL_INTERRUPT_PRIORITY 		255   /* ����λ��Ч��������0xff��������15 */

#define portNVIC_PENDSV_PRI					( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 16UL )
#define portNVIC_SYSTICK_PRI				( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )
//ͬ���жϺ���������
#define xPortPendSVHandler   PendSV_Handler
#define xPortSysTickHandler  SysTick_Handler
#define vPortSVCHandler      SVC_Handler




/* �жϿ���״̬�Ĵ�����0xe000ed04
 * Bit 28 PENDSVSET: PendSV ����λ
 */
 //PandSV�Ŀ������ĵ�ַ
#define portNVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
//�����λ��
#define portNVIC_PENDSVSET_BIT		( 1UL << 28UL )

#define portSY_FULL_READ_WRITE		( 15 )
//��������λ�ĺ���,֮������PandSV����
#define portYIELD()																\
{																				\
	/* ����PendSV�������������л� */								                \
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;								\
	__dsb( portSY_FULL_READ_WRITE );											\
	__isb( portSY_FULL_READ_WRITE );											\
}
#define taskYIELD()			portYIELD()



#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );

TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* ������� */
					            const char * const pcName,           /* �������ƣ��ַ�����ʽ */
					            const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
					            void * const pvParameters,           /* �����β� */
					            uint32_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
					            TCB_t * const pxTaskBuffer );         /* ������ƿ�ָ�� */
static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
									const char * const pcName,              /* �������ƣ��ַ�����ʽ */
									const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
									void * const pvParameters,              /* �����β� */
									TaskHandle_t * const pxCreatedTask,     /* ������ */
									TCB_t *pxNewTCB );                     /* ������ƿ�ָ�� */
//void prvInitialiseTaskLists( void );
BaseType_t xPortStartScheduler( void );
void vTaskSwitchContext( void );
struct TASK *task_alloc(void);
void task_switch(void);
void task_sleep(struct TASK *task);
void task_run(struct TASK *task, int priority);


/* �ٽ������� */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
									
/*******************�ٽ���***********************/

#ifndef portFORCE_INLINE
	#define portFORCE_INLINE __forceinline
#endif
//û�з���ֵ,����Ƕ��ʹ��			
#define portDISABLE_INTERRUPTS()				vPortRaiseBASEPRI()
#define portENABLE_INTERRUPTS()					vPortSetBASEPRI( 0 )

//�з���ֵ����Ƕ��ʹ�õĿ����ж�
#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortRaiseBASEPRI()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortSetBASEPRI(x)
//������ٽ���
#define portENTER_CRITICAL()					vPortEnterCritical()
#define portEXIT_CRITICAL()						vPortExitCritical()

static portFORCE_INLINE void vPortRaiseBASEPRI( void )//û�з���ֵ,�������ж���ʹ��
{
uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical
		section. */
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
}


static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )//�з���ֵ,���ȱ���֮ǰ�����εȼ�
{
uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical
		section. */
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}

	return ulReturn;
}


static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
	__asm
	{
		/* Barrier instructions are not used as this function is only used to
		lower the BASEPRI value. */
		msr basepri, ulBASEPRI
	}
}


#if USE_TASK_MODE

void Task_main(void);
#endif

#endif
