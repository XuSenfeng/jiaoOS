# my-stm32OS

/******************* 点个 ⭐ 吧 🙏 *******************/

## 视频介绍(1.0版本)

[[开源\]基于野火指南者开发板，轻松构建图形化系统！_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1Ep4y1N7UC/?spm_id_from=333.999.0.0&vd_source=3771cc8df803eed7244034a762706c24)



## 作品简介

记录手写stm32相关操作系统, 主要使野火指南者开发板, 参考30天自制操作系统以及FreeRTOS, 文件使用固件库开发

操作体统为图形化操作系统, 使用汇编以及C语言进行开发, 持续更新中,之后会优化任务切换,图层显示等,并开发板上外设的使用

使用软件, keil5

项目地址[XuSenfeng/jiaoOS: 一个基于stm32f103的图形化操作系统 (github.com)](https://github.com/XuSenfeng/jiaoOS)

作者: XvSenfeng, 1458612070@qq.com, 首都师范大学23级菜狗👀

## 文件分类

+ jiaoConfig.h 配置文件
+ jiao_daktop.c文件, 保存有桌面相关的绘制函数, 以及将桌面信息存储到flash等的函数
+ jiao_os.c 系统先关函数, 主要进行系统的初始化
+ jiao_FIFO.c, 数据类型FIFO, 主要用于存储各种标志位
+ jiao_sheet.c, 图层实现相关函数, 默认最高为五层, 可在头文件进行更改
+ jiao_list.c, 数列相关的函数
+ jiao_task.c, 任务切换相关的函数
+ jiao_time.c, 时钟相关函数, 时钟主要用于触摸屏软件延时, 以及任务切换等的实现, 定时时钟最多默认申请5个

## 主要模式

通过配置文件更改可以使用任务模式以及桌面模式

1. 任务模式默认情况下将桌面的运行设置为其中一个任务, 但是会占用大量的运算, 实时性较高的时候建议将桌面相关的代码删除

2. 桌面模式, 可以适用于用户化界面开发

## 任务切换

~~函数的具体使用方法参照实际工程里面的注释, 此文件不再进行具体介绍~~

模式的选择参照文件jiaoConfig.h

### 使用方法

参见jiao_task.c的Task_main函数

首先初始化任务切换的列表, 这时候会返回一个函数结构体指针, 指针用于之后的控制, 最少要有一个函数在运行, 之后初始化任务, 开启任务运行, 运行的函数可以使用函数关闭运行状态, 用于节省CPU的性能, 初始化任务之后调用函数实现第一个函数的开启, 如果更改默认函数TCB的名字, 需要在调用函数中更改默认的第一个默认函数

任务申请的函数使用的FreeRTOS的函数格式

```c
void Task_main(void)
{

	task1 = task_init();
	task2 = task_alloc();

	task1->tss = xTaskCreateStatic(Task1_Entry, 
						"Task1", 
						TASK1_STACK_SIZE, 
						NULL, 
						Task1Stack,
						&Task1TCB);
	task2->tss = xTaskCreateStatic(Task2_Entry, 
						"Task2", 
						TASK2_STACK_SIZE, 
						NULL, 
						Task2Stack,
						&Task2TCB);

	//申请另一个任务
	task_run(task1);
	task_run(task2);
	
	vTaskStartScheduler();
}
```

### 实现方法

使用结构体TASKCTL进行任务的控制, 这里面保存有所有运行中的任务, 默认设置最多为5个任务(TASK结构体), 可以修改宏定义进行增加, 还有一个TASKLEVEL数组, 这一个数组是用来进行任务优先级设置的, 数组里面包含一个任务指针数组, 用来控制任务的优先级

任务控制块为结构体TASK, 里面使用tskTaskControlBlock进行控制任务的状态, 这里面记录有任务的栈指针以及任务的名字, 目前支持使用函数xTaskCreateStatic进行任务的状态初始化, 函数的参数以及具体实现在jiao_task.c文件中, TASK结构体里面还保存有任务的优先级, 以及任务每一次切换的时候间隔的时间, 在进行任务的添加的时候进行设置

任务的栈设置需要用户根据实际的使用进行初始化, 使用一个数组作为栈

使用的是taskYIELD();函数触发PendSV中断, 这个中断设置为最低优先级, 在中断中会进行任务状态的保存,在处理完其他的中断以后会进行切换, 目前使用的是时钟中断进行任务的切换, 任务的在初始化的时候与任务切换时钟进行挂载, 时钟定时到达的时候会进行切换, 切换时候自动设置时间为下一个任务的时间设置

当存在高优先级任务的时候会对低优先级的任务进行屏蔽, 所以在高优先级的任务执行完毕以后使用			`task_sleep(task1);`进行任务的休眠

### 默认程序

初始化两个任务

1. 任务1: 进行各种标志位的判断, 判别是否有触摸事件和是否有按键事件以及时钟事件, 在处理结束之后会进行任务休眠处理, 之后唤醒通过FIFO写入数据, **在这里需要设置第一次切换任务的时间**
2. 任务2: 主要是用来测试切换任务, 会在这里进行灯的翻转以及打印一条信息

## 图形化界面

图层使用结构体SHTCTL进行管理, 每一个涂层的信息保存在SHEET结构体里面, 保存有图层的位置, 图层的颜色信息以及图层的事件等, 默认使用最多的图层为5层, 可以修改宏定义进行修改

首先绘制图形, 之后将桌面的的图像保存在Flash里面(芯片本身内部SRAM的大小的限制), 在保存之后会用于其他图层的绘制, 其他的图层保存在数组中, 使用画板的方式, 用8bit的画板在绘制的时候转化为16位的数据, 从而减少空间的使用, 之后更新图层的buf之后刷新图层即可完成画面的显示, 之后在申请的时候申请一个图层大小为0设置为最底层进行图层的任务检测

图层可以使用透明色col_inv, 之后的绘制的时候会显示下面的图层, 具体使用为鼠标的部分位置显示下面的图层

图层的相关操作有图层的上下移动, 图层的位置移动等, 使用`sheet_slide`函数, 实现鼠标的移动使用的就是这一个函数

其他还设置了从屏幕获取颜色信息, 从Flash获取颜色信息的函数, 具体的介绍参照工程中的注释

在打印的时候支持中文, 现在完成窗口make_textbox8, 文本框的数组初始化工作putfonts8_asc, 字模使用Flash里面存储的字模(野火烧录的字模程序)

```c
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
    //初始化鼠标的图层
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
```

### 图层事件



## 触摸屏

使用软件消抖, 采用两个中断, 第一个EXTI中断获取触摸, 之后开启时钟, 时钟在一定时期后发生中断, 中断会设置标志位, 之后系统通过标志位进行判断, 并进行相应的处理, 标志位的定义在jiao_os.h

## 按键

硬件消抖, 直接通过中断进行读取, 之后将标志位传入FIFO里面, 在主函数中进行判断

## 任务切换

使用PendSV进行切换, 设置中断优先级为最低, 在处理其他中断之后会进行切换, 现在可以设置每一个任务执行的时间, 建议设置10~20左右, 后期会加入根据优先级进行切换
