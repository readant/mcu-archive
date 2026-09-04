/*------------------------------------------------
  功能：数码管显示字母
  硬件：1位数码管，P0口接段选
  说明：展示如何用数码管显示A~F等字母
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

// 字母段码表（共阴极）
// A=0x77, b=0x7C, C=0x39, d=0x5E, E=0x79, F=0x71
unsigned char code letter[] = {
    0x77,  // A
    0x7C,  // b
    0x39,  // C
    0x5E,  // d
    0x79,  // E
    0x71   // F
};

void Delay(unsigned int t)
{
    while (--t);
}

void Display(unsigned char code_val)
{
    P0 = 0x00;
    LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFE;           // 选中第一位
    LATCH2 = 1; LATCH2 = 0;
    P0 = code_val;
    LATCH1 = 1; LATCH1 = 0;
}

void main(void)
{
    unsigned char i;

    while (1)
    {
        for (i = 0; i < 6; i++)
        {
            Display(letter[i]);
            Delay(50000);  // 每个字母显示约0.5秒
        }
    }
}
