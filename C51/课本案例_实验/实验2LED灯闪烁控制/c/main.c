#include <reg51.h>
#define uint unsigned int
// 延时子程序
void delay()
{
    uint i,j;
    for(i = 10; i > 0; i--)
        for(j = 1000; j > 0; j--);
}

// 主程序
void main()
{
    while(1)
    {
        P2 = 0xff;          // P2口全部赋高电平
        if(P2 != 0xff)      // 有开关被按下
        {
            P2 = P2 << 4;   // 移位运算，向左移动4位，将读入的4个开关状态送给4个LED
            delay();        // 延时
        }
    }
}
