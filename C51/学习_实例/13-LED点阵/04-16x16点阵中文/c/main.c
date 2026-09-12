/*------------------------------------------------
  功能：16x16 LED点阵显示中文
  硬件：行线接P0+P2，列线接P1+P3
  说明：显示中文"你"字（16x16点阵）
------------------------------------------------*/
#include <reg51.h>

#define ROW_HIGH P0
#define ROW_LOW P2
#define COL_HIGH P1
#define COL_LOW P3

// 中文"你"的16x16点阵数据
unsigned char code Ni[] = {
    0x00,0x00,0x20,0x00,0x24,0x80,0x24,0x80,
    0x24,0x80,0x3F,0xF8,0x44,0x80,0x44,0x80,
    0x44,0x80,0x44,0x80,0x44,0x80,0x44,0x80,
    0x44,0x80,0x44,0x80,0x40,0x80,0x40,0x00
};

void Delay(unsigned int t)
{
    while (--t);
}

void DisplayChinese(unsigned char *dat)
{
    unsigned char i;

    for (i = 0; i < 16; i++)
    {
        // 高8行
        ROW_HIGH = 0x01 << i;
        ROW_LOW = 0x00;
        COL_HIGH = ~dat[i * 2];
        COL_LOW = ~dat[i * 2 + 1];
        Delay(200);

        // 消隐
        COL_HIGH = 0xFF;
        COL_LOW = 0xFF;
    }
}

void main(void)
{
    while (1)
    {
        DisplayChinese(Ni);
    }
}
