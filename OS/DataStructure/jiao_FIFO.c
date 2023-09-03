#include "jiao_FIFO.h"
/**
  * @brief  ��ʼ��FIFO
  * @param  Ҫ��ʼ���Ľṹ��ָ��
  * @param  �ṹ��Ĵ�С
  * @param  ���ݴ洢������
  * @retval None
  */
void FIFO8_Init(struct FIFO8 *fifo, int size, unsigned char *buf, struct TASK *task)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;
	fifo->next_w = 0;
	fifo->next_r = 0;
	fifo->task = task; /* ������ص����� */
	
}
/**
  * @brief  ���һ������
  * @param  ���ݴ洢�Ľṹ��ָ��
  * @param  Ҫ���������
  * @retval �����Ƿ����,����Ļ�Ϊ-1,����Ϊ0
  */
int fifo8_put(struct FIFO8 *fifo, char data)
{
	if(fifo->free==0)
	{
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->next_w] = data;
	fifo->next_w++;
	if(fifo->next_w == fifo->size)
	{
		fifo->next_w = 0;
	}
	fifo->free--;
	if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* ����û������ */
			task_run(fifo->task, fifo->task->priority); /* �������������� */
			//printf("����");
		}
	}
	return 0;
}
/**
  * @brief  ��ȡһ������
  * @param  �����Ľṹ��
  * @retval �����Ƿ�Ϊ��,����Ļ�Ϊ-1,����Ϊ��ȡ��������
  */
int FIFO8_Get(struct FIFO8 *fifo)
{
	int data;
	if(fifo->free == fifo->size)
	{
		return -1;
	}
	data = fifo->buf[fifo->next_r];
	fifo->next_r++;
	if(fifo->next_r==fifo->size)
	{
		fifo->next_r=0;
	}
	fifo->free++;
	return data;
}
/**
  * @brief  ������ݵĶ���
  * @param  �����Ľṹ��
  * @retval �洢���ݵĸ���
  */
int FIFO8_Status(struct FIFO8 *fifo)
{
	return fifo->size - fifo->free;
}

/******************************************
+ ������32λ��FIFO, ����Ľ���ͬ��
*******************************************/



void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task)
/* FIFO��ʼ�� */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* ����Ϊfifo�Ĵ�С */
	fifo->flags = 0;
	fifo->p = 0; /* ����Ϊ��ʼ��λ�� */
	fifo->q = 0; /* ��ȡ��λ������Ϊ��ʼ��λ�� */
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data)
/* FIFO����һ������ */
{
	if (fifo->free == 0) {
		/* û�п�λ */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	return 0;
}

int fifo32_get(struct FIFO32 *fifo)
/* FIFO��ȡһ������ */
{
	int data;
	if (fifo->free == fifo->size) {
		/* û�п��Ի�ȡ������ */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32 *fifo)
/* ��ȡʣ��Ŀռ� */
{
	return fifo->size - fifo->free;
}


