#ifndef __JIAO_SHEET_H__
#define __JIAO_SHEET_H__

#define MAX_SHEETS		5
#include "stm32f10x.h"
#include "jiao_dasktop.h"
#include <stdlib.h>
#include <stdio.h>
typedef void (*EventFunction_t)(void);

struct SHEET_Event{
	//记录事件的位置,这个是图层里面的相对的位置
	uint16_t x0, y0, x1, y1;
	//这个是时间到达的时候要处理的函数
	EventFunction_t event_function;
	//这是一个指针指向下一个事件
	struct SHEET_Event *next_event;
};

struct SHEET {
	uint8_t *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHEET_Event * event;		//添加一个事件的指针
	
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
int Get_Top_Sheet(uint16_t x, uint16_t y);
void sheet_add_event(struct SHEET *sht, struct SHEET_Event *event, 
					uint16_t x0, uint16_t y0,
					uint16_t x1, uint16_t y1,
					EventFunction_t func);
void sheet_event_del(struct SHEET *sht, struct SHEET_Event *event);
void sheet_event_check_run(uint16_t x, uint16_t y);

#endif
