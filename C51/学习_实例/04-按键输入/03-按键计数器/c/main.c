/*------------------------------------------------
  功能：按键计数器（0~9循环）
  硬件：P2.0接按键，P0口接数码管
  说明：每按一次按键，数码管数字加1
------------------------------------------------*/
#include <reg51.h>

sbit KEY = P2^0;
sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char count = 0;

void Delay(unsigned int t)
{
    while (--t);
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
    while (1)
    {
        Display(count);

        if (KEY == 0)
        {
            Delay(20);
            if (KEY == 0)
            {
                count++;
                if (count >= 10)
                    count = 0;
                while (!KEY);  // 等待松手
            }
        }
    }
}
