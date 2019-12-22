#include "sys.h"
#include "key.h"
#include "EXTI.h"
#include "delay.h"

extern float Keytem;

void EXTIX(void)
{
	NVIC_InitTypeDef NVIC_InitStrue;
	EXTI_InitTypeDef EXIT_InitStrue;
key_init();  //�ⲿ�ж�GPIO������
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //ʹ�ܶ˿ڸ��ã�����λ�ⲿ�жϿڣ�ʱ��
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);//PE2ӳ�䵽�ⲿ�ж���2��
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);//PE3ӳ�䵽�ⲿ�ж���3��
	
	EXIT_InitStrue.EXTI_Line=EXTI_Line2;      //�ⲿ�ж���2
	EXIT_InitStrue.EXTI_LineCmd=ENABLE;	             //�ж���ʹ��
	EXIT_InitStrue.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж�ģʽ���жϣ��жϡ��¼���
	EXIT_InitStrue.EXTI_Trigger=EXTI_Trigger_Falling;//�жϴ�����ʽ���½���
	//�ⲿ�жϳ�ʼ���ṹ���Ա������ֵ
	EXTI_Init(&EXIT_InitStrue);
	
		EXIT_InitStrue.EXTI_Line=EXTI_Line3;      //�ⲿ�ж���3
	EXIT_InitStrue.EXTI_LineCmd=ENABLE;	             //�ж���ʹ��
	EXIT_InitStrue.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж�ģʽ���жϣ��жϡ��¼���
	EXIT_InitStrue.EXTI_Trigger=EXTI_Trigger_Falling;//�жϴ�����ʽ���½���
	//�ⲿ�жϳ�ʼ���ṹ���Ա������ֵ
	EXTI_Init(&EXIT_InitStrue);
	
NVIC_InitStrue.NVIC_IRQChannel=EXTI2_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=2;
	
	NVIC_Init(&NVIC_InitStrue);
	//NVIC�жϳ�ʼ��

	
NVIC_InitStrue.NVIC_IRQChannel=EXTI3_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=2;
	
	NVIC_Init(&NVIC_InitStrue);
	//NVIC�жϳ�ʼ��
}


/*
void EXTI3(void)
{
	NVIC_InitTypeDef NVIC_InitStrue;
	EXTI_InitTypeDef EXIT_InitStrue;
key_init();  //�ⲿ�ж�GPIO������
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //ʹ�ܶ˿ڸ��ã�����λ�ⲿ�жϿڣ�ʱ��
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);//PE3ӳ�䵽�ⲿ�ж���3��
	
	
	EXIT_InitStrue.EXTI_Line=EXTI_Line3;      //�ⲿ�ж���3
	EXIT_InitStrue.EXTI_LineCmd=ENABLE;	             //�ж���ʹ��
	EXIT_InitStrue.EXTI_Mode=EXTI_Mode_Interrupt;	//�ж�ģʽ���жϣ��жϡ��¼���
	EXIT_InitStrue.EXTI_Trigger=EXTI_Trigger_Falling;//�жϴ�����ʽ���½���
	//�ⲿ�жϳ�ʼ���ṹ���Ա������ֵ
	EXTI_Init(&EXIT_InitStrue);
	
NVIC_InitStrue.NVIC_IRQChannel=EXTI3_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=2;
	
	NVIC_Init(&NVIC_InitStrue);
	//NVIC�жϳ�ʼ��

}

*/

/*
void EXTI2_IRQHandler(void)
{




	if(KEY2==0) 
	{
	delay_ms(10);
	Keytem+=1;

	}


EXTI_ClearITPendingBit(EXTI_Line2); //����жϱ�־λ�á�����һ�ν����жϷ����������жϷ������־λ��1��

}


void EXTI3_IRQHandler(void)
{
	



	if(KEY1==0) 
	{
	delay_ms(10);
	Keytem-=1;

	}

EXTI_ClearITPendingBit(EXTI_Line3); //����жϱ�־λ�á�����һ�ν����жϷ����������жϷ������־λ��1��


}

*/

