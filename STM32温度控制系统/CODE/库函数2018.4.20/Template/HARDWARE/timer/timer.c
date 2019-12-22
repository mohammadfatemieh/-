#include "sys.h"
#include "timer.h"

u32 CA=0;
u32 CB=0;
u32 CC=0;
extern u8 flag;
extern u8 flag_usart2;
extern u8 flag_KEY;
void Timer2_Init(u16 arr,u16 psc)
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
NVIC_InitTypeDef NVIC_Initstrue;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //ͨ�ö�ʱ��2RCCʱ��ʹ��
	
	TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1;          //��Ԥ��Ƶǰ����ʱ����Ӱ��Ԥ��Ƶϵ������������
	TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up;   //���ϼ���
	TIM_TimeBaseInitStrue.TIM_Period=arr;         //�Զ�װ��ֵ
	TIM_TimeBaseInitStrue.TIM_Prescaler=psc;            //ʱ��Ԥ��Ƶϵ��

TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStrue);//��ʱ��2�Ĵ�����ʼ��

TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//��ʱ��2�ж����ã������жϣ���ĳ���Ĵ������£�����жϣ�

	
	NVIC_Initstrue.NVIC_IRQChannel= TIM2_IRQn;  //TIM2�ж�ͨ��
	NVIC_Initstrue.NVIC_IRQChannelCmd=ENABLE;	//IRQ�ж�ͨ��ʹ��
	NVIC_Initstrue.NVIC_IRQChannelPreemptionPriority=0; //��ռ���ȼ�Ϊ0
	NVIC_Initstrue.NVIC_IRQChannelSubPriority=3;	//�����ȼ�Ϊ3
	NVIC_Init(&NVIC_Initstrue); //��ʼ��NVIC�ж����ȼ��Ĵ���
	
	TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2
	
	
}

void TIM2_IRQHandler(void)
{

if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)       //�������жϱ�־λ SET=1�궨��
	{
		CA++;
		CB++;
		CC++;
		if(CA>=20) 
		{
		flag=1;
		CA=0;
		}				
		
		if(CB>=1000)   //WIFI���� ��ʱ
		{
		flag_usart2=1;
			CB=0;
		}
	/*	if(CC>=5)
		{
		
		flag_KEY=1;
			CC=0;
		}			//������ʱɨ��
	*/	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����������жϱ�־
		
	}	


}
