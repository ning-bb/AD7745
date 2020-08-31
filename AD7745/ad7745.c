#include "ad7745.h"
#include "myiic.h"
#include "delay.h"
#include "lcd.h"


u8 CapResult_H,CapResult_M,CapResult_L;


//用 PB6 读状态
//初始化IO口，复位，配置寄存器
void AD7745_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;          //RDY输入引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       //普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);             //初始化GPIOB6	
	
	IIC_Init();      //初始化IIC
	
	while(AD7745_Reset())//初始化OV2640
	{
		LCD_ShowString(30,50,240,16,16,"AD7745 ERROR");
	}
	 LCD_ShowString(30,50,240,16,16,"AD7745 fuweiOK");  
  
	AD7745_WriteOneByte(0x07,0x80);   // 1000 0000   暂时固定
	AD7745_WriteOneByte(0x09,0x1B);   // 0001 1011 设置CB和CA极性相反，电压设置为 Vdd/20x1B
	AD7745_WriteOneByte(0x0A,0x39);   // 0011 1001  转换时间 109ms最大,连续转换。单次转换：0x3A
	AD7745_WriteOneByte(0x0B,0xB8);   // 放大倍数 5倍  0xB8  0xC8
	AD7745_WriteOneByte(0x0C,0x80);   // 可设值：0x80(0pf), 0x00(关闭), 或者直接不用
}

//指定寄存器写入一个字节的数据
void AD7745_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
	IIC_Start();
	IIC_Send_Byte(0x90);    //AD7745地址
	if(IIC_Wait_Ack()==0)   //等待应答
		IIC_Send_Byte(WriteAddr);   //写地址
	if(IIC_Wait_Ack()==0)
		IIC_Send_Byte(DataToWrite);   //写寄存器
	 IIC_Wait_Ack();         //可修改
	 IIC_Stop();//产生一个停止条件 
	 delay_us(10);	 
}

//读寄存器地址
u8 AD7745_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;		  	    																 
  IIC_Start();  
	IIC_Send_Byte(0x90);  //写芯片地址
	if(IIC_Wait_Ack()==0)   //等待应答
	IIC_Send_Byte(ReadAddr);   //写地址
	
	if(IIC_Wait_Ack()==0)   //等待应答
	{
		IIC_Start();  	 	   
	  IIC_Send_Byte(0X91);  //主机进入接收状态
	}
	IIC_Wait_Ack(); 
	temp=IIC_Read_Byte(0);   //不产生应答
	IIC_Stop();    //产生一个停止位
	return temp; 
}

//读电容通道的值,连续读三个寄存器
void AD7745_Read_Cap(void)
{
	IIC_Start();  
	IIC_Send_Byte(0x90);  //写芯片地址
	if(IIC_Wait_Ack()==0)   //等待应答
	IIC_Send_Byte(0x01);
	IIC_Wait_Ack();
	IIC_Start();  
	IIC_Send_Byte(0x91);  //主机进入接收状态
	if(IIC_Wait_Ack()==0) 
	CapResult_H=IIC_Read_Byte(1);  //主机产生应答
	
	CapResult_M=IIC_Read_Byte(1);
	CapResult_L=IIC_Read_Byte(0);
	IIC_Stop();
}

//复位AD7745 返回0：成功
u8 AD7745_Reset(void)
{
	IIC_Start();
	IIC_Send_Byte(0x90);   //写芯片地址
	if(IIC_Wait_Ack()==0)  //等待应答
  {
		IIC_Send_Byte(0xBF);   //复位信号
		
	}		
	else return 1;
	IIC_Stop();
	delay_us(500);
	return 0;
}

