#ifndef __wifi_H_
#define __wifi_H_
#include "sys.h"
#include "stdio.h"	


#define USART2_REC_LEN  			300  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	
void WIFI_init(u32 bound);



void USART2_IRQHandler(void);  

void Usart_SendString(USART_TypeDef* USARTx,char *str);
#endif



