/*------------------------------------------------
  功能：24C02记忆开机次数
  硬件：24C02接I2C，P0口接数码管
  说明：每次开机次数加1并保存，掉电不丢失
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

#define ADDR_WRITE 0xA0
#define ADDR_READ  0xA1

void Delay(unsigned int t) { while (--t); }
void DelayUs(unsigned char us) { while (us--); }

void I2C_Start(void) { SDA=1; SCL=1; DelayUs(5); SDA=0; DelayUs(5); SCL=0; }
void I2C_Stop(void) { SDA=0; SCL=1; DelayUs(5); SDA=1; DelayUs(5); }

void I2C_SendByte(unsigned char dat)
{
    unsigned char i;
    for (i=0; i<8; i++) { SDA=(dat&0x80)>>7; dat<<=1; SCL=1; DelayUs(5); SCL=0; DelayUs(5); }
    SDA=1; SCL=1; DelayUs(5); SCL=0;
}

unsigned char I2C_ReadByte(void)
{
    unsigned char i, dat=0;
    SDA=1;
    for (i=0; i<8; i++) { SCL=1; DelayUs(5); dat<<=1; if(SDA) dat|=1; SCL=0; DelayUs(5); }
    return dat;
}

void Write24C02(unsigned char addr, unsigned char dat)
{
    I2C_Start(); I2C_SendByte(ADDR_WRITE); I2C_SendByte(addr); I2C_SendByte(dat); I2C_Stop();
    Delay(100);
}

unsigned char Read24C02(unsigned char addr)
{
    unsigned char dat;
    I2C_Start(); I2C_SendByte(ADDR_WRITE); I2C_SendByte(addr); I2C_Stop();
    I2C_Start(); I2C_SendByte(ADDR_READ); dat=I2C_ReadByte(); I2C_Stop();
    return dat;
}

void Display(unsigned char num)
{
    P0=0x00; LATCH1=1; LATCH1=0;
    P0=0xFE; LATCH2=1; LATCH2=0;
    P0=duanma[num/10]; LATCH1=1; LATCH1=0;
    Delay(5);

    P0=0x00; LATCH1=1; LATCH1=0;
    P0=0xFD; LATCH2=1; LATCH2=0;
    P0=duanma[num%10]; LATCH1=1; LATCH1=0;
    Delay(5);
}

void main(void)
{
    unsigned char count;

    count = Read24C02(0x00);  // 读取上次的值
    count++;                   // 加1
    Write24C02(0x00, count);   // 保存

    while (1)
    {
        Display(count);
    }
}
