/*------------------------------------------------
  功能：光敏电阻检测（ADC0832）
  硬件：ADC0832接P1口，P0口接数码管
  说明：读取光照强度，数码管显示AD值
------------------------------------------------*/
#include <reg51.h>

sbit CS = P1^3;
sbit CLK = P1^2;
sbit DIO = P1^1;

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void Delay(unsigned int t)
{
    while (--t);
}

// 读取ADC0832
unsigned char Read_ADC0832(unsigned char channel)
{
    unsigned char i, dat = 0;

    CS = 0;
    CLK = 0; DIO = 1;  // 起始位
    CLK = 1; CLK = 0;

    DIO = channel;      // 通道选择
    CLK = 1; CLK = 0;

    DIO = 1;            // 释放数据线
    CLK = 1; CLK = 0;

    // 读取8位数据（MSB先）
    for (i = 0; i < 8; i++)
    {
        CLK = 1; CLK = 0;
        dat <<= 1;
        if (DIO) dat |= 0x01;
    }

    CS = 1;
    return dat;
}

void Display(unsigned char dat)
{
    unsigned char bai = dat / 100;
    unsigned char shi = dat % 100 / 10;
    unsigned char ge = dat % 10;

    // 百位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFB; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[bai]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 十位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xF7; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[shi]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 个位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xEF; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[ge]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    unsigned char ad_val;

    while (1)
    {
        ad_val = Read_ADC0832(0);  // 读取通道0
        Display(ad_val);
    }
}
