/*------------------------------------------------
  功能：按键切换模式
  硬件：P2.0接按键，P1口接LED
  说明：每按一次按键切换一种LED模式
        模式1：全亮  模式2：流水灯  模式3：闪烁
------------------------------------------------*/
#include <reg51.h>

sbit KEY = P2^0;

void Delay(unsigned int t)
{
    while (--t);
}

void Mode1_Full(void)
{
    P1 = 0x00;  // 全亮
}

void Mode2_Water(void)
{
    unsigned char i;
    unsigned char temp = 0xFE;
    for (i = 0; i < 8; i++)
    {
        P1 = temp;
        Delay(50000);
        temp = (temp << 1) | 0x01;
    }
}

void Mode3_Blink(void)
{
    P1 = 0x00;
    Delay(50000);
    P1 = 0xFF;
    Delay(50000);
}

void main(void)
{
    unsigned char mode = 1;

    while (1)
    {
        // 检测按键切换模式
        if (KEY == 0)
        {
            Delay(20);
            if (KEY == 0)
            {
                mode++;
                if (mode > 3) mode = 1;
                while (!KEY);
            }
        }

        // 执行当前模式
        switch (mode)
        {
            case 1: Mode1_Full(); break;
            case 2: Mode2_Water(); break;
            case 3: Mode3_Blink(); break;
        }
    }
}
