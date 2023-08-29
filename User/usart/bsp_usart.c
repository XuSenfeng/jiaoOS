#include "bsp_usart.h"

#if IT_Config
static void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = DEBUG_USART_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

#endif
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//初始化时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);
	
	
	//设置GPIO
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	//配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	//数据长度
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//初始化串口
	USART_Init(DEBUG_USARTx, &USART_InitStructure);

#if IT_Config
	//初始化NVIC
	NVIC_Configuration();
	//初始化中断,在接受的时候产生中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
#endif
	//使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);
	
}

//发送一个字节
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t data)
{
	
	USART_SendData(pUSARTx, data);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
//发送两个字节
void Usart_sendHalfWord(USART_TypeDef* pUSARTx, uint16_t data)
{
	uint8_t temp_h, temp_l;
	temp_h = (data & 0xff00)>>8;
	temp_l = data & 0xff;
	USART_SendData(pUSARTx, temp_h);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	USART_SendData(pUSARTx, temp_l);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

//发送一个数组
void Usart_sendString(USART_TypeDef* pUSARTx, uint8_t* data, uint8_t num)
{
	int i;

	for(i=0 ; i<num ; i++)
	{
		USART_SendData(pUSARTx, data[i]);
		while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	}
	//这里设置为TC为发送完成的时候循环结束
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);	
}
//发送字符串
void Usart_sendString2(USART_TypeDef* pUSARTx, uint8_t* data)
{

	while(1)
	{
		if(*data != '\0'){
		USART_SendData(pUSARTx, *data++);
		while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
		}else
		break;
		
	}
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);	
}

//重定义C库函数
int fputc(int ch, FILE *f)
{
	USART_SendData(DEBUG_USARTx, (uint8_t)ch);
	while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
	return ch;
}

int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(DEBUG_USARTx);
}


