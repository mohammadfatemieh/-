#include "stm32f10x.h"
#include "delay.h"
#include "lcd.h"
#include "pwm.h"
#include "led.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "tem.h"
#include "pid.h"
#include "24c02.h"
#include "touch.h"
#include "wifi.h"
#include "EXTI.h"
#include "timer.h"
#include "string.h" 
//***�Զ���GUI***//
//���ߺ���
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}

//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
/*�������+�������GUI*/
//�������λ��   x��370>++&&y:550>++;
//id:NT35510 ID:6804����������Ҫ�ֿ� �˹�������
void MYGUI_LCD_Clear_right_point(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//ĩ����
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804������ʱ�����⴦��  
	{						    
 		lcddev.dir=0;	  //0����
 		lcddev.setxcmd=0X2A;      //д�������е�ַ����
		lcddev.setycmd=0X2B;  	 	//д������ҳ��ַ����		
		LCD_SetCursor(0xC8,0x0226);		//���ù��λ��  
 		lcddev.dir=1;	 //1����
  		lcddev.setxcmd=0X2B;        //д������ҳ��ַ����
		lcddev.setycmd=0X2A;  	  	//д�������е�ַ����
 	}else LCD_SetCursor(0xc8,0x190);	//���ù��λ��   
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
}  
/*�������-�������GUI*/  
//������λ��x<200&&550<y<650
//id:NT35510 ID:6804����������Ҫ�ֿ� �˹�������
void MYGUI_LCD_Clear_left_point(u16 color)
{
	u32 index=0;      
	u32 totalpoint=200;
	totalpoint*=200; 			//�õ��ܵ���
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804������ʱ�����⴦��  
	{						    
 		lcddev.dir=0;	  //0����
 		lcddev.setxcmd=0X2A;      //д�������е�ַ����
		lcddev.setycmd=0X2B;  	 	//д������ҳ��ַ����		
		LCD_SetCursor(0xC8,0x0226);		//���ù��λ��  ���е�ַ��ҳ��ַ�� //220=0XC8,550=0x0226
 		lcddev.dir=1;	 //1����
  		lcddev.setxcmd=0X2B;        //д������ҳ��ַ����
		lcddev.setycmd=0X2A;  	  	//д�������е�ַ����
 	}else LCD_SetCursor(0x3C,0x226);	//���ù��λ��   ��ҳ��ַ���е�ַ�� 0x226=550
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
}  

			/*		if(tp_dev.x[t]<200&&550<tp_dev.y[t]&&tp_dev.y[t]<650)
					{
				
					Keytem-=1;
					//gui_fill_circle(100,600,40,BLACK);
					}	
					else if(tp_dev.x[t]>370&&550<tp_dev.y[t]&&tp_dev.y[t]<650)
					{
			*/	




/*ȫ�ֱ���*/
	 float ADC1_tem=0;
	 int Keytem=0;
	 u8 flag=0;
	u8 flag_usart2=0;
	u8 flag_KEY=0;
	u8 flag_send;
 //u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//u16 USART2_RX_STA;         		//����״̬���	
