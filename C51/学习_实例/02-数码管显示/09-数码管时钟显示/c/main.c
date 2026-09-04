/*------------------------------------------------
  功能：数码管时钟显示（时:分）
  硬件：4位数码管，P0口接段选
  说明：显示格式 HH:MM，中间冒号闪烁
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7};

unsigned char hour = 12;  // 小时
unsigned char min = 0;    // 分钟
bit colon_on = 1;         // 冒号状态

void Delay(unsigned int t)
{
    while (--t);
}

void Display(void)
{
    unsigned char buf[4];
    unsigned char i;

    buf[0] = hour / 10;    // 小时十位
    buf[1] = hour % 10;    // 小时个位
    buf[2] = min / 10;     // 分钟十位
    buf[3] = min % 10;     // 分钟个位

    for (i = 0; i < 4; i++)
    {
        P0 = 0x00;
        LATCH1 = 1; LATCH1 = 0;
        P0 = weima[i];
        LATCH2 = 1; LATCH2 = 0;

        // 第2位（小时个位）加冒号
        if (i == 1 && colon_on)
            P0 = duanma[buf[i]] | 0x80;  // 加小数点当冒号
        else
            P0 = duanma[buf[i]];

        LATCH1 = 1; LATCH1 = 0;
        Delay(5);
    }
}

void main(void)
{
    unsigned int timer = 0;
    unsigned int blink = 0;

    while (1)
    {
        Display();

        timer++;
        blink++;

        // 冒号闪烁（约500ms切换一次）
        if (blink >= 2500)
        {
            blink = 0;
            colon_on = !colon_on;
        }

        // 每秒加1分钟
        if (timer >= 5000)
        {
            timer = 0;
            min++;
            if (min >= 60)
            {
                min = 0;
                hour++;
                if (hour >= 24)
                {
                    hour = 0;
                }
            }
        }
    }
}
