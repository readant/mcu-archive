/*------------------------------------------------
  功能：单个LED闪烁
  硬件：P1.0口接LED，低电平点亮
  说明：最基础的单片机入门程序
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    while (1)
    {
        LED = 0;        // 点亮LED（低电平有效）
        Delay(50000);   // 延时约500ms（12MHz晶振）
        LED = 1;        // 熄灭LED
        Delay(50000);
    }
}
