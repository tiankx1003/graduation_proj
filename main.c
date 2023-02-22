/*******************************************************************************
* 实验名			   :LCD显示实验
* 使用的IO	     : 
* 实验效果       :1602显示温度、阻值
*  注意		     ：reg51.h和reg52.h的使用
*******************************************************************************/

#include<reg51.h>
#include"lcd.h"
#include"temp.h"
#include"XPT2046.h"
//#include"studio.h"


typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit beep=P1^5;
//sbit moto=P1^0;

uchar CNCHAR[6] = "摄氏度";
void LcdDisplay();
void delay(u16 i);
void beepY();
void Dimension();

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay(u16 i)
{
	while(i--);	
}


/*******************************************************************************
* 函 数 名         : beepY
* 函数功能		   : 蜂鸣器发声
*******************************************************************************/
void beepY()
{
	u8 i;
	for(i=0;i<100;i++)
	{
	beep=~beep;
	delay(90);
	}
	beep=0;
	delay(1000);	
}


/*******************************************************************************
* 函数名         : LcdDisplay()
* 函数功能		   : LCD显示读取到的温度
* 输入           : v
* 输出         	 : 无
*******************************************************************************/
void LcdDisplay() 	 //lcd显示
{
    int temp;
	u16 temp1;
  	unsigned char datas[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //定义数组
	float tp;  
	u16 ADC,ADC1;
	static u8 i;
	temp=Ds18b20ReadTemp(); 	
	  	LcdWriteCom(0x80+0x40);		//写地址 80表示初始地址
	    LcdWriteData('+'); 		//显示正
		SBUF='+';//将接收到的数据放入到发送寄存器
		while(!TI);			         //等待发送数据完成
		TI=0;						 //清除发送完成标志位
		tp=temp;//因为数据处理有小数点所以将温度赋给一个浮点型变量
		//如果温度是正的那么，那么正数的原码就是补码它本身
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算加上0.5，还是在小数点后面。
		ADC1 = Read_AD_Data(0x94);		//   AIN0 电位器
//		ADC = ADC1*0.75;

//		temp1 = Read_AD_Data(0xE4);		//   AIN3 外部输入

	if(i==3)
	{
		i=0;
		temp1 = Read_AD_Data(0xE4);		//   AIN3 外部输入
//		temp1 = temp1-1250;
//		temp1 = temp1/6.21;
		ADC = ADC1*0.75;
		if(temp1>=1200)
	{
	   LcdWriteCom(0x8C+0x40);		 //写地址 80表示初始地址
	   LcdWriteData('+'); //显示小数第一位
	   temp1 = temp1-1200;
	   temp1 = temp1/6.21-3;
	}
	else
	{
	   	LcdWriteCom(0x8C+0x40);		 //写地址 80表示初始地址
	    LcdWriteData('-'); //显示小数第一位
		temp1 = temp1-1033;
		temp1 = temp1/3.7;
		temp1 = temp1+45-3;
		if(temp1>100) temp1 = 45; 
	}
	}
	i++;


	
	datas[0] = temp / 10000;			  //百位
	datas[1] = temp % 10000 / 1000;		  //十位
	datas[2] = temp % 1000 / 100;		  //个位
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;
	datas[5] = ADC / 1000;		  //百位
	datas[6] = ADC % 1000 / 100;		  //十位
	datas[7] = ADC % 100 / 10;			  //个位
	datas[8] = ADC % 10;				 //小数点后一位
	datas[9] = temp1/1000;//千位
	datas[10] = temp1%1000/100;//百位
	datas[11] = temp1%1000%100/10;//十位
	datas[12] = temp1%1000%100%10;//个位

/*第一行显示温度值*/
	LcdWriteCom(0x81+0x40);		  //写地址 80表示初始地址
	LcdWriteData('0'+datas[0]); //百位 
	
	LcdWriteCom(0x82+0x40);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[1]); //十位

	LcdWriteCom(0x83+0x40);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[2]); //个位 

	LcdWriteCom(0x84+0x40);		//写地址 80表示初始地址
	LcdWriteData('.'); 		//显示 ‘.’

	LcdWriteCom(0x85+0x40);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[3]); //显示小数第一位  

/*第二行显示电阻值*/
	LcdWriteCom(0x81);		  //写地址 80表示初始地址
	LcdWriteData('0'+datas[5]); //百位 
		
	LcdWriteCom(0x82);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[6]); //十位

	LcdWriteCom(0x83);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[7]); //个位 

	LcdWriteCom(0x84);		//写地址 80表示初始地址
	LcdWriteData('.'); 		//显示 ‘.’

	LcdWriteCom(0x85);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[8]); //显示小数第一位  


/*右侧显示仰俯角*/
//	LcdWriteCom(0x8C+0x40);		  //写地址 80表示初始地址
//	LcdWriteData('0'+datas[9]); //显示千位
	
	LcdWriteCom(0x8D+0x40);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[10]); //显示百位

	LcdWriteCom(0x8E+0x40);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[11]); //显示十位 

	LcdWriteCom(0x8F+0x40);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[12]); //显示个位


	 
}



/*******************************************************************************
* 函 数 名         :Dimension()
* 函数功能		   :单位显示
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Dimension()
{
	LcdWriteCom(0x87+0x40);	//写地址 80表示初始地址
	LcdWriteData('C');
	LcdWriteCom(0x8D);	
	LcdWriteData('X');	
	LcdWriteCom(0x8E);
	LcdWriteData('-');	
	LcdWriteCom(0x8F);	
	LcdWriteData('Z');	 
	LcdWriteCom(0x87);	
	LcdWriteData('P');
	LcdWriteCom(0x88);	
	LcdWriteData('P');
	LcdWriteCom(0x89);	
	LcdWriteData('M');
}

/*******************************************************************************
* 函数名         : main
* 函数功能		   : 主函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void main()
{

	u16 adc,adc1;
	int i=0;
	LcdInit();			 //初始化LCD1602
	Dimension();
//	moto=0;			//关闭电机
/*	for(i=0;i<100;i++)	  //循环100次，也就是大约5S
	{
		moto=1;			 //开启电机
		delay(5000);	//大约延时50ms
	}
	moto=0;			//关闭电机 */
	while(1)
	{
	 	adc1=Read_AD_Data(0x94);
	    adc=adc1*0.75;
		LcdDisplay();
//		moto=0;
		if(adc>=2000)	
		{
		beepY(); 
//      moto=1;
		i++;
		}
	}		  
}				
