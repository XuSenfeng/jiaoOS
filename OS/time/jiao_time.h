#ifndef __JIAO_TIME_H__
#define __JIAO_TIME_H__

#include "jiao_os.h"

#define MAX_TIMER		5
#define TIMER_FLAGS_ALLOC		1	/* ��ʼ�� */
#define TIMER_FLAGS_USING		2	/* ������ */
struct TIMER {
	struct TIMER *next;
	unsigned int timeout, flags;
	struct FIFO8 *fifo;
	unsigned char data;
};
struct TIMERCTL {
	unsigned int count, next;
	//ʱ���������ʼλ��
	struct TIMER *t0;
	//��¼���е�ʱ��
	struct TIMER timers0[MAX_TIMER];
};

struct TIMER *timer_alloc(void);
void Timer_init(void);
void init_time_ctl(void);
void Time_OutEventHandler(int i);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void timer_settime_without_change_irq(struct TIMER *timer, unsigned int timeout);

#endif
