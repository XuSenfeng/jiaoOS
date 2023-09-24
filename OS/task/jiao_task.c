#include "./task/jiao_task.h"
//�������ģ��
struct TASKCTL task_memman;
struct TASKCTL *taskctl;
struct TIMER *task_timer;
int next_priority=10;
//��������ջ
#define TASK1_STACK_SIZE                    0x580
uint32_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE                    80
uint32_t Task2Stack[TASK2_STACK_SIZE];

#define SENTRY_STACK_SIZE                    80
uint32_t SENTRYStack[SENTRY_STACK_SIZE];
//������ƿ�
TCB_t Task1TCB;
TCB_t Task2TCB;
TCB_t SentryTCB;

//��ʼ��������ʱ��,��һ��ʱ�����ڿ���������л�
extern struct TIMER * task_exchang_timer, *timer2;
//��־λ�Ľṹ��
extern struct Event_Flog EventFlog;

TCB_t * volatile pxCurrentTCB = NULL;

//�ٽ��ʹ�õı���
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;
//��������ָ��
struct TASK *task1, *task2;


/* �����ʱ */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}
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
	EventFlog.System_Flags.task = task1;

	//ʹ������ģʽ,�Ը����¼����м��
	while(1){
		//��ⴥ������ѹ�¼�
		if(FIFO8_Status(&EventFlog.System_Flags))
		{
			i = FIFO8_Get(&EventFlog.System_Flags);
			if(i==1 || i==2)
			{
				//������¼��Ǵ�����
				XPT2046_TouchEvenHandler(i);
			}else if(i>2 && i<7){
				//������¼��ǰ���
				Key_TouchEventHandler(i);
			}else{
				//������¼��Ƕ�ʱ��
				Time_OutEventHandler(i);
			}
		}else{
			task_sleep(task1);
		}			

	}
}

/**
  * @brief  ������Ĳ��Ժ���
  * @param  ��
  * @retval None
  */
void Task2_Entry( void *p_arg )
{

	for( ;; )
	{	
		printf("task2\n");
		delay(0x2fffff);
		//LED2_TOGGLE;
	}
}
/**
  * @brief  ��������
  * @param  ��
  * @retval None
  */
void task_idle(void *p_arg)
{
	for (;;) {
		__WFI();	//WFIָ�����˯��,���ٹ���
	}
}



/*******************************�����������л�ʹ�õĺ���*************************************/

/**
  * @brief  ��������ǳ�����,�����˳���ʱ��Ż����,��������²������,�������ķ��غ�����¼������һ������
  * @param  ��
  * @retval None
  */
static void prvTaskExitError( void )
{
    /* ����ֹͣ������ */
    for(;;)
		__WFI();
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

    
    /* ��ʼ������ջ */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );   


	/* ��������ָ��������ƿ� */
    if( ( void * ) pxCreatedTask != NULL )
	{		
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
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
	task_switch();
}
/******************************�������������ȼ�����ģ��*********************************/


//��ȡ��ǰ������
struct TASK *task_now(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

/**
  * @brief  ���һ�����񵽶�Ӧ�����ȼ�
  * @param  ���õ�����
  * @retval ���ص�һ������Ŀ��ƿ�
  */
void task_add(struct TASK *task)
{
	struct TASKLEVEL *tl = &taskctl->level[task->level];	//��ȡ���ڵ�����ȼ�
	tl->tasks[tl->running] = task;							//�������
	tl->running++;								//��������+1
	task->flags = 2; /* ���ñ�־λ */
	return;
}

/**
  * @brief  ��һ����������Ϊ˯��
			���ֻ����������������ȼ�,֮����Ǹ����ȼ�������������
  * @param  ���õ�����
  * @retval ���ص�һ������Ŀ��ƿ�
  */
void task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	/* taskλ��Ѱ�� */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* �ҵ�֮�� */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; /* �������е�����ǰ�� */
	}
	if (tl->now >= tl->running) {
		/* nowλ�ó������� */
		tl->now = 0;
	}
	task->flags = 1; /* ���������־ */

	/* �ƶ�λ�� */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
}

/**
  * @brief  Ѱ���Ƿ���Ҫ�������ȼ�,��Ҫ�Ļ���ֱ�ӽ��и���
  * @param  ��
  * @retval ���ص�һ������Ŀ��ƿ�
  */
void task_switchsub(void)
{
	int i;
	/* ��ȡ�������������ȼ� */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /* �ҵ��� */
		}
	}
	
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

/**
  * @brief  ��ʼ���������ȼ�����ģ��, ͬʱ��ʼ��һ������
  * @param  ��
  * @retval ���ص�һ������Ŀ��ƿ�
  */
