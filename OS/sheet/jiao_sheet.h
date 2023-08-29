#ifndef __JIAO_SHEET_H__
#define __JIAO_SHEET_H__

#define MAX_SHEETS		5
#include "stm32f10x.h"
#include "jiao_dasktop.h"
#include <stdlib.h>
struct SHEET {
	uint8_t *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	
};
struct SHTCTL {
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

void sheet_refreshsub(int vx0, int vy0, int vx1, int vy1);
void sheet_init(void);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height);

#endif
