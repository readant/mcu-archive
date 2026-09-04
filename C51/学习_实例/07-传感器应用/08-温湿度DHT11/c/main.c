/*------------------------------------------------
  功能：温湿度DHT11
  硬件：DHT11数据线接P2.2，P0口接数码管
  说明：读取温湿度值并显示
------------------------------------------------*/
#include <reg51.h>

sbit DHT11 = P2^2;
sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char humidity, temperature;  // 湿度、温度

void Delay(unsigned int t)
{
    while (--t);
}

void DelayUs(unsigned char us)
{
    while (us--);
}

// DHT11起始信号
void DHT11_Start(void)
{
    DHT11 = 1;
    DelayUs(2);
    DHT11 = 0;
    Delay(20);
    DHT11 = 1;
    DelayUs(30);
}

// 读取一个字节
unsigned char DHT11_ReadByte(void)
{
    unsigned char i, dat = 0;

    for (i = 0; i < 8; i++)
    {
        while (!DHT11);        // 等待高电平
        DelayUs(30);
        dat <<= 1;
        if (DHT11) dat |= 1;  // 高电平时间长表示1
        while (DHT11);        // 等待低电平
    }
    return dat;
}

// 读取温湿度
bit DHT11_Read(void)
{
    unsigned char buf[5];
    unsigned char i;

    DHT11_Start();

    if (!DHT11)               // DHT11响应
    {
        while (!DHT11);       // 等待响应结束
        while (DHT11);        // 等待数据开始

        for (i = 0; i < 5; i++)
            buf[i] = DHT11_ReadByte();

        while (!DHT11);       // 等待结束

        // 校验
        if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4])
        {
            humidity = buf[0];
            temperature = buf[2];
            return 1;         // 读取成功
        }
    }
    return 0;                 // 读取失败
}

void Display(unsigned char hum, unsigned char temp)
{
    // 湿度十位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFE; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[hum / 10]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 湿度个位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFD; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[hum % 10]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 温度十位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFB; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[temp / 10]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    // 温度个位
    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xF7; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[temp % 10]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    while (1)
    {
        if (DHT11_Read())
        {
            Display(humidity, temperature);
        }
        Delay(50000);  // DHT11采样间隔>1秒
    }
}
