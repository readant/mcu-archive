/*------------------------------------------------
  功能：外部中断电平触发
  硬件：P3.2接按键（INT0），P1.0接LED
  说明：按住按键LED亮，松开LED灭（电平触发）
        与边沿触发的区别：电平触发在整个低电平期间持续触发
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;
sbit KEY = P3^2;

void Init_INT0(void)
{
    IT0 = 0;  // 电平触发（0=低电平触发，1=下降沿触发）
    EX0 = 1;
    EA = 1;
}

// 外部中断0服务函数
void INT0_isr(void) interrupt 0
{
    if (KEY == 0)   // 按键按下（低电平）
    {
        LED = 0;    // 点亮LED
    }
    else
    {
        LED = 1;    // 熄灭LED
    }
}

void main(void)
{
    Init_INT0();
    LED = 1;  // 初始熄灭

    while (1);
}
