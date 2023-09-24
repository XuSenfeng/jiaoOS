#include "jiao_sheet.h"


#define SHEET_USE		1
//ͼ����ƽṹ��
struct SHEET * Mouse_sht, * Windoes_sht, *Dasktop_sht;
//extern Mouse_Message_Def Mouse_def;
extern uint8_t buf_win[120*52];
//ͼ�����ģ��
struct SHTCTL ctl;

/**
  * @brief  ��ʼ��ͼ����ƽṹ��
  * @param  ��Ļ�ĳ��Ϳ�
  * @retval None
  */
void shtctl_init(int xsize, int ysize)
{
	int i;
	ctl.xsize = xsize;
	ctl.ysize = ysize;
	ctl.top = -1; /* û��ʹ�õ�ͼ�� */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl.sheets0[i].flags = 0; /* ���е�ͼ����Ϊδʹ�� */
		ctl.sheets0[i].event = NULL;		//��ʼ�����е��¼�Ϊ��
	}

}
/**
  * @brief  ����һ���յ�ͼ��
  * @param  ͼ����ƼĴ�����ָ��
  * @retval None
  */
struct SHEET *sheet_alloc(struct SHTCTL *ctl)
{
	struct SHEET *sht;
	int i;
	//���һ�����е�ͼ��
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE; /* ��־λ���� */
			sht->height = -1; /* ��ʾʹ���� */
			return sht;
		}
	}
	return 0;	/* û������ɹ� */
}
/**
  * @brief  ����һ��ͼ�������
  * @param  ͼ���ָ
  * @param  ����ͼ�������
  * @param  ͼ��ĳ�
  * @param  ͼ��Ŀ�
  * @param  ͸������ɫ
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
  * @brief  ˢ��ĳһ������
  * @param  ��
  * @retval None
  */
void sheet_refreshsub(int vx0, int vy0, int vx1, int vy1)
{
	int h, bx, by, bx0, by0, bx1, by1, vy, vx;
	uint16_t width, high, c, width_x, high_x, i, j;
	struct SHEET *sht;
	uint16_t *temp_buf;
	uint8_t  *buf;
	/* refreh�ķ�Χ��������ʾ���ķ�Χ */
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl.xsize) { vx1 = ctl.xsize; }
	if (vy1 > ctl.ysize) { vy1 = ctl.ysize; }

	//����һ����ʱ��ͼ��
	width =vx1-vx0;
	high = vy1-vy0;
	//����һ��Ҫˢ�µ�ͼ��Ƚϴ��ʱ��Ҫ���еĴ���
	if((width*high) > (16*16+1))
	{
		//printf("ͼ�����\n\n");
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
				//�������е�ͼ��
				sht = ctl.sheets[h];
				//���ͼ����Ϣ
				buf = sht->buf;
				/* ��ȡ��ʱͼ�����λ�������ͼ���λ�� */
				bx0 = vx0 - sht->vx0;
				by0 = vy0 - sht->vy0;
				bx1 = vx1 - sht->vx0;
				by1 = vy1 - sht->vy0;	
				//printf("bx0 = %d %d", bx0, vx0);
				//��������λ���Ǹ�����ʱ����������
				if (bx0 < 0) { bx0 = 0;}
				if (by0 < 0) { by0 = 0;}
				if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
				if (by1 > sht->bysize) { by1 = sht->bysize; }
				//��������ʱͼ�������

				//��ʱͼ�������Ͻ�
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
			printf("����ʧ��\n");
	}
}
	
/**
  * @brief  ͼ��λ�õı仯,��ͼ��ĻḲ�ǵ�ͼ��,�ı�֮���ˢ��ͼ��ı��λ��
  * @param  ͼ�����ģ��
  * @param  ˢ�µ�ͼ��
  * @param  ͼ����¸߶�
  * @retval None
  */
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height)
{
	int h, old = sht->height; /* ����ı�֮ǰ�ĸ߶� */

	/* �Ը߶Ƚ������� */
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* �������ڵĸ߶� */

	/* ����������з��� */
	if (old > height) {	/* ����ǰ��ͼ��� */
		if (height >= 0) {
			/* ���ڵ�ͼ��������ʾ */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* ֮������ʾ��ʾ */
			if (ctl->top > old) {
				/* ֮ǰ��ͼ�����滹�б��ͼ�� */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* �߶��½� */
		}
		sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	} else if (old < height) {	/* ��֮ǰ��ͼ��� */
		if (old >= 0) {
			/* ֮ǰ��ͼ���Ѿ���ʾ�� */
			//printf("\n%d %d\n", old, height);

			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				if(ctl->sheets[h]!=0){
					//��ʱ����ĵ�ͼ��Ϊ��߲�,λ�Ƶ�ʱ����ߵĵط�Ϊ��
					ctl->sheets[h]->height = h;
					
				}else
				{
					height--;
				}

			}
			ctl->sheets[height] = sht;

		} else {	/* ֮ǰû����ʾ */
			/* ���Ȱ������ͼ������ */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; /* ����ͼ���������һ */
		}

		sheet_refreshsub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
	return;
}

/**
  * @brief  ˢ��ĳһ��ͼ������λ��
  * @param  ��
  * @retval None
  */
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) { /* �⤷���ʾ�Фʤ顢�¤����¤����������ؤäƻ�����褭ֱ�� */
		sheet_refreshsub(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
	}
	return;
}
/**
  * @brief  ͼ���ƶ�
  * @param  ��
  * @retval None
  */
