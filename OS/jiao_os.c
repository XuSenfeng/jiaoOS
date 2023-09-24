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
		printf("\r\n �������Ѹ�λ \r\n");
	}
	else
	{
		LED_GREEN;
		printf("\r\n �Ǵ������Ѹ�λ \r\n");
	}
	LED_RED;	

	/*���WU״̬λ*/
	PWR_ClearFlag (PWR_FLAG_WU);
	
	/* ʹ��WKUP���ŵĻ��ѹ��� ��ʹ��PA0*/
	PWR_WakeUpPinCmd (ENABLE);
	
	/* �������ģʽ */
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
//����Ǳ��洰��ͼ����Ϣ������
uint8_t buf_win[120*60];


/**
  * @brief  ����ǲ���ϵͳ��ʼ��ʱ����õĺ���
  * @param  None
  * @retval None
  */
void sheet_init(void)
{

	//��ʼ������Ľṹ��
	shtctl_init(ILI9341_MORE_PIXEL, ILI9341_LESS_PIXEL);
	//��������һ������ṹ��, ����ṹ��ʵ����û�н���ʵ�ʵĳ�ʼ��,ֻ��������֮���ʱ�����ʱ��ʹ��
	Dasktop_sht = sheet_alloc(&ctl);
	Mouse_sht = sheet_alloc(&ctl);
	//�������ͼ��
	sheet_setbuf(Mouse_sht, Mouse_def.mouse, Mouse_def.Width, Mouse_def.High, 	0x9999);
	sheet_setbuf(Dasktop_sht, NULL, 0, 0, 0);

	//��ʼ��������ɫ
	Mouse_sht->vx0 = 120;
	Mouse_sht->vy0 = 160;
	//����һ������ͼ��
	Windoes_sht = sheet_alloc(&ctl);
	if(Windoes_sht==0)
	{
		printf("����ͼ������ʧ��");
	}
	sheet_setbuf(Windoes_sht, buf_win, 120, 60, 0x9999);
	//���ô��ڵ�λ��
	Windoes_sht->vx0 = 20;
	Windoes_sht->vy0 = 20;
	//�����ı���
	make_textbox8(buf_win, 120, 8, 28, 104, 16, COL8_FFFFFF);
	//���ı��������������ֽ���
	putfonts8_asc(buf_win, 120, 8, 28, COL8_008484, "����");
	//����ʱ��û������
	//�������ͼ��Ƚϵ�
	sheet_updown(&ctl, Dasktop_sht, 0);

	sheet_updown(&ctl, Windoes_sht, 1);
	//�������Ϊ��߲��ͼ��
	sheet_updown(&ctl, Mouse_sht, MAX_SHEETS);
	/*********************/
	/******�����¼�*******/
	/*********************/
	sheet_add_event(Dasktop_sht, &event1, 0, 210, 60, 240, event1_function);
	sheet_add_event(Windoes_sht, &event2, 0, 0, 120, 90, event2_function);
	sheet_add_event(Windoes_sht, &event3, 100, 0, 120, 20, event3_function);

}

/**
  * @brief  ��ʼ��Ӳ�����л�������
  * @param  ��
  * @retval None
  */
static void Hareware_Init(void)
{
	//��ʼ����ʱ��
	TOUCH_TIM_Init();
	//��ʼ������
	USART_Config();
	//��ʼ��LED
	LED_GPIO_Config();
	printf("���\n");
	//��ʼ����Ļ
	ILI9341_Init();
	//��ʼ��������
	XPT2046_Init();
	printf("��ʼ��������, ͬʱ��ʼ��SPIFlash\n");
	//��FLASH���ȡУ����������FLASH�޲�������ʹ��ģʽ3����У��
	Calibrate_or_Get_TouchParaWithFlash(3,TOUCH_SCREEN_CALIBRATE);
	printf("��ʼ����Ļ,����������\n");
	//��ʼ������
	Key_GPIO_Config();
	//��������
	Draw_Dasktop();

}

/**
  * @brief  ��ʼ��ʹ�õ��Ĳ���
  * @param  ��
  * @retval None
  */
static void System_data_Init(void)
{
	//��ʼ����������
	Mouse_def.Width = MOUSE_WIDTH;
	Mouse_def.High = MOUSE_HIGH;
	//��ʼ����������
	init_mouse_cursor8(Mouse_def.mouse);
	//��ʼ��һ�����ڵĻ���
	make_window8(buf_win, 120, 60, "��", 1);
	//��ʼ��ͼ��
	sheet_init();
	//��־λ�ĳ�ʼ��,��Ҫ����ʱ��,����λ���Լ�����
	FIFO8_Init(&EventFlog.System_Flags, 16, EventFlog.system_flags_buf, 0);
	//����ʱ�������
	Timer_init();
}


void JIAO_OS_Init(void)
{
	Hareware_Init();
	System_data_Init();
}






