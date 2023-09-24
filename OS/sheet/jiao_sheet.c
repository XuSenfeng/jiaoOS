#include "jiao_sheet.h"


#define SHEET_USE		1
//图层控制结构体
struct SHEET * Mouse_sht, * Windoes_sht, *Dasktop_sht;
//extern Mouse_Message_Def Mouse_def;
extern uint8_t buf_win[120*52];
//图层控制模块
struct SHTCTL ctl;

/**
  * @brief  初始化图层控制结构体
  * @param  屏幕的长和宽
  * @retval None
  */
void shtctl_init(int xsize, int ysize)
{
	int i;
	ctl.xsize = xsize;
	ctl.ysize = ysize;
	ctl.top = -1; /* 没有使用的图层 */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl.sheets0[i].flags = 0; /* 所有的图层标记为未使用 */
		ctl.sheets0[i].event = NULL;		//初始化所有的事件为空
	}

}
/**
  * @brief  申请一个空的图层
  * @param  图层控制寄存器的指针
  * @retval None
  */
struct SHEET *sheet_alloc(struct SHTCTL *ctl)
{
	struct SHEET *sht;
	int i;
	//获得一个空闲的图层
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE; /* 标志位设置 */
			sht->height = -1; /* 表示使用中 */
			return sht;
		}
	}
	return 0;	/* 没有申请成功 */
}
/**
  * @brief  设置一个图层的属性
  * @param  图层的指
  * @param  保存图像的数组
  * @param  图像的长
  * @param  图像的宽
  * @param  透明的颜色
  * @retval None
  */
void sheet_setbuf(struct SHEET *sht, uint8_t *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}
/**
  * @brief  刷新某一块区域
  * @param  无
  * @retval None
  */
void sheet_refreshsub(int vx0, int vy0, int vx1, int vy1)
{
	int h, bx, by, bx0, by0, bx1, by1, vy, vx;
	uint16_t width, high, c, width_x, high_x, i, j;
	struct SHEET *sht;
	uint16_t *temp_buf;
	uint8_t  *buf;
	/* refreh的范围超过了显示器的范围 */
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl.xsize) { vx1 = ctl.xsize; }
	if (vy1 > ctl.ysize) { vy1 = ctl.ysize; }

	//设置一个临时的图层
	width =vx1-vx0;
	high = vy1-vy0;
	//这是一个要刷新的图层比较大的时候要进行的处理
	if((width*high) > (16*16+1))
	{
		//printf("图层过大\n\n");
		width_x = width/16+1;
		high_x = high/16+1;

		//printf("width_x = %d, high_x = %d\n", width_x, high_x);
		
		for(i=0;i<high_x;i++){
			for(j=0;j<width_x;j++)
			{
				sheet_refreshsub(vx0+j*16, vy0+i*16, vx0+(j+1)*16, vy0+(i+1)*16);
			}
		}
		
	}else{
		temp_buf = malloc(width * high *2);
		if(temp_buf != NULL)
		{
			Get_Dasktop_Part(temp_buf, vx0, vy0, width, high);
			for (h = 0; h <= ctl.top; h++) {
				//遍历所有的图层
				sht = ctl.sheets[h];
				//获得图层信息
				buf = sht->buf;
				/* 获取临时图层更新位置相对于图层的位置 */
				bx0 = vx0 - sht->vx0;
				by0 = vy0 - sht->vy0;
				bx1 = vx1 - sht->vx0;
				by1 = vy1 - sht->vy0;	
				//printf("bx0 = %d %d", bx0, vx0);
				//算出来相对位置是负数的时候会出现问题
				if (bx0 < 0) { bx0 = 0;}
				if (by0 < 0) { by0 = 0;}
				if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
				if (by1 > sht->bysize) { by1 = sht->bysize; }
				//首先是临时图层在左边

				//临时图层在左上角
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					for (bx = bx0; bx < bx1; bx++) {
						vx = sht->vx0 + bx;
						c = table_rgb565[buf[by * sht->bxsize + bx]];
						if (c != sht->col_inv) {
							temp_buf[(vy-vy0) * width + vx-vx0] = c;
						}
					}
				}
				
			}

			boxfill_buf(temp_buf, vx0, vy0, width, high );
			free(temp_buf);
		}
		else
			printf("申请失败\n");
	}
}
	
