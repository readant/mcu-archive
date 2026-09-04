/*------------------------------------------------
  功能：LED双向流水灯
  硬件：P1口接8个LED，低电平点亮
  说明：左移一遍 → 右移一遍 → 全闪三遍，循环
------------------------------------------------*/
#include <reg51.h>

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    unsigned char i;
    unsigned char temp;

    while (1)
    {
        /*--- 左移流水灯 ---*/
        temp = 0xFE;
        for (i = 0; i < 7; i++)
        {
            P1 = temp;
            Delay(50000);
            temp = (temp << 1) | 0x01;
        }

        /*--- 右移流水灯 ---*/
        temp = 0x7F;
        for (i = 0; i < 7; i++)
        {
            P1 = temp;
            Delay(50000);
            temp = (temp >> 1) | 0x80;
        }

        /*--- 全闪3次 ---*/
        for (i = 0; i < 3; i++)
        {
            P1 = 0x00;    // 全亮
            Delay(50000);
            P1 = 0xFF;    // 全灭
            Delay(50000);
        }
    }
}
