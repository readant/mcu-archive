/*------------------------------------------------
  功能：独立按键检测
  硬件：P2.0~P2.3接4个按键，P1口接LED
  说明：按下不同按键，点亮对应LED
------------------------------------------------*/
#include <reg51.h>

sbit KEY1 = P2^0;
sbit KEY2 = P2^1;
sbit KEY3 = P2^2;
sbit KEY4 = P2^3;

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    P1 = 0xFF;  // 初始：LED全灭

    while (1)
    {
        if (KEY1 == 0) { Delay(20); if (KEY1 == 0) P1_0 = 0; }
        if (KEY2 == 0) { Delay(20); if (KEY2 == 0) P1_1 = 0; }
        if (KEY3 == 0) { Delay(20); if (KEY3 == 0) P1_2 = 0; }
        if (KEY4 == 0) { Delay(20); if (KEY4 == 0) P1_3 = 0; }

        // 松手恢复
        if (KEY1 == 1) P1_0 = 1;
        if (KEY2 == 1) P1_1 = 1;
        if (KEY3 == 1) P1_2 = 1;
        if (KEY4 == 1) P1_3 = 1;
    }
}
