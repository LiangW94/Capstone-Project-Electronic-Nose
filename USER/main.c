#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "hc05.h"
#include "usart2.h"			 	 
#include "string.h"	 
#include "key.h"
#include "adc.h"
#include "stm32f10x_adc.h"
//ALIENTEKminiSTM32开发板扩展实验 
//ATK-HC05蓝牙串口模块实验-库函数版本  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
//ALIENTEK战舰STM32开发板实验13
//TFTLCD显示实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
 	
	
	//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(30,140,200,16,16,"ROLE:Master");	//主机
	else LCD_ShowString(30,140,200,16,16,"ROLE:Slave ");			 		//从机
}
//显示ATK-HC05模块的连接状态
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)LCD_ShowString(120,140,120,16,16,"STA:Connected ");			//连接成功
	else LCD_ShowString(120,140,120,16,16,"STA:Disconnect");	 			//未连接				 
}	  
int main(void)
 {	 
	u8 t;
	u16 show_voltage;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0;
	 
	u16 adcx;
	float temp;

	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();				//初始化与LED连接的硬件接口
	KEY_Init();				//初始化按键
	LCD_Init();				//初始化LCD
 	Adc_Init();		  		//ADC初始化	
	 
 //---------------------------- ADC//
	 

	 
	 
 //---------------------------- ADC//
   
	POINT_COLOR=GREEN;
	LCD_ShowString(30,30,200,16,16,"University of Technology Sydney");	
	 LCD_ShowString(30,50,200,16,16,"Capstone project");	
	LCD_ShowString(30,70,200,16,16,"Liang Wang 12086928");
	while(HC05_Init()) 		//初始化ATK-HC05模块  
	{
		LCD_ShowString(30,90,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,90,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	
	
	LCD_ShowString(30,90,200,16,16,"WK_UP:ROLE KEY0:SEND/STOP");  
	LCD_ShowString(30,110,200,16,16,"ATK-HC05 Standby!");  
  	LCD_ShowString(30,160,200,16,16,"Send:");	
	LCD_ShowString(30,180,200,16,16,"Receive:");	
	
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(60,260,200,16,16,"ADC_CH1_VAL:");	      
	LCD_ShowString(60,280,200,16,16,"ADC_CH1_VOL:0.000V");	

	POINT_COLOR=BLUE;
	HC05_Role_Show();	  
 	while(1) 
	{		
		key=KEY_Scan(0);
		if(key==WKUP_PRES)						//切换模块主从设置
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//状态取反	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//复位ATK-HC05模块
			}
		}
		
		else if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//发送/停止发送  	 
			if(sendmask==0)LCD_Fill(30+40,160,240,160+16,WHITE);//清除显示
		}
		
		else delay_ms(10);
		
		if(t==50)
		{
			if(sendmask)					//定时发送
			{
				sprintf((char*)sendbuf,"ALIENTEK HC05 %d\r\n",sendcnt);
	  			LCD_ShowString(30+40,160,200,16,16,sendbuf);	//显示发送数据	
				u2_printf("ALIENTEK HC05 %d\r\n",sendcnt);		//发送到蓝牙模块
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART2_RX_STA&0X8000)			//接收到一次数据了
		{
			LCD_Fill(30,200,240,320,WHITE);	//清除显示
 			reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
		  	USART2_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen==9||reclen==8) 		//控制DS1检测
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0)LED1=0;	//打开LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)LED1=1;//关闭LED1
			}
 			LCD_ShowString(30,200,209,119,16,USART2_RX_BUF);//显示接收到的数据
 			USART2_RX_STA=0;	 
		}	 															     				   
		t++;	
		
		
		
		
		
				adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,260,adcx,4,16,0);//显示ADC的值
		temp=(float)adcx*(3.3/4096);
		adcx=temp;
		LCD_ShowxNum(156,280,adcx,1,16,0);//显示电压值
	  show_voltage = (float)adcx;
		temp-=adcx;
		
		show_voltage += temp;

		temp*=1000;
		LCD_ShowxNum(172,280,temp,3,16,0X80);
		
		
				LCD_ShowxNum(156,300,show_voltage,5,16,0);
		
		LED0=!LED0;
		//delay_ms(250);
		
		
		
	}											    
}