/**
  * @brief  图层位置的变化,高图层的会覆盖低图层,改变之后会刷新图层改变的位置
  * @param  图层控制模块
  * @param  刷新的图层
  * @param  图层的新高度
  * @retval None
  */
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height)
{
	int h, old = sht->height; /* 保存改变之前的高度 */

	/* 对高度进行修正 */
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* 设置现在的高度 */

	/* 根据情况进行分析 */
	if (old > height) {	/* 比以前的图层低 */
		if (height >= 0) {
			/* 现在的图层依旧显示 */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* 之后不再显示显示 */
			if (ctl->top > old) {
				/* 之前的图层上面还有别的图层 */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* 高度下降 */
		}
		sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	} else if (old < height) {	/* 比之前的图层高 */
		if (old >= 0) {
			/* 之前的图层已经显示了 */
			//printf("\n%d %d\n", old, height);

			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				if(ctl->sheets[h]!=0){
					//这时候更改的图层为最高层,位移的时候最高的地方为空
					ctl->sheets[h]->height = h;
					
				}else
				{
					height--;
				}

			}
			ctl->sheets[height] = sht;

		} else {	/* 之前没有显示 */
			/* 首先把上面的图层上移 */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; /* 所有图层的数量加一 */
		}

		sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
	return;
}

/**
  * @brief  刷新某一个图层的相对位置
  * @param  无
  * @retval None
  */
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) { /* もしも表示中なら、新しい下じきの情報に沿って画面を描き直す */
		sheet_refreshsub(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
	}
	return;
}
/**
  * @brief  图层移动
  * @param  无
  * @retval None
  */
void sheet_slide(struct SHEET *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { /* 图层的高度为显示的图层 */
		sheet_refreshsub(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
	return;
}
/**
  * @brief  释放一个图层
  * @param  图层控制器指针
  * @param  某一个图层的指针
  * @retval None
  */
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht)
{
	if (sht->height >= 0) {
		sheet_updown(ctl, sht, -1); /* 从正在使用的图层中去处 */
	}
	sht->flags = 0; /* 未使用 */
	return;
}

/***********************************处理图层事件********************************************/
/**
  * @brief  获取按键所在位置显示的图层
  * @param  指针的绝对坐标
  * @retval 所在位置的图层的高度
  */
int Get_Top_Sheet(uint16_t vx0, uint16_t vy0)
{
	struct SHEET *sht;
	int bx0, by0, h, heigh=0;
	uint8_t  *buf, c;
	
	for (h = 0; h <= ctl.top-1; h++) {
		//遍历所有的图层
		sht = ctl.sheets[h];
		//获得图层信息
		buf = sht->buf;
		/* 获取临时图层更新位置相对于图层的位置 */
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		//printf("bx0 = %d %d", bx0, vx0);
		//算出来相对位置是负数的时候会出现问题
		if (bx0 < 0) { continue;}
		if (by0 < 0) { continue;}
		if (bx0 > sht->bxsize) { continue; }
		if (by0 > sht->bysize) { continue; }
		//首先是临时图层在左边

		//临时图层在左上角
		//获取对应位置的颜色
		c = table_rgb565[buf[by0 * sht->bxsize + bx0]];
		if (c != sht->col_inv) {
			heigh = h;
		}
		
	}
	return heigh;

}
/**
  * @brief  给某一个图层添加一个事件,会加载所有事件最后
  * @param  创建事件的图层
  * @param  要添加的事件
  * @param  事件产生时候触发的范围
  * @param  事件产生之后会调用的处理函数
  * @retval 无
  */
void sheet_add_event(struct SHEET *sht, struct SHEET_Event *event, 
					uint16_t x0, uint16_t y0,
					uint16_t x1, uint16_t y1,
					EventFunction_t func)
{
	struct SHEET_Event * now_event;
	//初始化事件
	event->x0 = x0;
	event->x1 = x1;
	event->y0 = y0;
	event->y1 = y1;
	event->event_function = func;
	
	now_event = sht->event;
	//这个是这一个图层初始的时候没有事件
	if(now_event == NULL)
	{
		sht->event = event;
		return;
	}
	
	while(now_event->next_event != NULL)
	{
		now_event = now_event->next_event;
	}
	now_event->next_event = event;
}
/**
  * @brief  从某一个图层里面删除某一个事件
  * @param  创建事件的图层
  * @param  要删除的事件
  * @retval 无
  */
void sheet_event_del(struct SHEET *sht, struct SHEET_Event *event){
	struct SHEET_Event * now_event;
	now_event = sht->event;
	if(now_event == event)
	{
		sht->event = now_event->next_event;
		return;
	}
	//不是第一个事件的时候
	while(now_event->next_event!=NULL)
	{
		//还没有到最后一个事件
		if(now_event->next_event == event)
		{
			now_event->next_event = now_event->next_event->next_event;
		}
	}
}
/**
  * @brief  检测某一个位置的事件并进行处理
  * @param  要检测的位置
  * @retval 所在位置的图层的高度
  */
void sheet_event_check_run(uint16_t x, uint16_t y)
{
	int h, bx0, by0;
	struct SHEET *sht;
	struct SHEET_Event * now_event;

	h = Get_Top_Sheet(x, y);
	sht = ctl.sheets[h];
	//计算图层的相对位置
	bx0 = x - sht->vx0;
	by0 = y - sht->vy0;

	now_event = sht->event;
	while(now_event != NULL){		
		if(bx0>=now_event->x0 && bx0<now_event->x1 && by0>=now_event->y0 && by0<now_event->y1)
		{
			//事件成立
			now_event->event_function();
		}
		//获取下一个事件
		now_event = now_event->next_event;
	}
}









