#include "jiao_FIFO.h"
/**
  * @brief  初始化FIFO
  * @param  要初始化的结构体指针
  * @param  结构体的大小
  * @param  数据存储的数组
  * @retval None
  */
void FIFO8_Init(struct FIFO8 *fifo, int size, unsigned char *buf)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;
	fifo->next_w = 0;
	fifo->next_r = 0;
	
}
/**
  * @brief  存放一个数据
  * @param  数据存储的结构体指针
  * @param  要保存的数据
  * @retval 数据是否溢出,溢出的话为-1,否则为0
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
	return 0;
}
/**
  * @brief  获取一个数据
  * @param  操作的结构体
  * @retval 数据是否为空,溢出的话为-1,否则为获取到的数据
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
  * @brief  获得数据的多少
  * @param  操作的结构体
  * @retval 存储数据的个数
  */
int FIFO8_Status(struct FIFO8 *fifo)
{
	return fifo->size - fifo->free;
}

/******************************************
+ 以下是32位的FIFO, 具体的介绍同上
*******************************************/



void fifo32_init(struct FIFO32 *fifo, int size, int *buf)
/* FIFO初始化 */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 设置为fifo的大小 */
	fifo->flags = 0;
	fifo->p = 0; /* 设置为初始的位置 */
	fifo->q = 0; /* 读取的位置设置为起始的位置 */
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data)
/* FIFO放入一个数据 */
{
	if (fifo->free == 0) {
		/* 没有空位 */
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
/* FIFO获取一个数字 */
{
	int data;
	if (fifo->free == fifo->size) {
		/* 没有可以获取的数字 */
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
/* 获取剩余的空间 */
{
	return fifo->size - fifo->free;
}


