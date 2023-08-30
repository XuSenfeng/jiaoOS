#include "./time/jiao_time.h"

/***********************************
��ʼ��һ��ϵͳʱ��, ����ʹ�õ���TIM7
************************************/

//ʱ�ӿ��ƿ�
struct TIMERCTL timerctl;
//�����ʱ�ӵ�ָ��
struct TIMER * task_exchang_timer, *timer2;
//��־λ
extern struct Event_Flog EventFlog;




/**
  * @brief  ��ʼ��ʱ�ӵĿ�����
  * @param  ��
  * @retval None
  */
void init_pit(void)
{
	int i;
	struct TIMER *t;

	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; /* ����Ϊû��ʹ�� */
	}
	t = timer_alloc(); /* ����һ���ڱ� */
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0; /* û����һ�� */
	timerctl.t0 = t; /* ����Ϊ�ڱ� */
	timerctl.next = 0xffffffff; /* ����Ϊ���ʱ�� */
	return;
}

/**
  * @brief  ����һ��ʱ��
  * @param  ��
  * @retval ����ɹ�����һ��ʱ�ӽṹ��,ʧ�ܷ���0
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
	return 0; /* ����ʧ�� */
}
/**
  * @brief  �ͷ�һ��ʱ��
  * @param  ��
  * @retval ����ɹ�����һ��ʱ�ӽṹ��,ʧ�ܷ���0
  */
void timer_free(struct TIMER *timer)
{
	timer->flags = 0; /* δʹ�� */
	return;
}
/**
  * @brief  ��ʼ��һ��ʱ��
  * @param  ʱ�ӽṹ��
  * @param  ʱ��ʹ�õ�FIFO
  * @param  ʱ��ʱ�䵽��ʱ�򷵻ص���ֵ
  * @retval None
  */
void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}
/**
  * @brief  ����һ��ʱ�Ӷ೤ʱ�������
  * @param  ʱ�ӽṹ��
  * @param  ���õ�ʱ��
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
		/* ֱ�Ӳ���ͷ�� */
		timerctl.t0 = timer;
		timer->next = t; /* ��һ����t */
		timerctl.next = timer->timeout;
		__enable_irq();
		return;
	}
	/* Ѱ���м��λ�� */
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			/* s��t���м���в��� */
			s->next = timer; /* s��һ����timer */
			timer->next = t; /* timer��һ����t */
			__enable_irq();
			return;
		}
	}
}




/**
  * @brief  ��ʼ��ʱ��,��ϵͳ��ʼ����ʱ������
  * @param  None
  * @retval None
  */
void Timer_init()
{
	//����������һ�������л�����ĺ���
	task_exchang_timer = timer_alloc();
	timer_init(task_exchang_timer, &EventFlog.System_Flags, TIM1_FLAG);
	timer2 = timer_alloc();
	timer_init(timer2, &EventFlog.System_Flags, TIM2_FLAG);
	//timer_settime(timer2, 1000);
}
/**
  * @brief  ��main�����е��õĶ�ʱ�ӳ�ʱ�Ĵ�����
  * @param  None
  * @retval None
  */
void Time_OutEventHandler(int i)
{
	
	if(i==TIM1_FLAG)	
	{
		//LED2_TOGGLE;
	}

	
}
