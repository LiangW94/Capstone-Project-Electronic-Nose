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
//ALIENTEKminiSTM32��������չʵ�� 
//ATK-HC05��������ģ��ʵ��-�⺯���汾  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
//ALIENTEKս��STM32������ʵ��13
//TFTLCD��ʾʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
 	
	
	//��ʾATK-HC05ģ�������״̬
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(30,140,200,16,16,"ROLE:Master");	//����
	else LCD_ShowString(30,140,200,16,16,"ROLE:Slave ");			 		//�ӻ�
}
//��ʾATK-HC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)LCD_ShowString(120,140,120,16,16,"STA:Connected ");			//���ӳɹ�
	else LCD_ShowString(120,140,120,16,16,"STA:Disconnect");	 			//δ����				 
}	  
int main(void)
 {	 
	u8 t;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0;
	 
	u16 adcx;
	float temp;
	//u16 show_voltage;
	u16 voltage_send;

	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();				//��ʼ������
	LCD_Init();				//��ʼ��LCD
 	Adc_Init();		  		//ADC��ʼ��	
	 
 //---------------------------- ADC//
	 

	 
	 
 //---------------------------- ADC//
   
	POINT_COLOR=GREEN;
	LCD_ShowString(30,30,200,16,16,"University of Technology Sydney");	
	 LCD_ShowString(30,50,200,16,16,"Capstone project");	
	LCD_ShowString(30,70,200,16,16,"Liang Wang 12086928");
	while(HC05_Init()) 		//��ʼ��ATK-HC05ģ��  
	{
		LCD_ShowString(30,90,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,90,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	
	
	LCD_ShowString(30,90,200,16,16,"WK_UP:ROLE KEY0:SEND/STOP");  
	LCD_ShowString(30,110,200,16,16,"ATK-HC05 Standby!");  
  	LCD_ShowString(10,160,200,16,16,"Send:");	
	LCD_ShowString(10,180,200,16,16,"Receive:");	
	
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,260,200,16,16,"ADC_CH1_VAL:");	      
	LCD_ShowString(60,280,200,16,16,"ADC_CH1_VOL:0.000V");	

	POINT_COLOR=BLUE;
	HC05_Role_Show();	  
 	while(1) 
	{		
		adcx=Get_Adc_Average(ADC_Channel_1,10);
    voltage_send = adcx;
		LCD_ShowxNum(156,260,adcx,4,16,0);//��ʾADC��ֵ
		temp=(float)adcx*(3.3/4096);
		
			//	show_voltage = temp;
		
		adcx=temp;
		
		LCD_ShowxNum(156,280,adcx,1,16,0);//��ʾ��ѹֵ
	  
		temp-=adcx;

		temp*=1000;
		LCD_ShowxNum(172,280,temp,3,16,0X80);
		
				//LCD_ShowxNum(156,300,show_voltage,5,16,0);
				
		LED0=!LED0;
	
		key=KEY_Scan(0);
		if(key==WKUP_PRES)						//�л�ģ����������
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//״̬ȡ��	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//��λATK-HC05ģ��
			}
		}
		
		else if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//����/ֹͣ����  	 
			if(sendmask==0)LCD_Fill(30+40,160,240,160+16,WHITE);//�����ʾ
		}
		
		else delay_ms(10);
		
		//if(t==50)
		if(t==1)
		{
			if(sendmask)					//��ʱ����
			{
				//sprintf((char*)sendbuf,"ALIENTEK HC05 %d Voltage is %d\r\n",sendcnt,show_voltage);
				sprintf((char*)sendbuf,"ADC on PA1: %d \r\n",voltage_send);
				
	  			//LCD_ShowString(30+40,160,200,16,16,show_voltage);	//��ʾ��������
				 LCD_ShowString(10+40,160,200,16,16,sendbuf);
				//u2_printf("ADC voltage on PA1 is%d  convert to voltage: *3.3/4096\r\n",voltage_send);		//���͵�����ģ��
				u2_printf("%d\r\n",voltage_send);
			//	u2_printf("ALIENTEK HC05 %d\r\n",show_voltage);
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART2_RX_STA&0X8000)			//���յ�һ��������
		{
			LCD_Fill(30,200,240,320,WHITE);	//�����ʾ
 			reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
		  	USART2_RX_BUF[reclen]=0;	 	//���������
			if(reclen==9||reclen==8) 		//����DS1���
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0)LED1=0;	//��LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)LED1=1;//�ر�LED1
			}
 			LCD_ShowString(30,200,209,119,16,USART2_RX_BUF);//��ʾ���յ�������
 			USART2_RX_STA=0;	 
		}	 															     				   
		t++;	
		
		//delay_ms(250);
		
		
		
	}											    
}
