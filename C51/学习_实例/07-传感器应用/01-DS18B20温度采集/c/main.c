/*------------------------------------------------
  功能：DS18B20温度采集
  硬件：DS18B20数据线接P3.7，P0口接数码管
  说明：读取DS18B20温度值并显示
------------------------------------------------*/
#include <reg51.h>

sbit DS18B20 = P3^7;
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

void DelayUs(unsigned int us)
{
    while (us--);
}

// DS18B20初始化
bit Init_DS18B20(void)
{
    bit presence;
    DS18B20 = 1;
    DelayUs(2);
    DS18B20 = 0;
    DelayUs(500);
    DS18B20 = 1;
    DelayUs(60);
    presence = DS18B20;
    DelayUs(500);
    return presence;
}

// 写一个字节
void WriteByte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DS18B20 = 0;
        DelayUs(2);
        DS18B20 = dat & 0x01;
        DelayUs(60);
        DS18B20 = 1;
        dat >>= 1;
    }
}

// 读一个字节
unsigned char ReadByte(void)
{
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        DS18B20 = 0;
        DelayUs(2);
        DS18B20 = 1;
        DelayUs(5);
        dat >>= 1;
        if (DS18B20) dat |= 0x80;
        DelayUs(60);
    }
    return dat;
}

// 读取温度值
int ReadTemp(void)
{
    unsigned char low, high;
    int temp;

    Init_DS18B20();
    WriteByte(0xCC);     // 跳过ROM
    WriteByte(0x44);     // 启动温度转换

    Delay(1000);

    Init_DS18B20();
    WriteByte(0xCC);
    WriteByte(0xBE);     // 读取寄存器

    low = ReadByte();
    high = ReadByte();

    temp = (high << 8) | low;
    return temp;
}

void Display(int temp)
{
    unsigned char ge, shi, bai;
    unsigned int abs_temp;

    if (temp < 0) abs_temp = -temp;
    else abs_temp = temp;

    bai = abs_temp / 100;
    shi = abs_temp % 100 / 10;
    ge = abs_temp % 10;

    // 十位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFD; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[shi]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 个位（带小数点）
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFB; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[ge] | 0x80; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 小数位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xF7; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[abs_temp % 10]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    int temp;

    while (1)
    {
        temp = ReadTemp();
        Display(temp);
    }
}
