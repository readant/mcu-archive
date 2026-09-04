/*------------------------------------------------
  功能：PCF8591 AD采集
  硬件：PCF8591通过I2C接P2.0(SCL)/P2.1(SDA)
  说明：读取4路AD值，数码管显示
------------------------------------------------*/
#include <reg51.h>

sbit SCL = P2^0;
sbit SDA = P2^1;
sbit LATCH1 = P2^2;
sbit LATCH2 = P2^3;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

#define PCF8591_ADDR 0x90

void Delay(unsigned int t)
{
    while (--t);
}

void DelayUs(unsigned char us)
{
    while (us--);
}

// I2C起始信号
void I2C_Start(void)
{
    SDA = 1; SCL = 1;
    DelayUs(5);
    SDA = 0;
    DelayUs(5);
    SCL = 0;
}

// I2C停止信号
void I2C_Stop(void)
{
    SDA = 0; SCL = 1;
    DelayUs(5);
    SDA = 1;
    DelayUs(5);
}

// 发送一个字节
void I2C_SendByte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        SDA = (dat & 0x80) >> 7;
        dat <<= 1;
        SCL = 1;
        DelayUs(5);
        SCL = 0;
        DelayUs(5);
    }
    SDA = 1;  // 释放数据线
    SCL = 1;
    DelayUs(5);
    SCL = 0;
}

// 读取一个字节
unsigned char I2C_ReadByte(void)
{
    unsigned char i, dat = 0;

    SDA = 1;  // 释放数据线
    for (i = 0; i < 8; i++)
    {
        SCL = 1;
        DelayUs(5);
        dat <<= 1;
        if (SDA) dat |= 0x01;
        SCL = 0;
        DelayUs(5);
    }
    return dat;
}

// 读取PCF8591指定通道
unsigned char ReadPCF8591(unsigned char channel)
{
    unsigned char dat;

    I2C_Start();
    I2C_SendByte(PCF8591_ADDR);  // 写地址
    I2C_SendByte(0x40 | channel); // 控制字：使能+通道
    I2C_Stop();

    I2C_Start();
    I2C_SendByte(PCF8591_ADDR | 1);  // 读地址
    dat = I2C_ReadByte();
    I2C_Stop();

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
    unsigned char ad_val;

    while (1)
    {
        ad_val = ReadPCF8591(0);  // 读取通道0
        Display(ad_val);
    }
}
