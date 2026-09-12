/*------------------------------------------------
  功能：8x8点阵显示字符
  硬件：P0口接行线，P1口接列线
  说明：显示字母"A"的图案
------------------------------------------------*/
#include <reg51.h>

#define ROW_PORT P0
#define COL_PORT P1

// 字母A的点阵数据（8x8）
unsigned char code LetterA[] = {
    0x00, 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x00
};

void Delay(unsigned int t)
{
    while (--t);
}

void DisplayChar(unsigned char *dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        ROW_PORT = 0x01 << i;     // 扫描行
        COL_PORT = ~dat[i];       // 列数据（取反）
        Delay(200);
        COL_PORT = 0xFF;          // 消隐
    }
}

void main(void)
{
    while (1)
    {
        DisplayChar(LetterA);
    }
}
