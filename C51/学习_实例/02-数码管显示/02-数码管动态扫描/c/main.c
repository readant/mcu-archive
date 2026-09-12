/*------------------------------------------------
  功能：数码管动态扫描（8位）
  硬件：P0口接段选，P2.0/P2.1接锁存器
  说明：利用视觉暂留，轮流点亮8个数码管
        每个数码管显示不同的数字
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;  // 段选锁存
sbit LATCH2 = P2^1;  // 位选锁存

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

// 位选码：对应8个数码管
unsigned char code weima[] = {
    0xFE, 0xFD, 0xFB, 0xF7,
    0xEF, 0xDF, 0xBF, 0x7F
};

// 要显示的数据：87654321
unsigned char disp_buf[] = {8, 7, 6, 5, 4, 3, 2, 1};

void Delay(unsigned int t)
{
    while (--t);
}

void Display(unsigned char pos)
{
    // 消隐：先关显示
    P0 = 0x00;
    LATCH1 = 1;
    LATCH1 = 0;

    // 位选
    P0 = weima[pos];
    LATCH2 = 1;
    LATCH2 = 0;

    // 段选
    P0 = duanma[disp_buf[pos]];
    LATCH1 = 1;
    LATCH1 = 0;

    Delay(5);  // 短暂延时，保持亮度
}

void main(void)
{
    unsigned char i;

    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            Display(i);
        }
    }
}
