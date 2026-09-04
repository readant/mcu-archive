#include <reg51.h>
#define uchar unsigned char
#define uint unsigned int

// sbit 声明一个可位寻址变量
sbit LED = P1^0;	//取P1.0口作为点亮发光二极管的输出信号接口
//bit 声明一个位标量或一个位类型的函数
bit flag;		    //用户定义标志位，标志是否发生中断
uchar count = 0;    //计数变量
/****************延时子程序************************/
void delay(uchar c)   // 延时C毫秒
{
	unsigned char a,b;
	for(;c>0;c--)
		for(b = 142;b > 0;b--)
			for(a = 2;a > 0;a--);
}
/****************主程序****************************/
void main(void)
{	
	TMOD = 0x01;			     //设置定时器0为工作方式1
	TH0 = (65536 - 50000)/256;	 
	TL0 = (65536 - 50000)%256;	 //给定时器0赋初值50ms
	EA = 1;						 //开总中断
	ET0 = 1;					 //开定时/计数器0中断
	TR0 = 1;					 //启动定时/计数器0
	while(1)
	{
		if(flag == 1)
		{
			LED = 0;	         //点亮发光二极管
		delay(100);				 //延时100ms
		LED = 1;				 //熄灭发光二极管
		flag = 0;				 //将用户定义中断标志位flag置0,防止误判中断
		}
	}
}
/******************定时器0终中断子程序**************/
void time0(void) interrupt 1
{
	TH0 = (65536 - 50000)/256;	 
	TL0 = (65536 - 50000)%256;	 //重新给定时器赋处值
	/*重新赋处值是为了*/
	count++;					 //每中断一次count加1
	if(count >= 20)				 //每定时到1s时给用户定义中断标志flag置1
	{
		count = 0;
		flag = 1;
	}
}
