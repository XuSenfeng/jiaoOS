#include "jiao_dasktop.h"


extern struct SHEET * Mouse_sht, * Windoes_sht;

//����ԭ������ɫ
extern uint16_t Old_Color[20*40];
//��ʼ��һ�����Ϊȫ�ֱ���
Mouse_Message_Def Mouse_def;
//�����ǲ���ϵͳʹ�õ���ɫ��RGB565��ʽ
uint16_t table_rgb565[17] = {
	0x0000,	/*  0:�\ */
	0xf800,	/*  1:���� */
	0x07e0,	/*  2:�� */
	0xffe0,	/*  3:�� */
	0x001f,	/*  4:�� */
	0xf817,	/*  5:�� */
	0x07ff,	/*  6:ǳ��*/
	0xffff,	/*  7:�� */
	0xc638,	/*  8:�� */
	0x8000,	/*  9:���� */
	0x0420,	/* 10:���v */
	0x8420,	/* 11:����ɫ */
	0x0010,	/* 12:���� */
	0x8010,	/* 13:���� */
	0x0430,	/* 14:����ɫ */
	0x8430,	/* 15:����ɫ */
	0x9999
};

/**
  * @brief  ����һ�������λ���һ����,ֱ������Ļ�ϻ���,��Ҫ���ڻ�������
  * @param  �����յ�
  * @param  ��ʼ��λ��x
  * @param  ��ʼ��λ��y
  * @param  ������λ��x
  * @param  ������λ��y
  * @retval None
  */
void boxfill8(unsigned char c, int x0, int y0, int x1, int y1)
{
	
	ILI9341_OpenWindow ( x0, y0, (x1-x0+1), (y1-y0+1) );
	ILI9341_FillColor ( (x1-x0+1)*(y1-y0+1) ,table_rgb565[c]);
}

/**
  * @brief  ����һ�������λ���һ����,��Ҫ������һ��ͼ��������л���
  * @param  �����յ�����
  * @param  ��ʼ��λ��x
  * @param  ��ʼ��λ��y
  * @param  ������λ��x
  * @param  ������λ��y
  * @retval None
  */
void boxfill_buf(uint16_t *buf, int x0, int y0, int width, int height)
{
	uint32_t i;
	ILI9341_OpenWindow ( x0, y0, width, height);
	
	ILI9341_Write_Cmd ( CMD_SetPixel );	
	for(i=0;i<height*width;i++)
	{
		ILI9341_Write_Data(buf[i]);
	}
}
/**
  * @brief  ��������ͼ��ı���,��һ��λ�õ���������������Ӧ�ĵ�ַ
  * @param  ��ʼ��λ��x
  * @param  ��ʼ��λ��y
  * @param  ������λ��x
  * @param  ������λ��y
  * @retval None
  */
void flish_Disply_fill8(int x0, int y0, int x1, int y1)
{
	
	uint16_t Color_Data[ILI9341_MORE_PIXEL], high, width, i;
	high = y1-y0+1;
	width = x1-x0+1;
	for(i=0;i<high;i++)
	{
		//��ȡ���ʼʱ�����ɫ
		ILI9341_Read_Datas(Color_Data, x0, y0+i, width, 1);
		//��������ÿһ����ɫ������λ, ���Լ����ʱ����Ҫ����2
		SPI_FLASH_BufferWrite((uint8_t *)Color_Data, (DASKTOP_SHEET_ADDR + (x0 + (y0+i) * ILI9341_MORE_PIXEL)*2), width*2);
		printf("д��%d\n", i);
	}
	return;
}

/**
  * @brief  ��������,��ʼ�����
  * @param  ��
  * @retval None
  */
