#include "./time/jiao_time.h"

/***********************************
初始化一个系统时钟, 这里使用的是TIM7
************************************/


struct TIMERCTL timerctl;
struct TIMER * task_exchang_timer, *timer2;
extern struct Event_Flog EventFlog;




/**
  * @brief  初始化时钟的控制器
  * @param  无
  * @retval None
  */
void init_pit(void)
{
	int i;
	struct TIMER *t;

	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; /* 设置为没有使用 */
	}
	t = timer_alloc(); /* 申请一个哨兵 */
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0; /* 没有下一个 */
	timerctl.t0 = t; /* 设置为哨兵 */
	timerctl.next = 0xffffffff; /* 设置为最大时间 */
	return;
}

/**
  * @brief  申请一个时钟
  * @param  无
  * @retval 申请成功返回一个时钟结构体,失败返回0
  */
struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0; /* 申请失败 */
}
/**
  * @brief  释放一个时钟
  * @param  无
  * @retval 申请成功返回一个时钟结构体,失败返回0
  */
void timer_free(struct TIMER *timer)
{
	timer->flags = 0; /* 未使用 */
	return;
}
/**
  * @brief  初始化一个时钟
  * @param  时钟结构体
  * @param  时钟使用的FIFO
  * @param  时钟时间到的时候返回的数值
  * @retval None
  */
void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}
/**
  * @brief  设置一个时钟多长时间后提醒
  * @param  时钟结构体
  * @param  设置的时间
  * @retval None
  */
void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	__disable_irq();

	t = timerctl.t0;
	if (timer->timeout <= t->timeout) {
		/* 直接插入头部 */
		timerctl.t0 = timer;
		timer->next = t; /* 下一个是t */
		timerctl.next = timer->timeout;
		__enable_irq();
		return;
	}
	/* 寻找中间的位置 */
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			/* s和t的中间进行插入 */
			s->next = timer; /* s下一个是timer */
			timer->next = t; /* timer上一个是t */
			__enable_irq();
			return;
		}
	}
}




/**
  * @brief  初始化时钟,在系统初始化的时候会调用
  * @param  None
  * @retval None
  */
void Timer_init()
{
	//在这里设置一个用于切换任务的函数
	task_exchang_timer = timer_alloc();
	timer_init(task_exchang_timer, &EventFlog.System_Flags, TIM1_FLAG);
	timer2 = timer_alloc();
	timer_init(timer2, &EventFlog.System_Flags, TIM2_FLAG);
	//timer_settime(timer2, 1000);
}
/**
  * @brief  在main函数中调用的对时钟超时的处理函数
  * @param  None
  * @retval None
  */
void Time_OutEventHandler(int i)
{
	
	if(i==TIM1_FLAG)	
	{
		printf("\ntime1out i = %d\n\n", i);
		timer_settime(task_exchang_timer, 1000);
	}else if(i==TIM2_FLAG)
	{
		printf("\ntime2out i = %d\n\n", i);
		timer_settime(timer2, 500);
	}

	
}
