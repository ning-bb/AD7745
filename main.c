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
u16 Caiji[50];    //一次检测收集50个数据
u16 DuiZhao[40];  //放40张纸，包含一个空位

//单次数据采集,采集50次分析,排序滤波
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
	
	 for(i=0;i<50-1;i++)	    //冒泡排序    已经检查好用
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
	
		return temp;    //返回测得数值
}

//校准系统
//开机手动校准，测得数据放入对照数组
void Jiao_zhun(void)
{
	u8 t=0;   //提前退出标志
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
			key=KEY_Scan(0);      //按键扫描
			if(key==KEY0_PRES)
			{			
				delay_ms(1000);   //退出半米远
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
		{                             //收集数据
		  DuiZhao[i]=Maopao_lvbo();   //得到本次数据
		}
		else break;
	}
	
	 LCD_Fill(30,70,240,150+16,WHITE);  
	 LCD_Fill(30,90,240,150+16,WHITE);
	 POINT_COLOR=BLACK;
	 LCD_ShowString(30,70,230,16,16,"zheng shi ce shi");  //校准结束,开始测量
	
	 LCD_ShowString(30,110,230,16,16,"Paper count:");     //显示纸张结果
	
	 while(1)   //校准后循环检测
	 { 		 
  	 LCD_ShowString(30,130,230,16,16,"key0:qidong");
		 
		 key=KEY_Scan(0);      //按键扫描
			if(key==KEY0_PRES)  
			{				
			  LCD_Fill(30+96,110,210,150+16,WHITE);
				LCD_ShowString(30,150,230,16,16,"du qu zhong");   //读数中				
				delay_ms(300);    //退出半米远
				        				
	  	   temp=Maopao_lvbo();         //得到本次数据
									 
						 for(i=1;i<40;i++)   //从1 比较
						 {
							 if(temp==DuiZhao[i])
							 {
									LCD_ShowNum(30+96,110,i,2,16);  //显示结果数
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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
	
	LED_Init();					//初始化LED 
	BEEP_Init();
 	LCD_Init();					//LCD初始化 
	KEY_Init(); 				//按键初始化
	POINT_COLOR=RED;   
	AD7745_Init();			//IIC初始化 

	POINT_COLOR=BLACK;  
	LCD_ShowString(30,50,200,16,16,"F:");    //标题显示
	POINT_COLOR=RED;	
	LCD_ShowString(30+16,50,200,16,16,"Paper counting");
	POINT_COLOR=BLACK;
	LCD_ShowString(30,70,230,16,16,"KEY0:jiaoduiKEY1:ceshi"); 
	
	while(1)
	{	
		key=KEY_Scan(0);        //不支持连续按
		if(key==KEY0_PRES)			//直接测试,使用历史数组测量
		 {
			  LCD_Fill(30,70,210,150+16,WHITE);
		    
			 Jiao_zhun();
			 
		 }								 			
			delay_ms(5);	  
  }    	 	    
}

