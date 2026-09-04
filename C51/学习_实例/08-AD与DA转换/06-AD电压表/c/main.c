/*------------------------------------------------
  功能：AD电压表（精确版）
  硬件：ADC0832接P1口，4位数码管显示
  说明：精确显示电压值，支持0.01V精度
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

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7};

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

void Display(unsigned char ad_val)
{
    unsigned int voltage;
    unsigned char buf[4];
    unsigned char i;

    // 转换为电压值（放大100倍）
    // 电压 = AD值 * 500 / 255
    voltage = (unsigned int)ad_val * 500 / 255;

    // 拆分各位
    buf[0] = voltage / 100;         // 整数位
    buf[1] = voltage % 100 / 10;    // 小数第一位
    buf[2] = voltage % 10;          // 小数第二位
    buf[3] = 10;                    // V字符（用10代表）

    for (i = 0; i < 4; i++)
    {
        P0 = 0x00;
        LATCH1 = 1; LATCH1 = 0;
        P0 = weima[i];
        LATCH2 = 1; LATCH2 = 0;

        if (i == 1)  // 第二位加小数点
            P0 = duanma[buf[i]] | 0x80;
        else if (i == 3)
            P0 = 0x5E;  // V字符
        else
            P0 = duanma[buf[i]];

        LATCH1 = 1; LATCH1 = 0;
        Delay(5);
    }
}

void main(void)
{
    unsigned char ad_val;

    while (1)
    {
        ad_val = Read_ADC0832(0);
        Display(ad_val);
    }
}
