/*------------------------------------------------
  功能：双定时器同时工作
  硬件：P1.0接LED1，P1.1接LED2
  说明：定时器0控制LED1（500ms），定时器1控制LED2（200ms）
------------------------------------------------*/
#include <reg51.h>

sbit LED1 = P1^0;
sbit LED2 = P1^1;

void Init_Timers(void)
{
    // 定时器0：500ms
    TMOD |= 0x01;
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    // 定时器1：50ms
    TMOD |= 0x10;
    TH1 = (65536 - 50000) / 256;
    TL1 = (65536 - 50000) % 256;

    EA = 1;
    ET0 = 1;
    ET1 = 1;
    TR0 = 1;
    TR1 = 1;
}

void Timer0_isr(void) interrupt 1
{
    static unsigned char count0 = 0;

    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    count0++;
    if (count0 >= 10)       // 10 x 50ms = 500ms
    {
        count0 = 0;
        LED1 = !LED1;
    }
}

void Timer1_isr(void) interrupt 3
{
    static unsigned char count1 = 0;

    TH1 = (65536 - 50000) / 256;
    TL1 = (65536 - 50000) % 256;

    count1++;
    if (count1 >= 4)        // 4 x 50ms = 200ms
    {
        count1 = 0;
        LED2 = !LED2;
    }
}

void main(void)
{
    Init_Timers();
    while (1);
}