void Draw_Dasktop(void)
{
	//����ĳ���, �Լ�Flash�����Ƿ��������ͼ����ڵı�־
	int xsize, ysize;
	uint32_t i;
	uint8_t Dasktop_Flash_flag;
	xsize =ILI9341_MORE_PIXEL;
	ysize = ILI9341_LESS_PIXEL;
	//��ȡFlash����ͼ���Ƿ񱣴�ĳ�ʼ����Ϣ
	SPI_FLASH_BufferRead(&Dasktop_Flash_flag,DASKTOP_SHEET_FLAG_ADDR,1);
	//ILI9341_GramScan(3);
	//��ʼ������
	boxfill8(COL8_008484,  0,         0,          xsize, ysize - 29);
	boxfill8(COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28);
	boxfill8(COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27);
	boxfill8(COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1);

	boxfill8(COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24);
	boxfill8(COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4);
	boxfill8(COL8_848484,  3,         ysize -  4, 59,         ysize -  4);
	boxfill8(COL8_848484, 59,         ysize - 23, 59,         ysize -  5);
	boxfill8(COL8_000000,  2,         ysize -  3, 59,         ysize -  3);
	boxfill8(COL8_000000, 60,         ysize - 24, 60,         ysize -  3);

	boxfill8(COL8_848484, xsize - 273, ysize - 24, xsize -  4, ysize - 24);
	boxfill8(COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
	boxfill8(COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
	boxfill8(COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);
	
	
	if((Dasktop_Flash_flag==0x32) & (~RELOAD_FLASH_SHEET))
	{
		printf("�Ѿ���ʼ�������ͼ��\n");
	}else
	{
		Dasktop_Flash_flag=0x32;
		printf("û�г�ʼ�������ͼ��, �������г�ʼ��, result = %#x\n", Dasktop_Flash_flag);
		SPI_FLASH_SectorErase(DASKTOP_SHEET_ADDR-256);
		SPI_FLASH_BufferWrite(&Dasktop_Flash_flag,DASKTOP_SHEET_FLAG_ADDR,1);
		for(i=0;i<1024;i++)
		{	
			//����Ӧ��λ�ý������
			SPI_FLASH_SectorErase(DASKTOP_SHEET_ADDR + i*SPI_FLASH_PageSize);
			printf("����%d\n", i);

		}
		flish_Disply_fill8(0, 0, ILI9341_MORE_PIXEL, ILI9341_LESS_PIXEL);
		
	}
}

/**
  * @brief  ��ʼ������ͼ��
  * @param  �����ʼ�����ݵĽṹ��
  * @param  ���Ƶ�ʱ�򱳾�����ɫ
  * @retval None
  */
void init_mouse_cursor8(uint8_t *mouse)
/* ��ʼ�����ṹ�� */
{

	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;
	//���������ͼ�γ�ʼ�����Ľṹ��
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = 16;
			}
		}
	}
	return;
}



/**
  * @brief  �������,ͬʱ�������λ�ýṹ��
  * @param  ���Ƶ�λ��x
  * @param  ���Ƶ�λ��y
  * @retval None
  */
void Draw_Mouse(uint16_t x, uint16_t y)
{
	if(y>(ILI9341_LESS_PIXEL-1))
		y=(ILI9341_LESS_PIXEL-1);
	if(x>(ILI9341_MORE_PIXEL-1))
		x=(ILI9341_MORE_PIXEL-1);
	if(x==Mouse_def.x && y==Mouse_def.y)
		return;
	Mouse_def.x_old = Mouse_def.x;
	Mouse_def.y_old = Mouse_def.y;
	Mouse_def.x = x;
	Mouse_def.y = y;
	sheet_slide(Mouse_sht, x, y);
	
}
/**
  * @brief  ��ȡһ�����������ͼ��,��Flash�����ȡ,�����Ҫ����ͼ�㻯֮��Ļ���
  * @param  ��ʼ��λ��x
  * @param  ��ʼ��λ��y
  * @param  ��ȡ�Ŀ�
  * @param  ��ȡ�ĸ�
  * @retval None
  */
void Get_Dasktop_Part(uint16_t * buf, uint16_t x, uint16_t y, uint16_t width, uint16_t high)
{
	uint16_t i;
	for(i=0;i<high;i++)
	{
		SPI_FLASH_BufferRead((uint8_t *)(&buf[i*width]), (DASKTOP_SHEET_ADDR + (x + (y+i)*ILI9341_MORE_PIXEL)*2), width*2);
	}

}

/**
  * @brief  ���һ��ͼ������,������������λ��,�����ǳ�����
  * @param  ͼ��Ŀ�
  * @param  ������ɫ
  * @param  ��ʼλ��,��������
  * @param  ĩβ��λ��,��������
  * @retval None
  */
void buf_fill8(uint8_t *buf, int xsize, uint8_t c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			buf[y * xsize + x] = c;
	}
	return;
}

