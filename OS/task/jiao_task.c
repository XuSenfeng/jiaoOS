#include "./task/jiao_task.h"
//任务控制模块
struct TASKCTL task_memman;
struct TASKCTL *taskctl;
struct TIMER *task_timer;

//定义任务栈
#define TASK1_STACK_SIZE                    0x400
uint32_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE                    80
uint32_t Task2Stack[TASK2_STACK_SIZE];
//任务控制块
TCB_t Task1TCB;
TCB_t Task2TCB;
//初始化的两个时钟
extern struct TIMER * task_exchang_timer, *timer2;
//标志位的结构体
extern struct Event_Flog EventFlog;

TCB_t * volatile pxCurrentTCB = NULL;

//临界段使用的变量
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;
//两个任务指针
struct TASK *task1, *task2;


/* 软件延时 */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}
/* 任务1 */
uint8_t flag1;
/**
  * @brief  任务一的测试函数
  * @param  无
  * @retval None
  */
void Task1_Entry( void *p_arg )
{	
	int i;
	timer_settime(task_exchang_timer, 100);
	LED2_TOGGLE;
	EventFlog.System_Flags.task = task1;

	//使用桌面模式,对各种事件进行检查
	while(1){
		//检测触控屏按压事件
		if(FIFO8_Status(&EventFlog.System_Flags))
		{
			i = FIFO8_Get(&EventFlog.System_Flags);
			if(i==1 || i==2)
			{
				//处理的事件是触摸屏
				XPT2046_TouchEvenHandler(i);
			}else if(i>2 && i<7){
				//处理的事件是按键
				Key_TouchEventHandler(i);
			}else{
				//处理的事件是定时器
				Time_OutEventHandler(i);
			}
		}else{
			task_sleep(task1);
		}			

	}
}
uint8_t flag2;

/**
  * @brief  任务二的测试函数
  * @param  无
  * @retval None
  */
void Task2_Entry( void *p_arg )
{

	for( ;; )
	{	
		printf("task2\n");
		delay(0xfffff);
		LED2_TOGGLE;
	}
}
/**
  * @brief  这个函数是出问题,任务退出的时候才会进入,正常情况下不会进入,任务函数的返回函数记录的是这一个函数
  * @param  无
  * @retval None
  */
static void prvTaskExitError( void )
{
    /* 函数停止在这里 */
    for(;;);
}


/**
  * @brief  创建静态函数
  * @param  任务执行的函数
  * @param  任务的名称
  * @param  任务栈的大小
  * @param  任务形参
  * @param  任务栈起始地址
  * @param  任务控制指针,返回的是TCB
  * @retval None
  */
TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* 任务入口,任务实际上执行的函数 */
					            const char * const pcName,           /* 任务名称，字符串形式 */
					            const uint32_t ulStackDepth,         /* 任务栈大小，单位为字 */
					            void * const pvParameters,           /* 任务形参 */
					            uint32_t * const puxStackBuffer,  /* 任务栈起始地址 */
					            TCB_t * const pxTaskBuffer )         /* 任务控制块指针 */
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;

	if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
	{		
		pxNewTCB = ( TCB_t * ) pxTaskBuffer; 
		pxNewTCB->pxStack = ( uint32_t * ) puxStackBuffer;

		/* 创建新的任务 */
		prvInitialiseNewTask( pxTaskCode,        /* 任务入口 */
                              pcName,            /* 任务名称，字符串形式 */
                              ulStackDepth,      /* 任务栈大小，单位为字 */ 
                              pvParameters,      /* 任务形参 */
                              &xReturn,          /* 任务句柄 */ 
                              pxNewTCB);         /* 任务栈起始地址 */      

	}
	else
	{
		xReturn = NULL;
	}

	/* 返回任务句柄，如果任务创建成功，此时xReturn应该指向任务控制块 */
    return xReturn;
}
/**
  * @brief  创建任务的函数,由创建静态函数等的函数调用
  * @param  任务执行的函数
  * @param  任务的名称
  * @param  任务栈的大小
  * @param  任务形参
  * @param  任务控制指针,返回的是TCB
  * @param  任务控制指针,操作之后会进行返回
  * @retval None
  */
static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* 任务入口 */
									const char * const pcName,              /* 任务名称，字符串形式 */
									const uint32_t ulStackDepth,            /* 任务栈大小，单位为字 */
									void * const pvParameters,              /* 任务形参 */
									TaskHandle_t * const pxCreatedTask,     /* 任务句柄 */
									TCB_t *pxNewTCB )                       /* 任务控制块指针 */

{
	StackType_t *pxTopOfStack;
	UBaseType_t x;	
	
	/* 获取栈顶地址,并进行一个对齐操作 */
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
	//pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
	/* 向下做8字节对齐 */
	pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t ) 0x0007 ) ) );	

	/* 将任务的名字存储在TCB中 */
	for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];

		if( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	/* 任务名字的长度不能超过configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';

    
    /* 初始化任务栈 */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );   


	/* 让任务句柄指向任务控制块 */
    if( ( void * ) pxCreatedTask != NULL )
	{		
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
	}
}

