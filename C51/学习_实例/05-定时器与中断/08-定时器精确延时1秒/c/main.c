/*------------------------------------------------
  功能：定时器精确延时1秒
  硬件：P1.0口接LED
  说明：用定时器中断实现精确的1秒延时
        比软件Delay更精确
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;

unsigned int timer_count = 0;

void Init_Timer0(void)
{
    TMOD |= 0x01;
    TH0 = (65536 - 50000) / 256;  // 50ms
    TL0 = (65536 - 50000) % 256;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void Timer0_isr(void) interrupt 1
{
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    timer_count++;
    if (timer_count >= 20)    // 20 x 50ms = 1秒
    {
        timer_count = 0;
        LED = !LED;           // 每秒翻转一次
    }
}

void Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

void main(void)
{
    Init_Timer0();

    while (1)
    {
        // 主循环可以做其他事情
        // 定时器在后台精确计时
    }
}
