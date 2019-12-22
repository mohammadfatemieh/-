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



	 float ADC1_tem=0;
	 
 	int main(void)
 {	 	float temp=0;
	 
	 float tp=0;
	 float dr=0;
	 float res=0;
	 
	 float getadcx[14];      //�洢15��adcֵ
	 float adctemp=0;
	 float t=0;
	 float receive=0;
float try;
	 int Keytem=0;
	 int a=0;
	 int b=0;
	 int i;
	 int j=0;
	 vu8 key=0;	
	 	 
     u16 adcx;
	 u16 led0pwmval=720;
	 u16 test;
	 u16 ADC1_du;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	
 	Adc_Init();		  		//ADC��ʼ��
		 key_init();
		 pid_init();        //PID��ʼ��
TIM3_PWM_Init(1439,0);//1439+1=1440,����Ƶ��Ƶ��Ϊ72000000/1440=50khz
	 

	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"graduation project��temperature-control system");	
	LCD_ShowString(60,70,200,16,16,"author:zhuchen");	
	LCD_ShowString(60,90,200,16,16,"14 Electrical Engineering and Automation");
	LCD_ShowString(60,110,200,16,16,"2018/4/5");	
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
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
				 led0pwmval+=1;
			break;
			case KEY2_PRESS:
				led0pwmval-=1;
			break;
			}
		
		}else delay_ms(10);
			if(led0pwmval>1439)led0pwmval=350;
		    else if(led0pwmval<0) led0pwmval=0;
		LCD_ShowString(60,170,200,16,16,"Tempreture:");
		LCD_ShowxNum(145,170,Keytem,3,16,0);
	
	
		
		
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
		t+=getadcx[i];
		}
		t=t/10               ;        //ȡƽ��ֵ
		
		
		
		temp=t;
		 
		receive=t;
		
		adcx=temp;//adcxΪ���� tempΪfloat�������ͣ���temp��ֵ��adcxֻȡ����
		LCD_ShowxNum(156,150,adcx,1,16,0);//��ʾ��ѹֵ ���������֣�
		temp-=adcx; //temp=temp-adcx ��ȥ��ѹ��������
		temp*=1000;//temp=temp*1000 ��С����Ϊ���� ��ȷ��3λС��
		LCD_ShowxNum(172,150,temp,3,16,0X80); //��ʾ��ѹֵ��С�����֣�
		
		res=((receive/10.0)/0.909)*1000.0;
		
		ADC1_tem=(int)get_tem(res);
	
				LCD_ShowString(60,190,200,16,16,"ActureTempretureFromADC1:");
	

		LCD_ShowxNum(320,190,ADC1_tem,3,16,0);
		
		/*
		 led0pwmval=pid_realize(Keytem);
		*/
		
		TIM_SetCompare2(TIM3,led0pwmval);//PWM���
		
		LCD_ShowString(60,210,200,16,16,"PWMVAL:");
		LCD_ShowxNum(150,210,led0pwmval,4,16,0);
/*		
		dr=(1-(led0pwmval/1800))*100;
		LCD_ShowString(60,230,200,16,16,"Duty Ratio:");
		LCD_ShowxNum(150,230,dr,3,16,0);
	*/	
LCD_Draw_Circle(400,600,40);
delay_ms(100);
	
	}
}
 


 