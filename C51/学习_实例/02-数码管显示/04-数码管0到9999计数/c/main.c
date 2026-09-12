/*------------------------------------------------
  功能：数码管0~9999计数
  硬件：4位数码管，P0口接段选
  说明：每秒加1，从0000显示到9999
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7};

unsigned int count = 0;

void Delay(unsigned int t)
{
    while (--t);
}

void Display(unsigned int num)
{
    unsigned char i;
    unsigned char buf[4];

    // 拆分各位数字
    buf[0] = num / 1000;         // 千位
    buf[1] = num % 1000 / 100;   // 百位
    buf[2] = num % 100 / 10;     // 十位
    buf[3] = num % 10;           // 个位

    for (i = 0; i < 4; i++)
    {
        P0 = 0x00;
        LATCH1 = 1; LATCH1 = 0;
        P0 = weima[i];
        LATCH2 = 1; LATCH2 = 0;
        P0 = duanma[buf[i]];
        LATCH1 = 1; LATCH1 = 0;
        Delay(5);
    }
}

void main(void)
{
    unsigned int timer = 0;

    while (1)
    {
        Display(count);
        timer++;

        if (timer >= 5000)
        {
            timer = 0;
            count++;
            if (count >= 10000)
            {
                count = 0;
            }
        }
    }
}
