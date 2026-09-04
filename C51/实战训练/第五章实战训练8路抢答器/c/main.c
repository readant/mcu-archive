#include <reg51.h>
#define uint unsigned int
#define unchar unsigned char

uchar code table[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
uchar m = 0;

// 延时子程序
void delay(uchar c)
{
    unsigned char a,b;
    for(;c>0;c--)
        for(b=142;b>0;b--)
            for(a=2;a>0;a--);
}
// 外部中断0	 假设为主持人  优先级最高
void INT_0() interrupt 0
{
    EX0 = 0;                 
    delay(20);               
    EX0 = 1;                  //  防抖
}

/***************** 外部中断1 *****************/
void INT_1() interrupt 2
{
    EX1 = 0;                  
    delay(20);                
    EX1 = 1;                  //防抖        
}

/***************** 主程序 *****************/
void main()
{
    P1 = 0x00;
    EA = 1;                   
    EX0 = 1;                 
    IT0 = 1;                  
    EX1 = 1;                 
    IT1 = 1;                  
    while(1);                 
}