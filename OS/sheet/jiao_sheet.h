#ifndef __JIAO_SHEET_H__
#define __JIAO_SHEET_H__

#define MAX_SHEETS		5
#include "stm32f10x.h"
#include "jiao_dasktop.h"
#include <stdlib.h>
#include <stdio.h>

struct SHEET_Event{
	//��¼�¼���λ��,�����ͼ���������Ե�λ��
	uint16_t x0, y0, x1, y1;
	//�����ʱ�䵽���ʱ��Ҫ����ĺ���
	void *event_function;
	//����һ��ָ��ָ����һ���¼�
	struct SHEET_Event *next_event;
};

struct SHEET {
	uint8_t *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHEET_Event *event;
	
};
struct SHTCTL {
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

void sheet_refreshsub(int vx0, int vy0, int vx1, int vy1);
//void sheet_init(void);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);
void shtctl_init(int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, uint8_t *buf, int xsize, int ysize, int col_inv);

#endif