void sheet_slide(struct SHEET *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { /* ͼ��ĸ߶�Ϊ��ʾ��ͼ�� */
		sheet_refreshsub(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);
	}
	return;
}
/**
  * @brief  �ͷ�һ��ͼ��
  * @param  ͼ�������ָ��
  * @param  ĳһ��ͼ���ָ��
  * @retval None
  */
void sheet_free(struct SHTCTL *ctl, struct SHEET *sht)
{
	if (sht->height >= 0) {
		sheet_updown(ctl, sht, -1); /* ������ʹ�õ�ͼ����ȥ�� */
	}
	sht->flags = 0; /* δʹ�� */
	return;
}

/***********************************����ͼ���¼�********************************************/
/**
  * @brief  ��ȡ��������λ����ʾ��ͼ��
  * @param  ָ��ľ�������
  * @retval ����λ�õ�ͼ��ĸ߶�
  */
int Get_Top_Sheet(uint16_t vx0, uint16_t vy0)
{
	struct SHEET *sht;
	int bx0, by0, h, heigh=0;
	uint8_t  *buf, c;
	
	for (h = 0; h <= ctl.top-1; h++) {
		//�������е�ͼ��
		sht = ctl.sheets[h];
		//���ͼ����Ϣ
		buf = sht->buf;
		/* ��ȡ��ʱͼ�����λ�������ͼ���λ�� */
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		//printf("bx0 = %d %d", bx0, vx0);
		//��������λ���Ǹ�����ʱ����������
		if (bx0 < 0) { continue;}
		if (by0 < 0) { continue;}
		if (bx0 > sht->bxsize) { continue; }
		if (by0 > sht->bysize) { continue; }
		//��������ʱͼ�������

		//��ʱͼ�������Ͻ�
		//��ȡ��Ӧλ�õ���ɫ
		c = table_rgb565[buf[by0 * sht->bxsize + bx0]];
		if (c != sht->col_inv) {
			heigh = h;
		}
		
	}
	return heigh;

}
/**
  * @brief  ��ĳһ��ͼ�����һ���¼�,����������¼����
  * @param  �����¼���ͼ��
  * @param  Ҫ��ӵ��¼�
  * @param  �¼�����ʱ�򴥷��ķ�Χ
  * @param  �¼�����֮�����õĴ�����
  * @retval ��
  */
void sheet_add_event(struct SHEET *sht, struct SHEET_Event *event, 
					uint16_t x0, uint16_t y0,
					uint16_t x1, uint16_t y1,
					EventFunction_t func)
{
	struct SHEET_Event * now_event;
	//��ʼ���¼�
	event->x0 = x0;
	event->x1 = x1;
	event->y0 = y0;
	event->y1 = y1;
	event->event_function = func;
	
	now_event = sht->event;
	//�������һ��ͼ���ʼ��ʱ��û���¼�
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
  * @brief  ��ĳһ��ͼ������ɾ��ĳһ���¼�
  * @param  �����¼���ͼ��
  * @param  Ҫɾ�����¼�
  * @retval ��
  */
void sheet_event_del(struct SHEET *sht, struct SHEET_Event *event){
	struct SHEET_Event * now_event;
	now_event = sht->event;
	if(now_event == event)
	{
		sht->event = now_event->next_event;
		return;
	}
	//���ǵ�һ���¼���ʱ��
	while(now_event->next_event!=NULL)
	{
		//��û�е����һ���¼�
		if(now_event->next_event == event)
		{
			now_event->next_event = now_event->next_event->next_event;
		}
	}
}
/**
  * @brief  ���ĳһ��λ�õ��¼������д���
  * @param  Ҫ����λ��
  * @retval ����λ�õ�ͼ��ĸ߶�
  */
void sheet_event_check_run(uint16_t x, uint16_t y)
{
	int h, bx0, by0;
	struct SHEET *sht;
	struct SHEET_Event * now_event;

	h = Get_Top_Sheet(x, y);
	sht = ctl.sheets[h];
	//����ͼ������λ��
	bx0 = x - sht->vx0;
	by0 = y - sht->vy0;

	now_event = sht->event;
	while(now_event != NULL){		
		if(bx0>=now_event->x0 && bx0<now_event->x1 && by0>=now_event->y0 && by0<now_event->y1)
		{
			//�¼�����
			now_event->event_function();
		}
		//��ȡ��һ���¼�
		now_event = now_event->next_event;
	}
}









