#include "stm32f10x.h"
#include "delay.h"
#include "lcd.h"
#include "pwm.h"
#include "led.h"
#include "key.h"
#include "sys.h"
//#include "usart.h"
#include "adc.h"
#include "tem.h"
#include "pid.h"
#include "24c02.h"
#include "touch.h"
#include "wifi.h"
#include "EXTI.h"
#include "timer.h"
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






/*ȫ�ֱ���*/
	 float ADC1_tem=0;
	 int Keytem=0;
	 u8 flag=0;
/*������*/

 	int main(void)
 {	 
	 /*��������*/

	 float temp=0;
	 float tp=0;
	 float dr=0;
	 float res=0;
	 
	 float getadcx[14];      //�洢15��adcֵ
	 float adctemp=0;
	
	 float receive=0;
	float c; 
	 
	 int a=0;
	 int b=0;
	 int i;
	 int j=0;
	 vu8 key=0;	
	 	  u8 t=0;

     u16 adcx;
	 u16 led0pwmval;
	 u16 test;
	 u16 ADC1_du;
	 
	 /*��ʼ����NVIC�жϹ���*/
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	//uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	//LCD��ʼ��
 	Adc_Init();		  		//ADC��ʼ��
	key_init();				//������ʼ��
	pid_init();        		//PID��ʼ��
	tp_dev.init();        	//��������ʼ��
	//WIFI_init(115200);      //usart2 WIFIģ�鴮�ڳ�ʼ��
	AT24CXX_Init();        //24c02��ʼ��
TIM3_PWM_Init(1439,0);//1439+1=1440,����Ƶ��Ƶ��Ϊ72000000/1440=50khz
	// EXTIX();
//Timer2_Init(99,7199); //��ʱ��2��ʼ����ʱ��10ms����һ���ж�
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,50,200,16,16,"graduation project��temperature-control system");	
	LCD_ShowString(60,70,200,16,16,"author:zhuchen");	
	LCD_ShowString(60,90,200,16,16,"14 Electrical Engineering and Automation");
	LCD_ShowString(60,110,200,16,16,"2018/4/5");	
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=RED;//��������Ϊ��ɫ
	LCD_ShowString(60,130,200,16,16,"ADC_CH0_VAL:");	      
	LCD_ShowString(60,150,200,16,16,"ADC_CH0_VOL:0.000V");	
LED1=0;	
	while(1)
	{
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

	
	POINT_COLOR=RED;
/*STMȡ��adc��ֵ*/	
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//��ʾADC��ֵ


		for(j=0;j<14;j++)              //ȡ14��ADCֵ �洢��getadcx����
		{
			
		temp=(float)adcx*(3.0/4096); //STM32��12λ��ADC ,2��12�η�=4096
		getadcx[j]=temp;
		}                    


		for(a=0;a<14;a++)       //ð������
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

		for(i=2;i<11;i++)    //�����˲���ȥ���������ֵ��������Сֵ��Ȼ�����
		{
		c+=getadcx[i];
		}
		c=c/10               ;        //ȡƽ��ֵ
		
		
		temp=c;
		 
		receive=c;
		
		adcx=temp;//adcxΪ���� tempΪfloat�������ͣ���temp��ֵ��adcxֻȡ����
		LCD_ShowxNum(156,150,adcx,1,16,0);//��ʾ��ѹֵ ���������֣�
		temp-=adcx; //temp=temp-adcx ��ȥ��ѹ��������
		temp*=1000;//temp=temp*1000 ��С����Ϊ���� ��ȷ��3λС��
		LCD_ShowxNum(172,150,temp,3,16,0X80); //��ʾ��ѹֵ��С�����֣�
		
		res=((receive/10.0)/0.909)*1000.0;
		
		ADC1_tem=(float)get_tem(res);
	
				LCD_ShowString(60,190,200,16,16,"ActureTempretureFromADC1:");
	//USART_SendData(USART2,ADC1_tem); //����2�������� WIFI��������

		LCD_ShowxNum(320,190,ADC1_tem,3,16,0);
		
		 led0pwmval=pid_realize(Keytem);
		
		TIM_SetCompare2(TIM3,led0pwmval);//PWM���
		
		LCD_ShowString(60,210,200,16,16,"PWMVAL:");
		LCD_ShowxNum(150,210,led0pwmval,4,16,0);
		dr=(led0pwmval/1440.0)*100;
		LCD_ShowString(60,230,200,16,16,"DutyCycle:");
		LCD_ShowxNum(140,230,dr,4,16,0);
		/*
		dr=(1-(led0pwmval/1800))*100;
		LCD_ShowString(60,230,200,16,16,"Duty Ratio:");
		LCD_ShowxNum(150,230,dr,3,16,0);
	*/	
	
	POINT_COLOR=BROWN;
		LCD_ShowString(60,350,200,50,16,"Tempreture:");
		LCD_ShowxNum(145,350,Keytem,3,24,0);

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
					//gui_fill_circle(100,600,40,BLACK);
				}	
				else if(tp_dev.x[t]>370&&550<tp_dev.y[t]&&tp_dev.y[t]<650)
				{
				
					Keytem+=1;
					
					//gui_fill_circle(400,600,40,BLACK);
				}
					
				
			}
		}
	
	
	


	
	}
	


	
		delay_ms(50);
}
 
}

 