struct TASK *task_init(void)
{	
	int i;
	struct TASK *task, *idea;	//��ʼ����������,һ������,һ����������
	taskctl = &task_memman;
	for (i = 0; i < MAX_TASKS; i++) {
		//�������е������ʼ������û������
		taskctl->tasks0[i].flags = 0;
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		//����ÿһ�����ȼ���
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}
	task = task_alloc();
	task->flags = 2;	/* ����Ϊʹ���� */
	task->priority = 20; /* �����������е�ʱ�� */
	task->level = 0;	/* �������ȼ�Ϊ��� */
	task_add(task);		//���һ������, ���뵽��Ӧ���ȼ��Ķ�Ӧλ��
	task_switchsub();	/* �������ȼ��Կ���ģ������޸� */
	
	//�ڱ�
	idea = task_alloc();
	idea->tss = xTaskCreateStatic(task_idle, 
					"Sentry", 
					SENTRY_STACK_SIZE, 
					NULL, 
					SENTRYStack,
					&SentryTCB);
	task_run(idea, MAX_TASKLEVELS-1, 1);
	return task;
}
/**
  * @brief  ��������, ���жϵ�ʱ�����е���
  * @param  ��
  * @retval ����һ������ṹ��,ʧ�ܵĻ�����0
  */
void task_switch(void)
{

	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task;
	int next;
	
	tl->now++;
	next = tl->now+1;
	if (tl->now >= tl->running) {
		tl->now = 0;
	}
	if (next >= tl->running) {
		next = 0;
	}

	if (taskctl->lv_change != 0) {
		//��Ҫ�������ȼ�
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	next_priority = tl->tasks[next]->priority;		//��ȡ�´���Ҫ��ʱ��
	//�������л���ʱ��ʵ�����п��ܻ�ȡ����������task_switchsub���������
	if (new_task->tss != pxCurrentTCB) {
		pxCurrentTCB = new_task->tss;
	}
	return;
}
/**
  * @brief  ����һ������
  * @param  ��
  * @retval ����һ������ṹ��,ʧ�ܵĻ�����0
  */
struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	__disable_irq();
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* ʹ���� */
			__enable_irq();
			return task;
		}
	}
	__enable_irq();
	return 0; /* ���еĶ���ʹ���� */
}
/**
  * @brief  �������������������
  * @param  Ҫ���е�����
  * @param  ��������ȼ�
  * @param  �������е�ʱ��
  * @retval ����һ������ṹ��,ʧ�ܵĻ�����0
  */
void task_run(struct TASK *task, int level, int priority)
{


	__disable_irq();
	if (level < 0) {
		level = task->level; /* �������ȼ��ȼ� */
	}
	if (priority > 0) {
		task->priority = priority;	//��������ʱ��
	}

	if (task->flags == 2 && task->level != level) { /* �ı��е�����ĵȼ�*/
		task_remove(task); /* �Ȱ������Ƴ� */
	}
	if (task->flags != 2) {
		/* ������״̬���� */
		task->level = level;
		task_add(task);
	}
	taskctl->lv_change = 1; /* ��һ���л������ʱ����ȼ� */
	__enable_irq();

	return;
}
/**
  * @brief  ��һ����������Ϊ˯��״̬
  * @param  ˯�ߵ�����
  * @retval ����һ������ṹ��,ʧ�ܵĻ�����0
  */
void task_sleep(struct TASK *task)
{

	struct TASK *now_task;
	if (task->flags == 2) {
		/* ��ȡ�����е����� */
		now_task = task_now();
		task_remove(task); /* �ѵ�ǰ��������б���ɾ�� */
		if (task == now_task) {
			/* ��ǰ�������������е����� */
			task_switchsub();//����������������еȼ�
			taskYIELD();
		}
	}
	return;


}
/*
*************************************************************************
*                             �ٽ����غ���
*************************************************************************
*/
//�����ٽ���
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
//�˳��ٽ���
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

	//��ȡ��������Ľṹ��ָ��
	task1 = task_init();
	task2 = task_alloc();
	//���������ʵ�ʲٿص�TCB
	task1->tss = xTaskCreateStatic(Task1_Entry, 
						"Task1", 
						TASK1_STACK_SIZE, 
						NULL, 
						Task1Stack,
						&Task1TCB);
	task2->tss = xTaskCreateStatic(Task2_Entry, 
						"Task2", 
						TASK2_STACK_SIZE, 
						NULL, 
						Task2Stack,
						&Task2TCB);

	//����һ������, ��������Ϊ����״̬
	task_run(task1, 0, 20);
	task_run(task2, 1, 10);

	//��������ĵ���
	vTaskStartScheduler();
}

#endif
