#include "ad7745.h"
#include "myiic.h"
#include "delay.h"
#include "lcd.h"


u8 CapResult_H,CapResult_M,CapResult_L;


//�� PB6 ��״̬
//��ʼ��IO�ڣ���λ�����üĴ���
void AD7745_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;          //RDY��������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       //��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
  GPIO_Init(GPIOB, &GPIO_InitStructure);             //��ʼ��GPIOB6	
	
	IIC_Init();      //��ʼ��IIC
	
	while(AD7745_Reset())//��ʼ��OV2640
	{
		LCD_ShowString(30,50,240,16,16,"AD7745 ERROR");
	}
	 LCD_ShowString(30,50,240,16,16,"AD7745 fuweiOK");  
  
	AD7745_WriteOneByte(0x07,0x80);   // 1000 0000   ��ʱ�̶�
	AD7745_WriteOneByte(0x09,0x1B);   // 0001 1011 ����CB��CA�����෴����ѹ����Ϊ Vdd/20x1B
	AD7745_WriteOneByte(0x0A,0x39);   // 0011 1001  ת��ʱ�� 109ms���,����ת��������ת����0x3A
	AD7745_WriteOneByte(0x0B,0xB8);   // �Ŵ��� 5��  0xB8  0xC8
	AD7745_WriteOneByte(0x0C,0x80);   // ����ֵ��0x80(0pf), 0x00(�ر�), ����ֱ�Ӳ���
}

//ָ���Ĵ���д��һ���ֽڵ�����
void AD7745_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
	IIC_Start();
	IIC_Send_Byte(0x90);    //AD7745��ַ
	if(IIC_Wait_Ack()==0)   //�ȴ�Ӧ��
		IIC_Send_Byte(WriteAddr);   //д��ַ
	if(IIC_Wait_Ack()==0)
		IIC_Send_Byte(DataToWrite);   //д�Ĵ���
	 IIC_Wait_Ack();         //���޸�
	 IIC_Stop();//����һ��ֹͣ���� 
	 delay_us(10);	 
}

//���Ĵ�����ַ
u8 AD7745_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;		  	    																 
  IIC_Start();  
	IIC_Send_Byte(0x90);  //доƬ��ַ
	if(IIC_Wait_Ack()==0)   //�ȴ�Ӧ��
	IIC_Send_Byte(ReadAddr);   //д��ַ
	
	if(IIC_Wait_Ack()==0)   //�ȴ�Ӧ��
	{
		IIC_Start();  	 	   
	  IIC_Send_Byte(0X91);  //�����������״̬
	}
	IIC_Wait_Ack(); 
	temp=IIC_Read_Byte(0);   //������Ӧ��
	IIC_Stop();    //����һ��ֹͣλ
	return temp; 
}

//������ͨ����ֵ,�����������Ĵ���
void AD7745_Read_Cap(void)
{
	IIC_Start();  
	IIC_Send_Byte(0x90);  //доƬ��ַ
	if(IIC_Wait_Ack()==0)   //�ȴ�Ӧ��
	IIC_Send_Byte(0x01);
	IIC_Wait_Ack();
	IIC_Start();  
	IIC_Send_Byte(0x91);  //�����������״̬
	if(IIC_Wait_Ack()==0) 
	CapResult_H=IIC_Read_Byte(1);  //��������Ӧ��
	
	CapResult_M=IIC_Read_Byte(1);
	CapResult_L=IIC_Read_Byte(0);
	IIC_Stop();
}

//��λAD7745 ����0���ɹ�
u8 AD7745_Reset(void)
{
	IIC_Start();
	IIC_Send_Byte(0x90);   //доƬ��ַ
	if(IIC_Wait_Ack()==0)  //�ȴ�Ӧ��
  {
		IIC_Send_Byte(0xBF);   //��λ�ź�
		
	}		
	else return 1;
	IIC_Stop();
	delay_us(500);
	return 0;
}

