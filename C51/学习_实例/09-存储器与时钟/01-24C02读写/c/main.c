/*------------------------------------------------
  功能：24C02读写
  硬件：24C02通过I2C接P2.0(SCL)/P2.1(SDA)
  说明：向24C02写入一个字节，再读出来
------------------------------------------------*/
#include <reg51.h>

sbit SCL = P2^0;
sbit SDA = P2^1;

#define ADDR_WRITE 0xA0
#define ADDR_READ  0xA1

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
        SCL = 1; DelayUs(5);
        SCL = 0; DelayUs(5);
    }
    SDA = 1;
    SCL = 1; DelayUs(5);
    SCL = 0;
}

unsigned char I2C_ReadByte(void)
{
    unsigned char i, dat = 0;
    SDA = 1;
    for (i = 0; i < 8; i++)
    {
        SCL = 1; DelayUs(5);
        dat <<= 1;
        if (SDA) dat |= 0x01;
        SCL = 0; DelayUs(5);
    }
    return dat;
}

// 写一个字节
void Write24C02(unsigned char addr, unsigned char dat)
{
    I2C_Start();
    I2C_SendByte(ADDR_WRITE);
    I2C_SendByte(addr);
    I2C_SendByte(dat);
    I2C_Stop();
    Delay(100);  // 等待写入完成
}

// 读一个字节
unsigned char Read24C02(unsigned char addr)
{
    unsigned char dat;

    I2C_Start();
    I2C_SendByte(ADDR_WRITE);
    I2C_SendByte(addr);
    I2C_Stop();

    I2C_Start();
    I2C_SendByte(ADDR_READ);
    dat = I2C_ReadByte();
    I2C_Stop();

    return dat;
}

void main(void)
{
    unsigned char read_val;

    Write24C02(0x00, 0xAA);  // 写入0xAA到地址0x00
    read_val = Read24C02(0x00);  // 读出来

    while (1);
}
