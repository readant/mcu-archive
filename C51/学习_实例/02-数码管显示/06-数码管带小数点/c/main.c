/*------------------------------------------------
  功能：数码管带小数点显示
  硬件：4位数码管，P0口接段选
  说明：在第二位显示小数点，模拟温度显示 2 5.6
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

// 段码表（带小数点版本）
unsigned char code duanma[] = {
    0xBF, 0x86, 0xDB, 0xCF, 0xE6,  // 0.~4.
    0xED, 0xFD, 0x87, 0xFF, 0xEF   // 5.~9.
};

// 普通段码表（无小数点）
unsigned char code duanma_nodp[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7};

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    // 显示 2 5.6（25.6度）
    unsigned char disp[] = {2, 5, 6, 0};  // 第二位带小数点
    unsigned char i;

    while (1)
    {
        for (i = 0; i < 4; i++)
        {
            P0 = 0x00;
            LATCH1 = 1; LATCH1 = 0;
            P0 = weima[i];
            LATCH2 = 1; LATCH2 = 0;

            if (i == 1)  // 第二位显示小数点
                P0 = duanma[disp[i]];
            else
                P0 = duanma_nodp[disp[i]];

            LATCH1 = 1; LATCH1 = 0;
            Delay(5);
        }
    }
}
