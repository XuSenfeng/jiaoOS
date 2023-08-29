#ifndef __JIAO_DASKTOP_H__
#define __JIAO_DASKTOP_H__
#include "jiaoConfig.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "bsp_spi_flash.h"
#include "jiao_sheet.h"
#define MOUSE_WIDTH	16
#define MOUSE_HIGH	16



#define 							DASKTOP_SHEET_FLAG_ADDR						(512*4096-1)
#define								DASKTOP_SHEET_ADDR							(512*4096)



/*鼠标结构体*/
struct Mouse_Message
{
	//鼠标的大小
	uint8_t Width, High;
	//鼠标之前的位置
	uint16_t x_old;
	uint16_t y_old;
	//鼠标现在的位置
	uint16_t x;
	uint16_t y;
	//保存鼠标的位置的颜色信息, 用于鼠标移动之后的设置
	uint8_t  mouse[MOUSE_WIDTH*MOUSE_HIGH];
	
};
typedef struct Mouse_Message Mouse_Message_Def;


void test(void);
void Draw_Dasktop(void);
void Draw_Mouse(uint16_t x, uint16_t y);
void init_mouse_cursor8(uint8_t *mouse);
void Get_Dasktop_Part(uint16_t * buf, uint16_t x, uint16_t y, uint16_t width, uint16_t high);
void boxfill_buf(uint16_t *buf, int x0, int y0, int width, int height);
void make_window8(uint8_t *buf, int xsize, int ysize, char *title);

#endif