/**
  * @brief  初始化任务的栈
  * @param  栈的顶层
  * @param  返回的函数(正常情况下不会调用)
  * @param  任务的形参
  * @retval None
  */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* 异常发生时，自动加载到CPU寄存器的内容 */
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;	                                    /* xPSR的bit24必须置1 */
	pxTopOfStack--;
	*pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;	/* PC，即任务入口函数 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) prvTaskExitError;	                    /* LR，函数返回地址,正常情况下不会返回 */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1 默认初始化为0 */
	*pxTopOfStack = ( StackType_t ) pvParameters;	                        /* R0，任务形参 */
    
    /* 异常发生时，手动加载到CPU寄存器的内容 */    
	pxTopOfStack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4默认初始化为0 */

	/* 返回栈顶指针，此时pxTopOfStack指向空闲栈 */
    return pxTopOfStack;
}

/**
  * @brief  启动第一个项目
  * @param  无
  * @retval None
  */
void vTaskStartScheduler( void )
{
    /* 手动指定第一个运行的任务 */
    pxCurrentTCB = &Task1TCB;
    
    /* 启动调度器 */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* 调度器启动成功，则不会返回，即不会来到这里 */
    }
}
/**
  * @brief  手动启动第一个任务,主要是设置调用一个SVC中断
  * @param  无
  * @retval None
  */
__asm void prvStartFirstTask( void )
{
	PRESERVE8

	/* 在Cortex-M中，0xE000ED08是SCB_VTOR这个寄存器的地址，
       里面存放的是向量表的起始地址，即MSP的地址 */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]

	/* 设置主堆栈指针msp的值 */
	msr msp, r0
    
	/* 使能全局中断 */
	cpsie i
	cpsie f
	dsb
	isb
	
    /* 调用SVC去启动第一个任务 */
	svc 0  
	nop
	nop
}

/**
  * @brief  任务调度器的启动
  * @param  无
  * @retval None
  */
BaseType_t xPortStartScheduler( void )
{
    /* 配置PendSV 和 SysTick 的中断优先级为最低 */
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

	/* 启动第一个任务，不再返回 */
	prvStartFirstTask();

	/* 不应该运行到这里 */
	return 0;
}
/**
  * @brief  正在运行的任务的切换
  * @param  无
  * @retval None
  */
void vTaskSwitchContext( void )
{    
    /* 两个任务轮流切换 */
	task_switch();
}
/********下面是任务优先级控制模块**********/

/**
  * @brief  初始化任务优先级控制模块, 同时初始化一个任务
  * @param  无
  * @retval 返回第一个任务的控制块
  */
struct TASK *task_init(void)
{
	int i;
	struct TASK *task;
	//设置控制模块
	taskctl = &task_memman;
	//修改控制模块的所有任务的标志位
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
	}
	task = task_alloc();
	task->flags = 2; /* 标志正在活动中 */
	taskctl->running = 0;
	taskctl->now = 0;
	taskctl->tasks[0] = task;
	return task;
}
/**
  * @brief  
  * @param  无
  * @retval 返回一个任务结构体,失败的话返回0
  */
void task_switch(void)
{
	taskctl->now++;

	if (taskctl->now == taskctl->running) {
		taskctl->now = 0;
	}
	pxCurrentTCB = taskctl->tasks[taskctl->now]->tss;
	return;
}
/**
  * @brief  申请一个任务
  * @param  无
  * @retval 返回一个任务结构体,失败的话返回0
  */
struct TASK *task_alloc(void)
{
	int i;
	struct TASK *task;
	__disable_irq();
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 使用中 */
			__enable_irq();
			return task;
		}
	}
	__enable_irq();
	return 0; /* 所有的都在使用中 */
}
/**
  * @brief  把任务插在数组的最后面
  * @param  无
  * @retval 返回一个任务结构体,失败的话返回0
  */
void task_run(struct TASK *task)
{
	__disable_irq();
	task->flags = 2; /* 运行中 */
	taskctl->tasks[taskctl->running] = task;
	taskctl->running++;
	__enable_irq();
	return;
}
/**
  * @brief  把一个任务设置为睡眠状态
  * @param  无
  * @retval 返回一个任务结构体,失败的话返回0
  */
void task_sleep(struct TASK *task)
{
	int i;
	char ts = 0;
	if (task->flags == 2) {		/* 指定的任务正在运行 */
		if (task == taskctl->tasks[taskctl->now]) {
			ts = 1; /* 这是正在运行的任务 */
		}
		/* 找到当前运行的任务在列表中的位置 */
		for (i = 0; i < taskctl->running; i++) {
			if (taskctl->tasks[i] == task) {
				/* 找到了 */
				break;
			}
		}
		taskctl->running--;
		if (i < taskctl->now) {
			//这个任务在当前的任务之前
			taskctl->now--; /* 当前运行的任务的位置减少一个*/
		}
		/* 重新排序 */
		for (; i < taskctl->running; i++) {
			taskctl->tasks[i] = taskctl->tasks[i + 1];
		}
		task->flags = 1; /* 任务的状态 */
		if (ts != 0) {
			/* 当前的任务在运行 */
			if (taskctl->now >= taskctl->running) {
				/* now出现异常进行修复 */
				taskctl->now = 0;
				taskYIELD();
			}			
		}
	}
	return;
}
/*
*************************************************************************
*                             临界段相关函数
*************************************************************************
*/
//进入临界区
void vPortEnterCritical( void )
{
	//关闭所有的中断

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
//退出临界区
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
  * @brief  测试函数
  * @param  无
  * @retval None
  */
void Task_main(void)
{
	//获取两个任务的结构体指针
	task1 = task_init();
	task2 = task_alloc();
	//设置任务的实际操控的TCB
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

	//申请一个任务, 任务设置为运行状态
	task_run(task1);
	task_run(task2);
	//开启任务的调度
	vTaskStartScheduler();
}

#endif