/**
  * @brief  ��ĳһ��ͼ���������һ���ַ���
  * @param  ͼ���buf
  * @param  ͼ��Ŀ��
  * @param  ��ʼλ��,��������
  * @param  �ֵ���ɫ
  * @param  �ַ�����ָ��
  * @retval None
  */void putfonts8_asc(uint8_t * buf,int xsize,uint16_t x,uint16_t y, uint16_t color,char * title)
{
	//ȡ��ģ���� 
	uint8_t ucBuffer [ WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8 ];	
	uint8_t rowCount, bitCount;
	uint16_t usTemp; 	
	uint16_t usCh;
	
	while(* title != '\0')
	{
		usCh = * ( uint16_t * ) title;	
		usCh = ( usCh << 8 ) + ( usCh >> 8 );
		GetGBKCode ( ucBuffer, usCh);

		for ( rowCount = 0; rowCount < HEIGHT_CH_CHAR; rowCount++ )
		{
			/* ȡ�������ֽڵ����ݣ���lcd�ϼ���һ�����ֵ�һ�� */
			usTemp = ucBuffer [ rowCount * 2 ];
			usTemp = ( usTemp << 8 );
			usTemp |= ucBuffer [ rowCount * 2 + 1 ];
			
			for ( bitCount = 0; bitCount < WIDTH_CH_CHAR; bitCount ++ )
			{			
				if ( usTemp & ( 0x8000 >> bitCount ) )  //��λ��ǰ,�������ʾ��λ�� 
				  buf[x +bitCount + (y+rowCount)*xsize] = color;
			}		
		}
		x += WIDTH_CH_CHAR;
		title+=2;
	}
}







void make_window8(uint8_t *buf, int xsize, int ysize, char *title)
{
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	buf_fill8(buf, xsize, COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	buf_fill8(buf, xsize, COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	buf_fill8(buf, xsize, COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	buf_fill8(buf, xsize, COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	buf_fill8(buf, xsize, COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	buf_fill8(buf, xsize, COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
	buf_fill8(buf, xsize, COL8_C6C6C6, 2,         2,         xsize - 3, ysize - 3);
	buf_fill8(buf, xsize, COL8_000084, 3,         3,         xsize - 4, 20       );
	buf_fill8(buf, xsize, COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	buf_fill8(buf, xsize, COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
	putfonts8_asc(buf, xsize, 4, 4, COL8_FFFFFF, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

#if Jiao_Debug
/**
  * @brief  ���Ժ��������޸��������������Ƿ���в���
  * @param  ��
  * @retval None
  */
void test(void)
{
	uint16_t data[20];
	uint16_t i, j;
	uint16_t Color_Data[ILI9341_MORE_PIXEL];
	for(i=0; i<16; i++)
	{	
			boxfill8(i, i, 10, i, 10);
		
	}
	ILI9341_Read_Datas(data, 0,10, 16, 1);

	for(i=0; i<4; i++)
	{
		for(j=0;j<4;j++)
		{
			printf("cl%d= 0x%x\t", (i*4+j), data[i*4+j]);
		}
		printf("\n");
	}
	//��ʾ����
	ILI9341_DispString_EN_CH(0, 220, "jhy������");
	ILI9341_DisplayStringEx(0, 120, 20, 40, "jhy������", 0);
	for(i=0;i<0xffff;i++)
		for(j=0;j<0xff;j++);
	//����,��ȡFlash�����ͼ��, �������»���, ���д���Ƿ�ɹ�
	sheet_slide(Mouse_sht, 55, 50);
	for(i=0;i<0xffff;i++)
		for(j=0;j<0xff;j++);
	sheet_slide(Mouse_sht, 60, 50);

	for(i=0;i<0xffff;i++)
		for(j=0;j<0xff;j++);
	printf("���»���\n");
	for(i=0;i<ILI9341_LESS_PIXEL;i++)
	{
		
		SPI_FLASH_BufferRead((uint8_t *)Color_Data, (DASKTOP_SHEET_ADDR + i*ILI9341_MORE_PIXEL*2), (ILI9341_MORE_PIXEL*2));
		
		ILI9341_OpenWindow ( 0, i, ILI9341_MORE_PIXEL, 1 );
		ILI9341_Write_Cmd ( CMD_SetPixel );	
		
		for ( j = 0; j < ILI9341_MORE_PIXEL; j ++ )
			ILI9341_Write_Data ( Color_Data[j] );
		//
	}
	
	
}
#endif
