#ifndef __JIAO__Config_H__
#define __JIAO__Config_H__
//调试使用的内容
#define Jiao_Debug				0

/*是否强制更新Flash里面的图层*/
/*
+ 由于内存大小比较小,所以将桌面的图层存放在Flash中,用于之后的图层刷新等
*/
#define RELOAD_FLASH_SHEET		0
/* 是否使用鼠标 */
#define USE_OF_MOUSE			1



/****************模式选择,必须选择一个***************/

/*
+ 使用任务模式,优先级高于桌面
+ 在Task_main()进行任务的申请,之后进入任务之中
+ 使用时钟进行切换
*/
#define USE_TASK_MODE			0

//使用桌面模式,需要将TASK模式设置为关闭
#define USE_DASKTOP_MODE		1




#endif
