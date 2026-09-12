/*------------------------------------------------
  功能：按键松手检测
  硬件：P2.0接按键，P0.0接LED
  说明：按下按键LED亮，松手后LED灭
        演示如何检测按键松手
------------------------------------------------*/
#include <reg51.h>

sbit KEY = P2^0;
sbit LED = P0^0;

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    LED = 1;

    while (1)
    {
        if (KEY == 0)               // 按下
        {
            Delay(20);              // 消抖
            if (KEY == 0)
            {
                LED = 0;            // 点亮
                while (KEY == 0);   // 等待松手（阻塞）
                LED = 1;            // 松手后熄灭
            }
        }
    }
}
