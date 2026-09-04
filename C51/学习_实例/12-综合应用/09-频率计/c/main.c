/*
 * 频率计
 * 功能：利用定时器+计数器实现频率测量
 * 硬件：信号输入接T0(P3.4)，数码管显示频率值
 * 说明：频率计是定时器和计数器的综合应用
 *       原理：在固定时间（如1秒）内计数外部脉冲数
 *       计数值即为频率（Hz）
 */
#include <reg51.h>

unsigned int freq_count = 0;
unsigned int frequency = 0;

unsigned char code SEG_TABLE[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 定时器1初始化：1秒定时
 */
void timer1_init(void)
{
    TMOD &= 0x0F;
    TMOD |= 0x10;  // T1方式1

    TH1 = 0x3C;    // 50ms @11.0592MHz
    TL1 = 0xB0;
    ET1 = 1;
    EA = 1;
    TR1 = 1;
}

/*
 * 定时器0初始化：计数模式
 */
void timer0_init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x05;  // T0方式1，外部计数

    TH0 = 0;
    TL0 = 0;
    TR0 = 1;
}

/*
 * 定时器1中断：每50ms触发一次，20次=1秒
 */
void timer1_isr(void) interrupt 3
{
    static unsigned char timer_cnt = 0;

    TH1 = 0x3C;
    TL1 = 0xB0;

    timer_cnt++;
    if (timer_cnt >= 20)  // 1秒到
    {
        timer_cnt = 0;
        frequency = (TH0 << 8) | TL0;  // 读取计数值
        TH0 = 0;  // 清零重计
        TL0 = 0;
    }
}

void display_freq(unsigned int freq)
{
    unsigned char seg[6];
    unsigned char i;

    seg[0] = freq / 100000 % 10;
    seg[1] = freq / 10000 % 10;
    seg[2] = freq / 1000 % 10;
    seg[3] = freq / 100 % 10;
    seg[4] = freq / 10 % 10;
    seg[5] = freq % 10;

    for (i = 0; i < 6; i++)
    {
        P0 = 0x00;
        P2 = ~(0x01 << i);
        P0 = SEG_TABLE[seg[i]];
        delay_ms(2);
    }
}

void main(void)
{
    timer0_init();
    timer1_init();

    while (1)
    {
        display_freq(frequency);
    }
}
