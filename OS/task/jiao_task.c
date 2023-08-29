#include "./task/jiao_task.h"

//��������ջ
#define TASK1_STACK_SIZE                    80
uint32_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE                    80
uint32_t Task2Stack[TASK2_STACK_SIZE];
//������ƿ�
TCB_t Task1TCB;
TCB_t Task2TCB;
//������,ʵ���Ͼ��ǳ�ʼ��֮���TCB,��һ����ջ��ָ��
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;
extern struct TIMER * task_exchang_timer, *timer2;
extern struct Event_Flog EventFlog;

TCB_t * volatile pxCurrentTCB = NULL;

//�ٽ��ʹ�õı���
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;



//���е�����
List_t pxReadyTasksLists[ configMAX_PRIORITIES ];


/* �����ʱ */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}
/* ����1 */
uint8_t flag1;
/**
  * @brief  ����һ�Ĳ��Ժ���
  * @param  ��
  * @retval None
  */
void Task1_Entry( void *p_arg )
{	
	int i;
	timer_settime(task_exchang_timer, 100);
	LED2_TOGGLE;
	for( ;; )
	{

		if(FIFO8_Status(&EventFlog.System_Flags))
		{
			
			i = FIFO8_Get(&EventFlog.System_Flags);
		
			if(i==TIM1_FLAG)	
			{
				LED2_TOGGLE;
			}
			printf("����1\n");
		}	

		//__WFI();
		
	}
}
uint8_t flag2;

/**
  * @brief  ������Ĳ��Ժ���
  * @param  ��
  * @retval None
  */
void Task2_Entry( void *p_arg )
{
	int i;

	for( ;; )
	{	

		if(FIFO8_Status(&EventFlog.System_Flags))
		{

			i = FIFO8_Get(&EventFlog.System_Flags);
		
			if(i==TIM1_FLAG)	
			{
				LED1_TOGGLE;				
			}
			printf("����2");
		}

		//__WFI();
	}
}
/**
  * @brief  ��������ǳ�����,�����˳���ʱ��Ż����,��������²������,�������ķ��غ�����¼������һ������
  * @param  ��
  * @retval None
  */
static void prvTaskExitError( void )
{
    /* ����ֹͣ������ */
    for(;;);
}


/**
  * @brief  ������̬����
  * @param  ����ִ�еĺ���
  * @param  ���������
  * @param  ����ջ�Ĵ�С
  * @param  �����β�
  * @param  ����ջ��ʼ��ַ
  * @param  �������ָ��,���ص���TCB
  * @retval None
  */
TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* �������,����ʵ����ִ�еĺ��� */
					            const char * const pcName,           /* �������ƣ��ַ�����ʽ */
					            const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
					            void * const pvParameters,           /* �����β� */
					            uint32_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
					            TCB_t * const pxTaskBuffer )         /* ������ƿ�ָ�� */
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;

	if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
	{		
		pxNewTCB = ( TCB_t * ) pxTaskBuffer; 
		pxNewTCB->pxStack = ( uint32_t * ) puxStackBuffer;

		/* �����µ����� */
		prvInitialiseNewTask( pxTaskCode,        /* ������� */
                              pcName,            /* �������ƣ��ַ�����ʽ */
                              ulStackDepth,      /* ����ջ��С����λΪ�� */ 
                              pvParameters,      /* �����β� */
                              &xReturn,          /* ������ */ 
                              pxNewTCB);         /* ����ջ��ʼ��ַ */      

	}
	else
	{
		xReturn = NULL;
	}

	/* ������������������񴴽��ɹ�����ʱxReturnӦ��ָ��������ƿ� */
    return xReturn;
}
/**
  * @brief  ��������ĺ���,�ɴ�����̬�����ȵĺ�������
  * @param  ����ִ�еĺ���
  * @param  ���������
  * @param  ����ջ�Ĵ�С
  * @param  �����β�
  * @param  �������ָ��,���ص���TCB
  * @param  �������ָ��,����֮�����з���
  * @retval None
  */
static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
									const char * const pcName,              /* �������ƣ��ַ�����ʽ */
									const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
									void * const pvParameters,              /* �����β� */
									TaskHandle_t * const pxCreatedTask,     /* ������ */
									TCB_t *pxNewTCB )                       /* ������ƿ�ָ�� */

{
	StackType_t *pxTopOfStack;
	UBaseType_t x;	
	
	/* ��ȡջ����ַ,������һ��������� */
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
	//pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
	/* ������8�ֽڶ��� */
	pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t ) 0x0007 ) ) );	

	/* ����������ִ洢��TCB�� */
	for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];

		if( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	/* �������ֵĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';

    /* ��ʼ��TCB�е�xStateListItem�ڵ� */
    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
    /* ����xStateListItem�ڵ��ӵ���� */
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
    
    
    /* ��ʼ������ջ */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );   


	/* ��������ָ��������ƿ� */
    if( ( void * ) pxCreatedTask != NULL )
	{		
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
	}
}
/**
  * @brief  ��ʼ���������ȼ��б���ƿ������
  * @param  ��
  * @retval None
  */
