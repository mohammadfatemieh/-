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

//***自定义GUI***//
//画线函数
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}

//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
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
/*清除按下+按键后的GUI*/
//清除大致位置   x：370>++&&y:550>++;
//id:NT35510 ID:6804横屏竖屏需要分开 此工程无视
void MYGUI_LCD_Clear_right_point(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//末坐标
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804横屏的时候特殊处理  
	{						    
 		lcddev.dir=0;	  //0竖屏
 		lcddev.setxcmd=0X2A;      //写入设置列地址命令
		lcddev.setycmd=0X2B;  	 	//写入设置页地址命令		
		LCD_SetCursor(0xC8,0x0226);		//设置光标位置  
 		lcddev.dir=1;	 //1横屏
  		lcddev.setxcmd=0X2B;        //写入设置页地址命令
		lcddev.setycmd=0X2A;  	  	//写入设置列地址命令
 	}else LCD_SetCursor(0xc8,0x190);	//设置光标位置   
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
}  
/*清除按下-按键后的GUI*/  
//清除大概位置x<200&&550<y<650
//id:NT35510 ID:6804横屏竖屏需要分开 此工程无视
void MYGUI_LCD_Clear_left_point(u16 color)
{
	u32 index=0;      
	u32 totalpoint=200;
	totalpoint*=200; 			//得到总点数
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804横屏的时候特殊处理  
	{						    
 		lcddev.dir=0;	  //0竖屏
 		lcddev.setxcmd=0X2A;      //写入设置列地址命令
		lcddev.setycmd=0X2B;  	 	//写入设置页地址命令		
		LCD_SetCursor(0xC8,0x0226);		//设置光标位置  （列地址，页地址） //220=0XC8,550=0x0226
 		lcddev.dir=1;	 //1横屏
  		lcddev.setxcmd=0X2B;        //写入设置页地址命令
		lcddev.setycmd=0X2A;  	  	//写入设置列地址命令
 	}else LCD_SetCursor(0x3C,0x226);	//设置光标位置   （页地址，列地址） 0x226=550
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
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

/*全局变量*/
	 float ADC1_tem=0;
	 int Keytem=0;
	 u8 flag=0;
	u8 flag_usart2=0;
	u8 flag_KEY=0;
	u8 flag_send;
/*主函数*/

 	int main(void)
 {	 
	 /*变量定义*/
	char aaaa[20]="0000000000";
	 float temp=0;
	 float tp=0;
	 float dr=0;
	 float res=0;
	 
	 float getadcx[14];      //存储15个adc值
	 float adctemp=0;
	
	 float receive=0;
	float c; 
	 
	 int a=0;
	 int b=0;
	 int i;
	 int j=0;
	 u32 gets;
	 char *s;
	 vu8 key=0;	
	 	  u8 t=0;

     u16 adcx;
	 u16 led0pwmval;
	 u16 test;
	 u16 ADC1_du;
	 
	 /*初始化及NVIC中断管理*/
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化
	LCD_Init();			 	//LCD初始化
 	Adc_Init();		  		//ADC初始化
	key_init();				//按键初始化
	pid_init();        		//PID初始化
	tp_dev.init();        	//触摸屏初始化
	WIFI_init(115200);      //usart2 WIFI模块串口初始化
	AT24CXX_Init();        //24c02初始化
TIM3_PWM_Init(1439,0);//1439+1=1440,不分频，频率为72000000/1440=50khz
	// EXTIX();
Timer2_Init(99,7199); //定时器2初始化，时间10ms开启一次中断

	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(60,50,200,16,16,"graduation project：temperature-control system");	
	LCD_ShowString(60,70,200,16,16,"author:zhuchen");	
	LCD_ShowString(60,90,200,16,16,"14 Electrical Engineering and Automation");
	LCD_ShowString(60,110,200,16,16,"2018/4/5");	
	//显示提示信息
	POINT_COLOR=RED;//设置字体为红色
	LCD_ShowString(60,130,200,16,16,"ADC_CH0_VAL:");	      
	LCD_ShowString(60,150,200,16,16,"ADC_CH0_VOL:0.000V");	
LED1=0;	

	while(1)
	{
/*ADC采集*/
		POINT_COLOR=RED;   //以下字体显示红色
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//显示ADC的值

/* 软件滤波  去极限值平均滤波*/
		//取14次数
		
		for(j=0;j<14;j++)              //取14次ADC值 存储到getadcx数组
		{
		temp=(float)adcx*(3.0/4096); //STM32是12位的ADC ,2的12次方=4096
		getadcx[j]=temp;
		}                    

		//冒泡排序
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
		//去两个最大两个最小值后取平均数
		for(i=2;i<11;i++)    //算数滤波，去除两个最大值和两个最小值，然后求和
		{
		c+=getadcx[i];
		}
		c=c/10               ;        //取平均值
		
		
			temp=c;	
			receive=c;
		
		
/*ADC数字转换电压*/

		
		
		adcx=temp;//adcx为整型 temp为float浮点数型，将temp赋值给adcx只取整数
		LCD_ShowxNum(156,150,adcx,1,16,0);//显示电压值 （整数部分）
		temp-=adcx; //temp=temp-adcx 减去电压整数部分
		temp*=1000;//temp=temp*1000 将小数变为整数 精确到3位小数
		LCD_ShowxNum(172,150,temp,3,16,0X80); //显示电压值（小数部分）
		
/*电压转换为电阻*/
		res=((receive/10.0)/0.909)*1000.0;
		
		
		
/*查表转换为温度*/	
		ADC1_tem=(float)get_tem(res);
	
		LCD_ShowString(60,190,200,16,16,"ActureTempretureFromADC1:");
		
		LCD_ShowxNum(320,190,ADC1_tem,3,16,0);
		
/*数据发送到WIFI
10s一次
*/
		if(flag_usart2==1)
		{
		sprintf(aaaa,"%3.1f\r\n",ADC1_tem);
Usart_SendString(USART2,aaaa);
			flag_usart2=0;
		}
		//printf("OK\n");
		
/*输出占空比和PWM寄存器CCRx的值到LCD*/
		 led0pwmval=pid_realize(Keytem);
		
		TIM_SetCompare2(TIM3,led0pwmval);//PWM输出
		
		LCD_ShowString(60,210,200,16,16,"PWMVAL:");
		LCD_ShowxNum(150,210,led0pwmval,4,16,0);
		dr=(led0pwmval/1440.0)*100;
		LCD_ShowString(60,230,200,16,16,"DutyCycle:");
		LCD_ShowxNum(140,230,dr,4,16,0);
	
		
/*输出控制温度*/		
	
POINT_COLOR=BROWN;
LCD_ShowString(60,350,200,50,16,"Tempreture:");
LCD_ShowxNum(145,350,Keytem,3,24,0);

/*画出两个触摸点图像*/
POINT_COLOR=BLACK;
LCD_Draw_Circle(400,600,40);  
LCD_ShowChar(395,590,'+',24,1); //+号
LCD_Draw_Circle(100,600,40);
LCD_ShowChar(95,590,'-',24,1); //-号

/*
gui_fill_circle(400,600,40,BLACK);
gui_fill_circle(100,600,40,BLACK);
深色触摸按钮
*/



/*触摸和按键改变温度值
50ms扫描一次
*/
	
	if(tp_dev.touchtype&0X80)	//电容屏触摸
		
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
LCD_ShowChar(395,590,'+',24,1); //+号
LCD_Draw_Circle(100,600,40);
LCD_ShowChar(95,590,'-',24,1); //-号

		
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

	/*		
		if(flag_send==1)	
		{
			*s=USART_ReceiveData(USART2);
			sscanf(*s,"%d",&s);
			if()
			flag_send=0;
		}	
			*/
	delay_ms(50);
	}//while(1) end
 
}//main end

 
