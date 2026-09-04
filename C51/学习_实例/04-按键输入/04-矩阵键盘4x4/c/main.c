/*------------------------------------------------
  功能：4x4矩阵键盘扫描
  硬件：P1口接4x4矩阵键盘，P0口接数码管
  说明：扫描矩阵键盘，按下按键数码管显示对应数字
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F,
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71  // A~F
};

// 键值映射表
unsigned char code keymap[] = {
    0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 10, 11,
    12, 13, 14, 15
};

unsigned char KeyScan(void)
{
    unsigned char row, col;
    unsigned char temp;

    for (row = 0; row < 4; row++)
    {
        P1 = ~(0x01 << row);  // 扫描某一行

        temp = P1 & 0xF0;     // 读取列状态
        if (temp != 0xF0)     // 有按键按下
        {
            for (col = 0; col < 4; col++)
            {
                if (!(temp & (0x10 << col)))
                {
                    return keymap[row * 4 + col];
                }
            }
        }
    }
    return 0xFF;  // 无按键按下
}

void Display(unsigned char num)
{
    P0 = 0x00;
    LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFE;
    LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[num];
    LATCH1 = 1; LATCH1 = 0;
}

void main(void)
{
    unsigned char key;

    while (1)
    {
        key = KeyScan();
        if (key != 0xFF)
        {
            Display(key);
        }
    }
}
