/*------------------------------------------------
  功能：电位器电压检测
  硬件：电位器接ADC0832通道0，P0口接数码管
  说明：旋转电位器，数码管显示电压值（0~5V）
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

void DisplayVoltage(unsigned char ad_val)
{
    unsigned int voltage;
    unsigned char v_int, v_dec;

    // AD值转换为电压值（放大10倍）
    // 电压 = AD值 * 5.0 / 255 * 10 = AD值 * 50 / 255
    voltage = (unsigned int)ad_val * 50 / 255;

    v_int = voltage / 10;    // 整数部分
    v_dec = voltage % 10;    // 小数部分

    // 整数位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFD; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[v_int]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 小数位（带小数点）
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFB; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[v_dec] | 0x80; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // V字符
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xF7; LATCH2 = 1; LATCH2 = 0;
    P0 = 0x5E;  // V的段码
    LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    unsigned char ad_val;

    while (1)
    {
        ad_val = Read_ADC0832(0);
        DisplayVoltage(ad_val);
    }
}
