/*------------------------------------------------
  功能：ADC0832多通道采集
  硬件：ADC0832接P1口，按键切换通道
  说明：4个按键分别选择4个通道，数码管显示
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

unsigned char Read_ADC0832(unsigned char channel)
{
    unsigned char i, dat = 0;

    CS = 0;
    CLK = 0; DIO = 1;
    CLK = 1; CLK = 0;

    DIO = channel;
    CLK = 1; CLK = 0;

    DIO = 1;
    CLK = 1; CLK = 0;

    for (i = 0; i < 8; i++)
    {
        CLK = 1; CLK = 0;
        dat <<= 1;
        if (DIO) dat |= 0x01;
    }

    CS = 1;
    return dat;
}

void Display(unsigned char ch, unsigned char dat)
{
    // 显示通道号
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFE; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[ch]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 显示百位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFD; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[dat / 100]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 显示十位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFB; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[dat % 100 / 10]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 显示个位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xF7; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[dat % 10]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    unsigned char channel = 0;
    unsigned char ad_val;

    while (1)
    {
        // 检测按键切换通道
        if (P2_0 == 0) { Delay(20); if (P2_0 == 0) { channel = 0; while (!P2_0); } }
        if (P2_1 == 0) { Delay(20); if (P2_1 == 0) { channel = 1; while (!P2_1); } }
        if (P2_2 == 0) { Delay(20); if (P2_2 == 0) { channel = 2; while (!P2_2); } }
        if (P2_3 == 0) { Delay(20); if (P2_3 == 0) { channel = 3; while (!P2_3); } }

        ad_val = Read_ADC0832(channel);
        Display(channel, ad_val);
    }
}
