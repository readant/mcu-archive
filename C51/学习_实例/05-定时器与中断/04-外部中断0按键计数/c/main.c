/*------------------------------------------------
  功能：外部中断0按键计数
  硬件：P3.2接按键（INT0），P0口接数码管
  说明：每按一次按键，计数加1，数码管显示
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char count = 0;

void Init_INT0(void)
{
    IT0 = 1;  // 下降沿触发
    EX0 = 1;  // 开外部中断0
    EA = 1;   // 开总中断
}

// 外部中断0服务函数
void INT0_isr(void) interrupt 0
{
    count++;
    if (count >= 10)
    {
        count = 0;
    }
}

void Display(unsigned char num)
{
    P0 = 0x00;
    LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFE;           // 选中第一位
    LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[num];
    LATCH1 = 1; LATCH1 = 0;
}

void main(void)
{
    Init_INT0();

    while (1)
    {
        Display(count);
    }
}
