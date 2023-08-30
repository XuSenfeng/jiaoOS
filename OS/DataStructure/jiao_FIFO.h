#ifndef __JIAO_FIFO_H__
#define __JIAO_FIFO_H__

#include "stm32f10x.h"
#include "./task/jiao_task.h"

#define FLAGS_OVERRUN		0x0001

struct FIFO8{
	uint8_t *buf;
	//��һ����ȡ��λ��,��һ��д���λ��,���������Ĵ�С,���е�λ��,��־
	uint32_t next_r, next_w, size, free, flags;
	struct TASK * task;
};

struct Event_Flog{
	struct FIFO8 System_Flags;
	uint8_t system_flags_buf[16];

};

struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;
};
void FIFO8_Init(struct FIFO8 *fifo, int size, unsigned char *buf, struct TASK *task);
int fifo8_put(struct FIFO8 *fifo, char data);
int FIFO8_Get(struct FIFO8 *fifo);
int FIFO8_Status(struct FIFO8 *fifo);
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

#endif
