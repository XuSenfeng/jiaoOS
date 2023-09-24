#include "./task/jiao_task.h"
//任务控制模块
struct TASKCTL task_memman;
struct TASKCTL *taskctl;
struct TIMER *task_timer;
int next_priority=10;
//定义任务栈
#define TASK1_STACK_SIZE                    0x580
uint32_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE                    80
uint32_t Task2Stack[TASK2_STACK_SIZE];

#define SENTRY_STACK_SIZE                    80
uint32_t SENTRYStack[SENTRY_STACK_SIZE];
//任务控制块
TCB_t Task1TCB;
TCB_t Task2TCB;
TCB_t SentryTCB;

//初始化的两个时钟,第一个时钟用于控制任务的切换
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
		delay(0x2fffff);
		//LED2_TOGGLE;
	}
}
/**
  * @brief  卫兵任务
  * @param  无
  * @retval None
  */
void task_idle(void *p_arg)
{
	for (;;) {
		__WFI();	//WFI指令进入睡眠,减少功耗
	}
}



/*******************************以下是任务切换使用的函数*************************************/

/**
  * @brief  这个函数是出问题,任务退出的时候才会进入,正常情况下不会进入,任务函数的返回函数记录的是这一个函数
  * @param  无
  * @retval None
  */
static void prvTaskExitError( void )
{
    /* 函数停止在这里 */
    for(;;)
		__WFI();
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
/******************************下面是任务优先级控制模块*********************************/


//获取当前的任务
struct TASK *task_now(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

/**
  * @brief  添加一个任务到对应的优先级
  * @param  设置的任务
  * @retval 返回第一个任务的控制块
  */
void task_add(struct TASK *task)
{
	struct TASKLEVEL *tl = &taskctl->level[task->level];	//获取所在的任务等级
	tl->tasks[tl->running] = task;							//添加任务
	tl->running++;								//任务数量+1
	task->flags = 2; /* 设置标志位 */
	return;
}

/**
  * @brief  讲一个任务设置为睡眠
			这个只会遍历任务所在优先级,之后对那个优先级进行重新设置
  * @param  设置的任务
  * @retval 返回第一个任务的控制块
  */
void task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	/* task位置寻找 */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* 找到之后 */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; /* 正在运行的任务前面 */
	}
	if (tl->now >= tl->running) {
		/* now位置出现问题 */
		tl->now = 0;
	}
	task->flags = 1; /* 设置任务标志 */

	/* 移动位置 */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
}

/**
  * @brief  寻找是否需要调换优先级,需要的话会直接进行更改
  * @param  无
  * @retval 返回第一个任务的控制块
  */
void task_switchsub(void)
{
	int i;
	/* 获取有任务的最高优先级 */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /* 找到了 */
		}
	}
	
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

/**
  * @brief  初始化任务优先级控制模块, 同时初始化一个任务
  * @param  无
  * @retval 返回第一个任务的控制块
  */
struct TASK *task_init(void)
{	
	int i;
	struct TASK *task, *idea;	//初始化两个任务,一个卫兵,一个是主函数
	taskctl = &task_memman;
	for (i = 0; i < MAX_TASKS; i++) {
		//设置所有的任务初始化都是没有运行
		taskctl->tasks0[i].flags = 0;
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		//设置每一个优先级的
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}
	task = task_alloc();
	task->flags = 2;	/* 设置为使用中 */
	task->priority = 20; /* 设置任务运行的时间 */
	task->level = 0;	/* 设置优先级为最高 */
	task_add(task);		//添加一个任务, 插入到对应优先级的对应位置
	task_switchsub();	/* 根据优先级对控制模块进行修改 */
	
	//哨兵
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
  * @brief  更换任务, 在中断的时候会进行调用
  * @param  无
  * @retval 返回一个任务结构体,失败的话返回0
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
		//需要更换优先级
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	next_priority = tl->tasks[next]->priority;		//获取下次需要的时间
	//在这里切换的时候实际上有可能获取到的任务在task_switchsub里面更改了
	if (new_task->tss != pxCurrentTCB) {
		pxCurrentTCB = new_task->tss;
	}
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
  * @param  要运行的任务
  * @param  任务的优先级
  * @param  任务运行的时间
  * @retval 返回一个任务结构体,失败的话返回0
  */
void task_run(struct TASK *task, int level, int priority)
{


	__disable_irq();
	if (level < 0) {
		level = task->level; /* 设置优先级等级 */
	}
	if (priority > 0) {
		task->priority = priority;	//设置运行时间
	}

	if (task->flags == 2 && task->level != level) { /* 改变活动中的任务的等级*/
		task_remove(task); /* 先把任务移除 */
	}
	if (task->flags != 2) {
		/* 从休眠状态唤醒 */
		task->level = level;
		task_add(task);
	}
	taskctl->lv_change = 1; /* 下一次切换任务的时候检查等级 */
	__enable_irq();

	return;
}
/**
  * @brief  把一个任务设置为睡眠状态
  * @param  睡眠的任务
  * @retval 返回一个任务结构体,失败的话返回0
  */
void task_sleep(struct TASK *task)
{

	struct TASK *now_task;
	if (task->flags == 2) {
		/* 获取运行中的任务 */
		now_task = task_now();
		task_remove(task); /* 把当前的任务从列表中删除 */
		if (task == now_task) {
			/* 当前的任务是在运行的任务 */
			task_switchsub();//重新设置任务的运行等级
			taskYIELD();
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
	task_run(task1, 0, 20);
	task_run(task2, 1, 10);

	//开启任务的调度
	vTaskStartScheduler();
}

#endif
