/*------------------------------------------------
  功能：蜂鸣器报警
  硬件：蜂鸣器接P1.5，P2.0接按键
  说明：按下按键蜂鸣器响，松开停止
        可用于温度超限报警等场景
------------------------------------------------*/
#include <reg51.h>

sbit BEEP = P1^5;
sbit KEY = P2^0;

void Delay(unsigned int t)
{
    while (--t);
}

// 蜂鸣器响一声
void BeepOnce(void)
{
    unsigned char i;
    for (i = 0; i < 100; i++)
    {
        BEEP = !BEEP;
        Delay(100);
    }
    BEEP = 1;
}

// 蜂鸣器连续响
void BeepOn(void)
{
    BEEP = 0;  // 低电平驱动蜂鸣器
}

void BeepOff(void)
{
    BEEP = 1;
}

void main(void)
{
    BEEP = 1;  // 初始静音

    while (1)
    {
        if (KEY == 0)
        {
            Delay(20);
            if (KEY == 0)
            {
                BeepOnce();  // 响一声
                while (!KEY);
            }
        }
    }
}
