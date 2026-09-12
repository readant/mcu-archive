/*------------------------------------------------
  功能：数码管温度显示（模拟）
  硬件：4位数码管，P0口接段选
  说明：模拟温度值显示，格式 XX.X°C
        实际项目中替换为DS18B20读取
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7};

// 模拟温度值（实际项目用DS18B20读取）
unsigned int temp_val = 256;  // 25.6°C（放大10倍存储）

void Delay(unsigned int t)
{
    while (--t);
}

void Display(void)
{
    unsigned char buf[4];
    unsigned char i;
    unsigned int val = temp_val;

    // 拆分各位
    buf[0] = val / 100;         // 十位
    buf[1] = val % 100 / 10;    // 个位
    buf[2] = val % 10;          // 小数位

    for (i = 0; i < 3; i++)
    {
        P0 = 0x00;
        LATCH1 = 1; LATCH1 = 0;
        P0 = weima[i];
        LATCH2 = 1; LATCH2 = 0;

        // 第2位加小数点
        if (i == 1)
            P0 = duanma[buf[i]] | 0x80;
        else
            P0 = duanma[buf[i]];

        LATCH1 = 1; LATCH1 = 0;
        Delay(5);
    }

    // 第4位显示°C符号（简化为C）
    P0 = 0x00;
    LATCH1 = 1; LATCH1 = 0;
    P0 = weima[3];
    LATCH2 = 1; LATCH2 = 0;
    P0 = 0x39;  // C的段码
    LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    while (1)
    {
        Display();

        // 模拟温度变化（实际项目替换为DS18B20读取）
        // temp_val = Read_DS18B20();
    }
}
