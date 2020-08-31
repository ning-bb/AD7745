#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "ad7745.h"
#include "myiic.h"
#include "key.h"  
#include "beep.h"


u8 countmode=0;
u16 temp;
u16 CapRseult;
u16 Caiji[50];    //һ�μ���ռ�50������
u16 DuiZhao[40];  //��40��ֽ������һ����λ

//�������ݲɼ�,�ɼ�50�η���,�����˲�
u16 Maopao_lvbo(void)    
{
	u8 i,j;
	u16 temp;

	for(i=0;i<50;i++) 
	{	
		while(1)
	{	
		 if(RDY==0)  
		 {			
			AD7745_Read_Cap();
			CapRseult=CapResult_H;
			CapRseult=(CapRseult<<8)+CapResult_M;
			printf("0x%x\n\n",CapRseult);
			Caiji[i]=CapRseult;
		  CapRseult=0;		 
			delay_ms(110);
			break;
		 }	
		}
	}
	
	 for(i=0;i<50-1;i++)	    //ð������    �Ѿ�������
		for(j=0;j<50-1-i;j++)
		{
		   if(Caiji[j]>Caiji[j+1])
		   {			
			   temp=Caiji[j];
			   Caiji[j]=Caiji[j+1];
			   Caiji[j+1]=temp;
		   }	
		}	
		
    temp=Caiji[31];
		printf("temp=0x%x\n\n",temp);
	
		return temp;    //���ز����ֵ
}

//У׼ϵͳ
//�����ֶ�У׼��������ݷ����������
void Jiao_zhun(void)
{
	u8 t=0;   //��ǰ�˳���־
	u8 key;
	u8 i;
	POINT_COLOR=BLACK;
	LCD_ShowString(30,70,230,16,16,"jiao zhun shu:");
	POINT_COLOR=BRED;
	for(i=0;i<36;i++)
	{
    LCD_ShowNum(30+120,70,i,2,16);
		LCD_ShowString(30,90,230,16,16,"fang zhi:key0:OK,key1:TUI");		
		while(1)
		{
			key=KEY_Scan(0);      //����ɨ��
			if(key==KEY0_PRES)
			{			
				delay_ms(1000);   //�˳�����Զ
				LCD_Fill(30,90,240,150+16,WHITE);
				LCD_ShowString(30,90,230,16,16,"jian ce zhong");
				break;
			}
			if(key==KEY1_PRES)
			{
				t=1;
				break;
			}
		}
		if(t==0)
		{                             //�ռ�����
		  DuiZhao[i]=Maopao_lvbo();   //�õ���������
		}
		else break;
	}
	
	 LCD_Fill(30,70,240,150+16,WHITE);  
	 LCD_Fill(30,90,240,150+16,WHITE);
	 POINT_COLOR=BLACK;
	 LCD_ShowString(30,70,230,16,16,"zheng shi ce shi");  //У׼����,��ʼ����
	
	 LCD_ShowString(30,110,230,16,16,"Paper count:");     //��ʾֽ�Ž��
	
	 while(1)   //У׼��ѭ�����
	 { 		 
  	 LCD_ShowString(30,130,230,16,16,"key0:qidong");
		 
		 key=KEY_Scan(0);      //����ɨ��
			if(key==KEY0_PRES)  
			{				
			  LCD_Fill(30+96,110,210,150+16,WHITE);
				LCD_ShowString(30,150,230,16,16,"du qu zhong");   //������				
				delay_ms(300);    //�˳�����Զ
				        				
	  	   temp=Maopao_lvbo();         //�õ���������
									 
						 for(i=1;i<40;i++)   //��1 �Ƚ�
						 {
							 if(temp==DuiZhao[i])
							 {
									LCD_ShowNum(30+96,110,i,2,16);  //��ʾ�����
									 break;
							 }	
							 else if(temp<DuiZhao[i])	
							 {
									 if(i==1)  LCD_ShowNum(30+96,110,1,2,16);
										else if(temp>DuiZhao[i-1])
											LCD_ShowNum(30+96,110,i,2,16);
										break;
							 }						 
						 }			
				   BEEP=1;
				   delay_ms(100);
				   BEEP=0;			
				   LCD_Fill(30,150,210,150+16,WHITE);  
				   LCD_ShowString(30,150,230,16,16,"du qu OK");
			   }
							
		}	 
}	 


int main(void)
{ 
	u8 key;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);    //��ʼ����ʱ����
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	
	LED_Init();					//��ʼ��LED 
	BEEP_Init();
 	LCD_Init();					//LCD��ʼ�� 
	KEY_Init(); 				//������ʼ��
	POINT_COLOR=RED;   
	AD7745_Init();			//IIC��ʼ�� 

	POINT_COLOR=BLACK;  
	LCD_ShowString(30,50,200,16,16,"F:");    //������ʾ
	POINT_COLOR=RED;	
	LCD_ShowString(30+16,50,200,16,16,"Paper counting");
	POINT_COLOR=BLACK;
	LCD_ShowString(30,70,230,16,16,"KEY0:jiaoduiKEY1:ceshi"); 
	
	while(1)
	{	
		key=KEY_Scan(0);        //��֧��������
		if(key==KEY0_PRES)			//ֱ�Ӳ���,ʹ����ʷ�������
		 {
			  LCD_Fill(30,70,210,150+16,WHITE);
		    
			 Jiao_zhun();
			 
		 }								 			
			delay_ms(5);	  
  }    	 	    
}

