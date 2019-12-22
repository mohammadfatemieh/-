#include "stm32f10x.h"
#include "wifi.h"
/*����2��������WIFIģ�� RXD->PA3 TXD->PA2
�������Ƽ�96000
��osϵͳ�궨��
֧��ͨѶЭ�飨HTTP,UDP/TCP��
*/

void WIFI_init(u32 bound)
{
	
	GPIO_InitTypeDef GPIO_Initstrue;			//GPIO��ʼ���ṹ��
		USART_InitTypeDef USART_Initstrue;		//USART2��ʼ���ṹ��
	NVIC_InitTypeDef NVIC_Initstrue;            //NVIC��ʼ���ṹ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //����2ʱ��ʹ�� 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����2����TX,RX(PA2,PA3)ʱ��ʹ��
	
	GPIO_Initstrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Initstrue.GPIO_Pin=GPIO_Pin_2;
	GPIO_Initstrue.GPIO_Speed=GPIO_Speed_10MHz;    //����2TX ��PA2��Ҫ��Ϊ���츴��
	//PA9��ʼ���ṹ���Ա������ֵ
	GPIO_Init(GPIOA,&GPIO_Initstrue);
	
		GPIO_Initstrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Initstrue.GPIO_Pin=GPIO_Pin_3;
	GPIO_Initstrue.GPIO_Speed=GPIO_Speed_10MHz;    //����1RX( PA3)Ҫ��Ϊ�����������������
	//PA10��ʼ���ṹ���Ա������ֵ
	GPIO_Init(GPIOA,&GPIO_Initstrue);
	

	USART_Initstrue.USART_BaudRate=115200;									//������115200
	USART_Initstrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //��Ӳ��������
	USART_Initstrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;		//�������ģʽ
	USART_Initstrue.USART_Parity=USART_Parity_No;		 		//����żУ��λ
	USART_Initstrue.USART_StopBits=USART_StopBits_1;			//1��ֹͣλ
	USART_Initstrue.USART_WordLength=USART_WordLength_8b;       //���ݳ���Ϊ8λ
	//���ڳ�ʼ���ṹ���Ա������ֵ
	
	USART_Init(USART2,&USART_Initstrue);
	
	USART_Cmd(USART2,ENABLE);           //ʹ�ܴ���2
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);   //��������2�����жϣ��ж����ͣ����ܻ������ǿգ����ܵ�����ʱ�����жϣ�
	
	
	NVIC_Initstrue.NVIC_IRQChannel=USART2_IRQn;				//�ж�ͨ��Ϊ����2�ж�
	NVIC_Initstrue.NVIC_IRQChannelCmd=ENABLE;				//ͨ��ʹ��
	NVIC_Initstrue.NVIC_IRQChannelPreemptionPriority=1;    //��ռ���ȼ�Ϊ1
	NVIC_Initstrue.NVIC_IRQChannelSubPriority=3;			//��Ӧ���ȼ�Ϊ3
	//NVIC��ʼ���ṹ���Ա������ֵ
	NVIC_Init(&NVIC_Initstrue);

}

void USART2_IRQHandler(void)  
{
u8 rx;
if(USART_GetITStatus(USART2,USART_IT_RXNE)) //���ܻ������ǿ�
{

rx=USART_ReceiveData(USART2);
	
}

}


