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
	//��ʼ��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);
	
	
	//����GPIO
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	//���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	//���ݳ���
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//��ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);

#if IT_Config
	//��ʼ��NVIC
	NVIC_Configuration();
	//��ʼ���ж�,�ڽ��ܵ�ʱ������ж�
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
#endif
	//ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);
	
}

//����һ���ֽ�
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t data)
{
	
	USART_SendData(pUSARTx, data);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
//���������ֽ�
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

//����һ������
void Usart_sendString(USART_TypeDef* pUSARTx, uint8_t* data, uint8_t num)
{
	int i;

	for(i=0 ; i<num ; i++)
	{
		USART_SendData(pUSARTx, data[i]);
		while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	}
	//��������ΪTCΪ������ɵ�ʱ��ѭ������
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);	
}
//�����ַ���
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

//�ض���C�⺯��
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


