/*------------------------------------------------
  功能：PCF8591 DA输出
  硬件：PCF8591通过I2C接P2.0(SCL)/P2.1(SDA)
  说明：通过DA输出模拟电压，控制LED亮度
------------------------------------------------*/
#include <reg51.h>

sbit SCL = P2^0;
sbit SDA = P2^1;

#define PCF8591_ADDR 0x90

void Delay(unsigned int t)
{
    while (--t);
}

void DelayUs(unsigned char us)
{
    while (us--);
}

void I2C_Start(void)
{
    SDA = 1; SCL = 1;
    DelayUs(5);
    SDA = 0;
    DelayUs(5);
    SCL = 0;
}

void I2C_Stop(void)
{
    SDA = 0; SCL = 1;
    DelayUs(5);
    SDA = 1;
    DelayUs(5);
}

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
    SDA = 1;
    SCL = 1;
    DelayUs(5);
    SCL = 0;
}

// 写PCF8591 DA输出
void WritePCF8591_DA(unsigned char dat)
{
    I2C_Start();
    I2C_SendByte(PCF8591_ADDR);   // 写地址
    I2C_SendByte(0x40);           // 控制字：使能DA输出
    I2C_SendByte(dat);            // DA值
    I2C_Stop();
}

void main(void)
{
    unsigned char da_val = 0;
    signed char step = 1;

    while (1)
    {
        WritePCF8591_DA(da_val);

        da_val += step;
        if (da_val >= 255)
            step = -1;
        else if (da_val == 0)
            step = 1;

        Delay(500);
    }
}
