#include "stm32f10x.h"
#include "wifi.h"
#include "led.h"
/*����2��������WIFIģ�� RXD->PA3 TXD->PA2
�������Ƽ�96000
��osϵͳ�궨��
֧��ͨѶЭ�飨HTTP,UDP/TCP��
*/
#if EN_USART2_RX   //���ʹ���˽���
//�жϷ�����
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	  
extern u8 flag_send;
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 rec;
void WIFI_init(u32 bound)
{
	
	GPIO_InitTypeDef GPIO_Initstrue;			//GPIO��ʼ���ṹ��
		USART_InitTypeDef USART_Initstrue;		//USART2��ʼ���ṹ��
	NVIC_InitTypeDef NVIC_Initstrue;            //NVIC��ʼ���ṹ��

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //����2ʱ��ʹ��  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����2����TX,RX(PA2,PA3)ʱ��ʹ��
	
	GPIO_Initstrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Initstrue.GPIO_Pin=GPIO_Pin_2;
	GPIO_Initstrue.GPIO_Speed=GPIO_Speed_50MHz;    //����2TX ��PA2��Ҫ��Ϊ���츴��
	//PA9��ʼ���ṹ���Ա������ֵ
	GPIO_Init(GPIOA,&GPIO_Initstrue);
	
		GPIO_Initstrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Initstrue.GPIO_Pin=GPIO_Pin_3;
	GPIO_Initstrue.GPIO_Speed=GPIO_Speed_50MHz;    //����2RX( PA3)Ҫ��Ϊ�����������������
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
	NVIC_Initstrue.NVIC_IRQChannelPreemptionPriority=2;    //��ռ���ȼ�Ϊ1
	NVIC_Initstrue.NVIC_IRQChannelSubPriority=0;			//��Ӧ���ȼ�Ϊ3
	//NVIC��ʼ���ṹ���Ա������ֵ
	NVIC_Init(&NVIC_Initstrue);

}

void USART2_IRQHandler(void)  
{

if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���ܻ������ǿ� (���յ������ݱ�����0x0d 0x0a��β)
{
	
	
	
	u8 Res;

	
		Res=USART_ReceiveData(USART2);	//��ȡ���յ�������
		
		if((USART2_RX_STA&0x8000)==0)//����δ���
			{
				
			if(USART2_RX_STA&0x4000)//���յ���0x0d
				{
					
				if(Res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
				else USART2_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
					{
					
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   
		
	

}
}

void Usart_SendString(USART_TypeDef* USARTx,char *str)
{
while(*str)
{
 while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE)); //�ж��Ƿ���Է���
      USART_SendData(USARTx,*str);
      while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)); //�ж��Ƿ�����ɣ��˾�����У��������
                                                                                                  //��ֻ�������һ���ַ������ǣ�
 str++;           
  }
}


#endif


