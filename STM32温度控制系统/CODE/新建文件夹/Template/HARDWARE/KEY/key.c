#include "stm32f10x.h"
#include "key.h"
#include "delay.h"

void key_init(void)
{
	
	GPIO_InitTypeDef   GPIO_InitStructure;//����ṹ�����������������
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE); //rccʱ��ʹ��PE��
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//rccʱ��ʹ��PA��
	
	


	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//�ṹ�帳ֵ
	GPIO_Init(GPIOE,&GPIO_InitStructure);//PE4�� key0 ����Ϊ��������
	


	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//�ṹ�帳ֵ
	GPIO_Init(GPIOE,&GPIO_InitStructure);//PE3�� key1 ����λ��������
	

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//�ṹ�帳ֵ
	GPIO_Init(GPIOE,&GPIO_InitStructure);//PE2�� key2 ����λ��������
	

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//�ṹ�帳ֵ
	GPIO_Init(GPIOA,&GPIO_InitStructure);//PA0�� key3 ����λ��������
	
}


 u8 KEY_SCAN(u8 mode)
{
static u8 key_up=1;//�����ɿ���־
	
if(mode) key_up=1;//֧������
	if(key_up&&(KEY1==0||KEY2==0))
		
	{
	delay_ms(10);
	key_up=0;

		if(KEY1==0) return KEY1_PRESS;
		if(KEY2==0) return KEY2_PRESS;
	
	} else if (KEY1==1&&KEY2==1)key_up=1;
return 0;
}



