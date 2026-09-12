/*------------------------------------------------
  功能：DS1302实时时钟
  硬件：DS1302接P3.4(SCLK)/P3.5(IO)/P3.6(CE)
  说明：读取DS1302时间并在数码管显示
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

unsigned char time_buf[6];  // 秒、分、时、日、月、年

void Delay(unsigned int t) { while (--t); }

// DS1302写一个字节
void DS1302_WriteByte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        IO = dat & 0x01;
        SCLK = 1;
        SCLK = 0;
        dat >>= 1;
    }
}

// DS1302读一个字节
unsigned char DS1302_ReadByte(void)
{
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat >>= 1;
        if (IO) dat |= 0x80;
        SCLK = 1;
        SCLK = 0;
    }
    return dat;
}

// 写寄存器
void DS1302_Write(unsigned char addr, unsigned char dat)
{
    CE = 0; SCLK = 0; CE = 1;
    DS1302_WriteByte(addr);
    DS1302_WriteByte(dat);
    CE = 0;
}

// 读寄存器
unsigned char DS1302_Read(unsigned char addr)
{
    unsigned char dat;
    CE = 0; SCLK = 0; CE = 1;
    DS1302_WriteByte(addr | 0x01);
    dat = DS1302_ReadByte();
    CE = 0;
    return dat;
}

// 读取时间
void ReadTime(void)
{
    time_buf[0] = DS1302_Read(0x80);  // 秒
    time_buf[1] = DS1302_Read(0x82);  // 分
    time_buf[2] = DS1302_Read(0x84);  // 时
    time_buf[3] = DS1302_Read(0x86);  // 日
    time_buf[4] = DS1302_Read(0x88);  // 月
    time_buf[5] = DS1302_Read(0x8C);  // 年
}

void Display(void)
{
    unsigned char i;
    unsigned char disp[6];

    disp[0] = time_buf[2] / 16;   // 时十位
    disp[1] = time_buf[2] % 16;   // 时个位
    disp[2] = time_buf[1] / 16;   // 分十位
    disp[3] = time_buf[1] % 16;   // 分个位
    disp[4] = time_buf[0] / 16;   // 秒十位
    disp[5] = time_buf[0] % 16;   // 秒个位

    for (i = 0; i < 6; i++)
    {
        P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
        P0 = weima[i]; LATCH2 = 1; LATCH2 = 0;
        P0 = duanma[disp[i]];
        if (i == 1 || i == 3) P0 |= 0x80;  // 加冒号
        LATCH1 = 1; LATCH1 = 0;
        Delay(5);
    }
}

void main(void)
{
    // 初始化DS1302（取消写保护）
    DS1302_Write(0x8E, 0x00);

    while (1)
    {
        ReadTime();
        Display();
    }
}