/*������*/
struct pid {
	
	
	float SetTem;    //�趨���¶�
	float ActualTem;  //ʵ�ʴﵽ���¶�
	float err;           //ƫ��ֵ
	float err_last;			//��һ�ε�ƫ��ֵ
	float Kp,Ki,Kd;            //PID����
	float voltage;				//PWMռ�ձȸı�ֵ��PID�㷨�����ֵ
	float integral;            //������
}pid;


 	int main(void)
 {	 
	 /*��������*/
	char aaaa[20]="0000000000";
	 char pppp[20]="0000000000";
	 char iiii[20]="0000000000";
	 char dddd[20]="0000000000";
	 char ssss[20]="000";
	 float temp=0;

	 float dr=0;
	 float res=0;
	 
	 float getadcx[14];      //�洢15��adcֵ
	 float adctemp=0;
	
	 float receive=0;
	float c; 
	 float temp_p;
	 float temp_i;
	 float temp_d;
	 float temp_tem;
	 int a=0;
	 int b=0;
	 int i;
	 int j=0;
	 int k=0;
	 int l;
	int m;
	int n;


	 char *ch;
	 char *ch1;
	 char *ch2;
	 char *ch3;
	 char *ch4;
	 char st[50];

	char JSONData1[64];
	char JSONData2[64];
	char JSONData3[64];
	char JSONData4[64];
	 vu8 key=0;	
	 	  u8 t=0;

	u16 KEYtem_integer;
	u16 Kp_integer;
	u16 Ki_integer;
	u16 Kd_integer;
     u16 adcx;
	 u16 led0pwmval;
	u16 LEN;

	 
	 /*��ʼ����NVIC�жϹ���*/
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	//LCD��ʼ��
 	Adc_Init();		  		//ADC��ʼ��
	key_init();				//������ʼ��
	pid_init();        		//PID��ʼ��
	tp_dev.init();        	//��������ʼ��
	WIFI_init(115200);      //usart2 WIFIģ�鴮�ڳ�ʼ��
	AT24CXX_Init();        //24c02��ʼ��
TIM3_PWM_Init(1439,0);//1439+1=1440,����Ƶ��Ƶ��Ϊ72000000/1440=50khz
	// EXTIX();
Timer2_Init(99,7199); //��ʱ��2��ʼ����ʱ��10ms����һ���ж�

	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,50,200,16,16,"graduation project��temperature-control system");	
	LCD_ShowString(60,70,200,16,16,"author:zhuchen");	
	LCD_ShowString(60,90,200,16,16,"14 Electrical Engineering and Automation");
	LCD_ShowString(60,110,200,16,16,"2018/4/5");	
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=RED;//��������Ϊ��ɫ
	LCD_ShowString(60,130,200,16,16,"ADC_CH0_VAL:");	      
	LCD_ShowString(60,150,200,16,16,"ADC_CH0_VOL:0.000V");	//��ʾ��X��ÿ8һ����16��16��
LED1=0;	

	while(1)
	{
/*ADC�ɼ�*/
		POINT_COLOR=RED;   //����������ʾ��ɫ
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//��ʾADC��ֵ

/* ����˲�  ȥ����ֵƽ���˲�*/
		//ȡ14����
		
		for(j=0;j<14;j++)              //ȡ14��ADCֵ �洢��getadcx����
		{
		temp=(float)adcx*(3.0/4096); //STM32��12λ��ADC ,2��12�η�=4096
		getadcx[j]=temp;
		}                    

		//ð������
		for(a=0;a<14;a++)       
		{
     		for(b=0;b<13;b++)
			{
				
				if(getadcx[b]>getadcx[b+1])
				{

			adctemp=getadcx[b];
			getadcx[b+1]=getadcx[b];
			getadcx[b]=adctemp;

				}
			}

		}
		//ȥ�������������Сֵ��ȡƽ����
		for(i=2;i<11;i++)    //�����˲���ȥ���������ֵ��������Сֵ��Ȼ�����
		{
		c+=getadcx[i];
		}
		c=c/10               ;        //ȡƽ��ֵ
		
		
			temp=c;	
			receive=c;
		
		
/*ADC����ת����ѹ*/

		
		
		adcx=temp;//adcxΪ���� tempΪfloat�������ͣ���temp��ֵ��adcxֻȡ����
		LCD_ShowxNum(156,150,adcx,1,16,0);//��ʾ��ѹֵ ���������֣�
		temp-=adcx; //temp=temp-adcx ��ȥ��ѹ��������
		temp*=1000;//temp=temp*1000 ��С����Ϊ���� ��ȷ��3λС��
		LCD_ShowxNum(172,150,temp,3,16,0X80); //��ʾ��ѹֵ��С�����֣�
		
/*��ѹת��Ϊ����*/
		res=((receive/10.0)/0.909)*1000.0;
		
		
		
/*���ת��Ϊ�¶�*/	
		ADC1_tem=(float)get_tem(res);
	
		LCD_ShowString(60,190,200,16,16,"ActureTempretureFromADC1:");
		sprintf(aaaa,"%3.1f\r\n",ADC1_tem);
		LCD_ShowxNum(320,190,ADC1_tem,3,24,0);
		
		
/*���ݷ��͵�WIFI
10sһ��
*/

		if(flag_usart2==1)
		{
		
Usart_SendString(USART2,aaaa);
			flag_usart2=0;
		}


		
/*���ռ�ձȺ�PWM�Ĵ���CCRx��ֵ��LCD*/
	/*		
		adcx=temp;//adcxΪ���� tempΪfloat�������ͣ���temp��ֵ��adcxֻȡ����
		LCD_ShowxNum(156,150,adcx,1,16,0);//��ʾ��ѹֵ ���������֣�
		temp-=adcx; //temp=temp-adcx ��ȥ��ѹ��������
		temp*=1000;//temp=temp*1000 ��С����Ϊ���� ��ȷ��3λС��
		LCD_ShowxNum(172,150,temp,3,16,0X80); //��ʾ��ѹֵ��С�����֣�
	*/	
		 led0pwmval=pid_realize(Keytem);
		
		TIM_SetCompare2(TIM3,led0pwmval);//PWM���
		
		LCD_ShowString(60,210,200,16,16,"PWMVAL:");
		LCD_ShowxNum(150,210,led0pwmval,4,16,0);
		dr=(led0pwmval/1440.0)*100;
		LCD_ShowString(60,230,200,16,16,"DutyCycle:   %");
		LCD_ShowxNum(140,230,dr,3,16,0);
	
		
/*��������¶��Լ���ǰPIDֵ*/		
	
POINT_COLOR=BROWN;
LCD_ShowString(60,350,200,50,24,"Tempreture:");
sprintf(ssss,"%3d",Keytem);
LCD_ShowString(200,350,200,50,24,ssss);
//LCD_ShowxNum(200,350,Keytem,3,24,0);

LCD_ShowString(100,250,200,16,16,"P:");

sprintf(pppp,"%3.1f",pid.Kp);
LCD_ShowString(145,250,200,16,16,pppp);

sprintf(iiii,"%4.3f",pid.Ki);
LCD_ShowString(100,270,200,16,16,"I:");
LCD_ShowString(145,270,200,16,16,iiii);
LCD_ShowString(100,290,200,16,16,"D:");
sprintf(dddd,"%3.2f",pid.Kd);
LCD_ShowString(145,290,200,200,16,dddd);
/*��������������ͼ��*/
POINT_COLOR=BLACK;
LCD_Draw_Circle(400,600,40);  
LCD_ShowChar(395,590,'+',24,1); //+��
LCD_Draw_Circle(100,600,40);
LCD_ShowChar(95,590,'-',24,1); //-��

/*
gui_fill_circle(400,600,40,BLACK);
gui_fill_circle(100,600,40,BLACK);
��ɫ������ť
*/



/*�����Ͱ����ı��¶�ֵ
50msɨ��һ��
*/
	
	if(tp_dev.touchtype&0X80)	//����������
		
	{
		tp_dev.scan(0);
			for(t=0;t<CT_MAX_TOUCH;t++)
				{
					if((tp_dev.sta)&(1<<t))
					{
					if(tp_dev.x[t]<200&&550<tp_dev.y[t]&&tp_dev.y[t]<650)
					{
				
					Keytem-=1;
					gui_fill_circle(100,600,40,BLACK);
					delay_ms(10);
						MYGUI_LCD_Clear_left_point(WHITE);
					}	
					else if(tp_dev.x[t]>370&&550<tp_dev.y[t]&&tp_dev.y[t]<650)
					{
				
					Keytem+=1;
					
					gui_fill_circle(400,600,40,BLACK);
						delay_ms(10);
				
						MYGUI_LCD_Clear_right_point(WHITE);
					}
					
				
					}
				}
	
		}

LCD_Draw_Circle(400,600,40);  
LCD_ShowChar(395,590,'+',24,1); //+��
LCD_Draw_Circle(100,600,40);
LCD_ShowChar(95,590,'-',24,1); //-��

		
			key=KEY_SCAN(1);
			if(key)
			{
			switch (key)
				{
				case KEY1_PRESS:
					Keytem+=1;
				break;
				case KEY2_PRESS:
					Keytem-=1;
				break;
				}
		
			}else delay_ms(10);
		
			if(Keytem>350) Keytem=350;
		    else if(Keytem<0) Keytem=0;

			
			
	
	/*����WIFI���ܵ�������*/
	if(USART2_RX_STA&0x8000) //������ΪΪ1���������
		
{
	LEN=USART2_RX_STA&0x3FFF; //ȡ�����ݳ���

	//Usart_SendString(USART2,"�����͵���ϢΪ\r\n");//printf("\r\n�����͵���ϢΪ��\r\n\r\n");

	for(k=0;k<LEN;k++) //�����ܵ����ݻ�����������һ��һ����������st
	{
	st[k]=USART2_RX_BUF[k];
		
	}

	
/*	for(k=0;k<LEN;k++)
	{
	USART_SendData(USART2, USART2_RX_BUF[k]);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
	}
	*/
	/*
	
		for(k=0;k<LEN;k++)
	{
	USART_SendData(USART2, st[k]);//�򴮿�2���ͻ���������
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET); //�ȴ����ݷ��ͽ���
	}
	*/
	
	/*�ַ����Ѿ��洢��str[K]��
	���͸�ʽ SetTemp:100,P:22,I:0.05,D:0.01\r\n 
	�ж�Settempֵ��PID����
	*/



	ch=strstr(st,"SetTemp:");
	
	ch1=strstr (st,",P:");
	ch2=strstr(st,",I:");
	ch3=strstr(st,",D:");
	ch4=strstr(st,"\r\n");
	
	
	
	for(n=0;n<=64;n++)
	{
	JSONData1[n]='\0';
	JSONData2[n]='\0';	
	JSONData3[n]='\0';
	JSONData4[n]='\0';
	}
	if(ch!=NULL)
	{
	strncpy(JSONData1,st+8,ch1-ch-8);
	
    Keytem=atoi(JSONData1);		
	}
	if(ch1!=NULL)
	{
		strncpy(JSONData2,ch1+3,ch2-ch1-3);
		
		pid.Kp=atof(JSONData2);
	}
	if(ch2!=NULL)
	{
		strncpy(JSONData3,ch2+3,ch3-ch2-3);
		
		pid.Ki=atof(JSONData3);
		
	}
	if(ch3!=NULL)
	{
	strcpy(JSONData4,ch3+3);
	
	pid.Kd=atof(JSONData4);
	}

	
/*
	Usart_SendString(USART2,JSONData1);
	Usart_SendString(USART2,"\r\n");
	Usart_SendString(USART2,JSONData2);
	Usart_SendString(USART2,"\r\n");
	Usart_SendString(USART2,JSONData3);
	Usart_SendString(USART2,"\r\n");
	Usart_SendString(USART2,JSONData4);
	Usart_SendString(USART2,"\r\n");
*/	
	


		
		for(l=0;l<LEN;l++) //
	{
	st[l]='\0';
	}
	
	for(m=0;m<LEN;m++) //
	{
	USART2_RX_BUF[m]='\0';
	}
	

/*	
	for (StrLen1=StrLen1+8;StrLen1<3;StrLen1++)
{
JSONData[StrLen1]=str[StrLen1];
}	
	*/
	
	/*
if(ch==NULL)
	{
		
	}
	else
	{
			if(ch[Strlen]==':')
		{
			for(l=0;l<4;l++)
				{
				JSONData[l]=str[Strlen+1+l];
					
				}
		}
	}
	*/

	//Usart_SendString(USART2,JSONData);

	//��ⷢ�������Ƿ���Ϸ��͹���
/*
   if(ch==NULL)
	{
	
	Usart_SendString(USART2,"�������ݴ���");
		
	}
	else
	{
		Usart_SendString(USART2,"����SetTemp");
	}

 
*/
	
	
	//Usart_SendString(USART2,"\r\n\r\n");//printf("\r\n\r\n");//���뻻��
	USART2_RX_STA=0;
	
}


	delay_ms(50);
	}//while(1) end
 
}//main end

 
