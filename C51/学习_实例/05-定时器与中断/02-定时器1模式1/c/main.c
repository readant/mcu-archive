/*------------------------------------------------
  功能：定时器1模式1基础
  硬件：P1.1口接LED
  说明：定时器1中断方式，每200ms翻转LED
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^1;

void Init_Timer1(void)
{
    TMOD |= 0x10;   // 定时器1，模式1（16位）
    TH1 = (65536 - 50000) / 256;
    TL1 = (65536 - 50000) % 256;
    EA = 1;
    ET1 = 1;
    TR1 = 1;
}

void Timer1_isr(void) interrupt 3
{
    static unsigned char count = 0;

    TH1 = (65536 - 50000) / 256;
    TL1 = (65536 - 50000) % 256;

    count++;
    if (count >= 4)      // 4 x 50ms = 200ms
    {
        count = 0;
        LED = !LED;
    }
}

void main(void)
{
    Init_Timer1();
    while (1);
}
