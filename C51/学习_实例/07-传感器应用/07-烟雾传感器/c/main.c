/*------------------------------------------------
  功能：烟雾传感器（MQ-2）
  硬件：MQ-2模拟输出接ADC0832通道1，P0口接数码管
  说明：检测烟雾浓度，超过阈值蜂鸣器报警
------------------------------------------------*/
#include <reg51.h>

sbit CS = P1^3;
sbit CLK = P1^2;
sbit DIO = P1^1;
sbit BEEP = P1^5;
sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

#define ALARM_THRESHOLD 150  // 报警阈值

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

void Display(unsigned char dat)
{
    unsigned char bai = dat / 100;
    unsigned char shi = dat % 100 / 10;
    unsigned char ge = dat % 10;

    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFB; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[bai]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xF7; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[shi]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xEF; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[ge]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    unsigned char smoke_val;

    BEEP = 1;  // 初始静音

    while (1)
    {
        smoke_val = Read_ADC0832(1);  // 读取通道1
        Display(smoke_val);

        if (smoke_val > ALARM_THRESHOLD)
        {
            BEEP = 0;    // 蜂鸣器报警
        }
        else
        {
            BEEP = 1;    // 关闭报警
        }
    }
}
