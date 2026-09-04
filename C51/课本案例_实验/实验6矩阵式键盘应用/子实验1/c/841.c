					 /************************ 必要变量定义 ************************/
#include<reg51.h>
#define uint unsigned int
sbit led0 = P1^0;    // led0 为 P1.0，以下 led1、led2、led3 均一样
sbit led1 = P1^1;
sbit led2 = P1^2;
sbit led3 = P1^3;
sbit key0 = P1^4;    //key0 为 P1.4，以下 key1、key2、key3 均一样
sbit key1 = P1^5;
sbit key2 = P1^6;
sbit key3 = P1^7;

/************************ 主程序 ************************/
void main()
{
    while(1)                    //死循环
    {
        led0 = key0; led1 = key1; led2 = key2; led3 = key3; //将 key(n)的值赋给对应的 led(n)
    }
}