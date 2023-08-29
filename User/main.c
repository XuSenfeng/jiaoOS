#include "jiao_os.h"
extern struct Event_Flog EventFlog;


int main()
{
#if USE_DASKTOP_MODE
	int i;
#endif

	JIAO_OS_Init();

#if Jiao_Debug

	printf("**********************************\n\n");
	test();
#endif
	
#if USE_TASK_MODE
	//使用任务模式
	Task_main();
#endif
	
#if USE_OF_MOUSE
	//使用桌面模式并且选择绘制鼠标
	Draw_Mouse(160, 120);
#endif

#if USE_DASKTOP_MODE
	//使用桌面模式,对各种事件进行检查
	while(1){
		//检测触控屏按压事件
		if(FIFO8_Status(&EventFlog.System_Flags))
		{
			i = FIFO8_Get(&EventFlog.System_Flags);
			if(i==1 || i==2)
			{
				//处理的事件是触摸屏
				XPT2046_TouchEvenHandler(i);
			}else if(i>2 && i<7){
				//处理的事件是按键
				Key_TouchEventHandler(i);
			}else{
				//处理的事件是定时器
				Time_OutEventHandler(i);
			}
		}	

		__WFI();	//WFI指令进入睡眠
	}
#endif	
}







