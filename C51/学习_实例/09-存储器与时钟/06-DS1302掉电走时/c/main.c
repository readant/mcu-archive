/*------------------------------------------------
  功能：DS1302掉电走时
  硬件：DS1302接P3.4~P3.6，备用电池接VCC1
  说明：设置时间后，掉电再上电时间继续走
        重点：备用电池的接法
------------------------------------------------*/
#include <reg51.h>

sbit SCLK = P3^4;
sbit IO = P3^5;
sbit CE = P3^6;
sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF};

void Delay(unsigned int t) { while (--t); }

void DS1302_WriteByte(unsigned char dat)
{
    unsigned char i;
    for (i=0; i<8; i++) { IO=dat&0x01; SCLK=1; SCLK=0; dat>>=1; }
}

unsigned char DS1302_ReadByte(void)
{
    unsigned char i, dat=0;
    for (i=0; i<8; i++) { dat>>=1; if(IO) dat|=0x80; SCLK=1; SCLK=0; }
    return dat;
}

void DS1302_Write(unsigned char addr, unsigned char dat)
{
    CE=0; SCLK=0; CE=1;
    DS1302_WriteByte(addr); DS1302_WriteByte(dat);
    CE=0;
}

unsigned char DS1302_Read(unsigned char addr)
{
    unsigned char dat;
    CE=0; SCLK=0; CE=1;
    DS1302_WriteByte(addr|0x01); dat=DS1302_ReadByte();
    CE=0;
    return dat;
}

void SetTime(unsigned char hour, unsigned char min, unsigned char sec)
{
    DS1302_Write(0x8E, 0x00);  // 关写保护
    DS1302_Write(0x80, sec);
    DS1302_Write(0x82, min);
    DS1302_Write(0x84, hour);
    DS1302_Write(0x8E, 0x80);  // 开写保护
}

void Display(void)
{
    unsigned char i;
    unsigned char disp[6];
    unsigned char sec, min, hour;

    sec = DS1302_Read(0x80);
    min = DS1302_Read(0x82);
    hour = DS1302_Read(0x84);

    disp[0] = hour / 16;
    disp[1] = hour % 16;
    disp[2] = min / 16;
    disp[3] = min % 16;
    disp[4] = sec / 16;
    disp[5] = sec % 16;

    for (i=0; i<6; i++)
    {
        P0=0x00; LATCH1=1; LATCH1=0;
        P0=weima[i]; LATCH2=1; LATCH2=0;
        P0=duanma[disp[i]];
        if (i==1 || i==3) P0|=0x80;
        LATCH1=1; LATCH1=0;
        Delay(5);
    }
}

void main(void)
{
    // 首次使用设置时间（后续掉电会继续走）
    // SetTime(0x12, 0x00, 0x00);  // 12:00:00

    DS1302_Write(0x8E, 0x00);  // 确保写保护关闭

    while (1)
    {
        Display();
    }
}