void prvInitialiseTaskLists( void )
{
    UBaseType_t uxPriority;
    
    
    for( uxPriority = ( UBaseType_t ) 0U; uxPriority < ( UBaseType_t ) configMAX_PRIORITIES; uxPriority++ )
	{
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}
}
/**
  * @brief  ��ʼ�������ջ
  * @param  ջ�Ķ���
  * @param  ���صĺ���(��������²������)
  * @param  ������β�
  * @retval None
  */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* �쳣����ʱ���Զ����ص�CPU�Ĵ��������� */
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;	                                    /* xPSR��bit24������1 */
	pxTopOfStack--;
	*pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;	/* PC����������ں��� */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) prvTaskExitError;	                    /* LR���������ص�ַ,��������²��᷵�� */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1 Ĭ�ϳ�ʼ��Ϊ0 */
	*pxTopOfStack = ( StackType_t ) pvParameters;	                        /* R0�������β� */
    
    /* �쳣����ʱ���ֶ����ص�CPU�Ĵ��������� */    
	pxTopOfStack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4Ĭ�ϳ�ʼ��Ϊ0 */

	/* ����ջ��ָ�룬��ʱpxTopOfStackָ�����ջ */
    return pxTopOfStack;
}

/**
  * @brief  ������һ����Ŀ
  * @param  ��
  * @retval None
  */
void vTaskStartScheduler( void )
{
    /* �ֶ�ָ����һ�����е����� */
    pxCurrentTCB = &Task1TCB;
    
    /* ���������� */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* �����������ɹ����򲻻᷵�أ��������������� */
    }
}
/**
  * @brief  �ֶ�������һ������,��Ҫ�����õ���һ��SVC�ж�
  * @param  ��
  * @retval None
  */
__asm void prvStartFirstTask( void )
{
	PRESERVE8

	/* ��Cortex-M�У�0xE000ED08��SCB_VTOR����Ĵ����ĵ�ַ��
       �����ŵ������������ʼ��ַ����MSP�ĵ�ַ */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]

	/* ��������ջָ��msp��ֵ */
	msr msp, r0
    
	/* ʹ��ȫ���ж� */
	cpsie i
	cpsie f
	dsb
	isb
	
    /* ����SVCȥ������һ������ */
	svc 0  
	nop
	nop
}

/**
  * @brief  ���������������
  * @param  ��
  * @retval None
  */
BaseType_t xPortStartScheduler( void )
{
    /* ����PendSV �� SysTick ���ж����ȼ�Ϊ��� */
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

	/* ������һ�����񣬲��ٷ��� */
	prvStartFirstTask();

	/* ��Ӧ�����е����� */
	return 0;
}
/**
  * @brief  �������е�������л�
  * @param  ��
  * @retval None
  */
void vTaskSwitchContext( void )
{    
    /* �������������л� */
    if( pxCurrentTCB == &Task1TCB )
    {
        pxCurrentTCB = &Task2TCB;
    }
    else
    {
        pxCurrentTCB = &Task1TCB;
    }
}

/*
*************************************************************************
*                             �ٽ����غ���
*************************************************************************
*/
void vPortEnterCritical( void )
{
	//�ر����е��ж�

	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;

	/* This is not the interrupt safe version of the enter critical function so
	assert() if it is being called from an interrupt context.  Only API
	functions that end in "FromISR" can be used in an interrupt.  Only assert if
	the critical nesting count is 1 to protect against recursive calls if the
	assert function also uses a critical section. */
	if( uxCriticalNesting == 1 )
	{
		//configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );
	}
}

void vPortExitCritical( void )
{
	uxCriticalNesting--;
    
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
#if USE_TASK_MODE
/**
  * @brief  ���Ժ���
  * @param  ��
  * @retval None
  */
void Task_main(void)
{
	Task1_Handle = xTaskCreateStatic(Task1_Entry, 
						"Task1", 
						TASK1_STACK_SIZE, 
						NULL, 
						Task1Stack,
						&Task1TCB);
	Task2_Handle = xTaskCreateStatic(Task2_Entry, 
						"Task2", 
						TASK2_STACK_SIZE, 
						NULL, 
						Task2Stack,
						&Task2TCB);
	//��ʼ�����ȼ��б������
	prvInitialiseTaskLists();
	//������������ȼ��б�
	vListInsert(&pxReadyTasksLists[2], &Task2TCB.xStateListItem);
	vListInsert(&pxReadyTasksLists[3], &Task1TCB.xStateListItem);
	vTaskStartScheduler();
}

#endif
