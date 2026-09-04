/*
 * 外部中断边沿计数
 * 功能：通过外部中断0对P3.2输入的脉冲进行计数
 * 硬件：信号源接P3.2，数码管显示计数值
 * 说明：外部中断可以配置为下降沿触发或低电平触发
 *       本例使用下降沿触发，对输入脉冲计数
 *       展示定时器/计数器的外部脉冲计数功能
 */
#include <reg51.h>

unsigned int pulse_count = 0;

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
 * 外部中断0中断服务函数
 * 每检测到一个下降沿，计数+1
 */
void int0_isr(void) interrupt 0
{
    pulse_count++;
}

void display_count(unsigned int cnt)
{
    unsigned char seg[4];
    seg[0] = cnt / 1000 % 10;
    seg[1] = cnt / 100 % 10;
    seg[2] = cnt / 10 % 10;
    seg[3] = cnt % 10;

    P2 = 0xFE; P0 = SEG_TABLE[seg[0]]; delay_ms(2);
    P2 = 0xFD; P0 = SEG_TABLE[seg[1]]; delay_ms(2);
    P2 = 0xFB; P0 = SEG_TABLE[seg[2]]; delay_ms(2);
    P2 = 0xF7; P0 = SEG_TABLE[seg[3]]; delay_ms(2);
}

void main(void)
{
    IT0 = 1;  // 下降沿触发
    EX0 = 1;  // 使能外部中断0
    EA = 1;   // 开总中断

    while (1)
    {
        display_count(pulse_count);
    }
}
