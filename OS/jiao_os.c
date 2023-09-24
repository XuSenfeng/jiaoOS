#include "jiao_os.h"
extern Mouse_Message_Def Mouse_def;
extern struct SHTCTL ctl;

struct Event_Flog EventFlog;
extern struct SHEET * Mouse_sht, * Windoes_sht, *Dasktop_sht;



struct SHEET_Event event1, event2, event3;
void event1_function()
{
	printf("event1\n");
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);
	if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
	{
		LED_BLUE;
		printf("\r\n 待机唤醒复位 \r\n");
	}
	else
	{
		LED_GREEN;
		printf("\r\n 非待机唤醒复位 \r\n");
	}
	LED_RED;	

	/*清除WU状态位*/
	PWR_ClearFlag (PWR_FLAG_WU);
	
	/* 使能WKUP引脚的唤醒功能 ，使能PA0*/
	PWR_WakeUpPinCmd (ENABLE);
	
	/* 进入待机模式 */
	PWR_EnterSTANDBYMode();
}
void event2_function()
{
	printf("event2\n");
}
void event3_function()
{
	printf("event3\n");
	sheet_updown(&ctl, Windoes_sht, -1);
}
//这个是保存窗口图层信息的数组
uint8_t buf_win[120*60];


/**
  * @brief  这个是操作系统初始化时候调用的函数
  * @param  None
  * @retval None
  */
void sheet_init(void)
{

	//初始化管理的结构体
	shtctl_init(ILI9341_MORE_PIXEL, ILI9341_LESS_PIXEL);
	//申请鼠标和一个桌面结构体, 桌面结构体实际上没有进行实际的初始化,只是用来在之后的时间检测的时候使用
	Dasktop_sht = sheet_alloc(&ctl);
	Mouse_sht = sheet_alloc(&ctl);
	//设置鼠标图层
	sheet_setbuf(Mouse_sht, Mouse_def.mouse, Mouse_def.Width, Mouse_def.High, 	0x9999);
	sheet_setbuf(Dasktop_sht, NULL, 0, 0, 0);

	//初始化鼠标的颜色
	Mouse_sht->vx0 = 120;
	Mouse_sht->vy0 = 160;
	//申请一个窗口图层
	Windoes_sht = sheet_alloc(&ctl);
	if(Windoes_sht==0)
	{
		printf("窗口图层申请失败");
	}
	sheet_setbuf(Windoes_sht, buf_win, 120, 60, 0x9999);
	//设置窗口的位置
	Windoes_sht->vx0 = 20;
	Windoes_sht->vy0 = 20;
	//绘制文本框
	make_textbox8(buf_win, 120, 8, 28, 104, 16, COL8_FFFFFF);
	//在文本框里面设置文字进行
	putfonts8_asc(buf_win, 120, 8, 28, COL8_008484, "测试");
	//在这时候没有桌面
	//设置这个图层比较低
	sheet_updown(&ctl, Dasktop_sht, 0);

	sheet_updown(&ctl, Windoes_sht, 1);
	//设置鼠标为最高层的图层
	sheet_updown(&ctl, Mouse_sht, MAX_SHEETS);
	/*********************/
	/******创建事件*******/
	/*********************/
	sheet_add_event(Dasktop_sht, &event1, 0, 210, 60, 240, event1_function);
	sheet_add_event(Windoes_sht, &event2, 0, 0, 120, 90, event2_function);
	sheet_add_event(Windoes_sht, &event3, 100, 0, 120, 20, event3_function);

}

/**
  * @brief  初始化硬件进行绘制桌面
  * @param  无
  * @retval None
  */
static void Hareware_Init(void)
{
	//初始化定时器
	TOUCH_TIM_Init();
	//初始化串口
	USART_Config();
	//初始化LED
	LED_GPIO_Config();
	printf("你好\n");
	//初始化屏幕
	ILI9341_Init();
	//初始化触摸屏
	XPT2046_Init();
	printf("初始化触摸屏, 同时初始化SPIFlash\n");
	//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
	Calibrate_or_Get_TouchParaWithFlash(3,TOUCH_SCREEN_CALIBRATE);
	printf("初始化屏幕,绘制桌面中\n");
	//初始化按键
	Key_GPIO_Config();
	//绘制桌面
	Draw_Dasktop();

}

/**
  * @brief  初始化使用到的参数
  * @param  无
  * @retval None
  */
static void System_data_Init(void)
{
	//初始化鼠标的数据
	Mouse_def.Width = MOUSE_WIDTH;
	Mouse_def.High = MOUSE_HIGH;
	//初始化鼠标的数组
	init_mouse_cursor8(Mouse_def.mouse);
	//初始化一个窗口的画面
	make_window8(buf_win, 120, 60, "焦", 1);
	//初始化图层
	sheet_init();
	//标志位的初始化,主要用于时钟,触摸位置以及按键
	FIFO8_Init(&EventFlog.System_Flags, 16, EventFlog.system_flags_buf, 0);
	//设置时间计数器
	Timer_init();
}


void JIAO_OS_Init(void)
{
	Hareware_Init();
	System_data_Init();
}






