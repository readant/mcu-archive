/*------------------------------------------------
  功能：定时器0模式2（8位自动重装）
  硬件：P1.0口接LED
  说明：模式2自动重装TH0值，无需手动赋值
        适合精确的串口波特率生成
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;

void Init_Timer0(void)
{
    TMOD |= 0x02;   // 定时器0，模式2（8位自动重装）
    TH0 = 0x06;     // 重装值：256-6=250，约250us@12MHz
    TL0 = 0x06;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void Timer0_isr(void) interrupt 1
{
    // 模式2自动重装，无需手动赋值TH0/TL0
    static unsigned int count = 0;

    count++;
    if (count >= 2000)      // 2000 x 250us = 500ms
    {
        count = 0;
        LED = !LED;
    }
}

void main(void)
{
    Init_Timer0();
    while (1);
}
