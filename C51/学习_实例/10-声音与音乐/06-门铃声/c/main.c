/*------------------------------------------------
  功能：门铃声
  硬件：无源蜂鸣器接P1.5，按键P2.0
  说明：按下按键播放"叮咚"门铃声
------------------------------------------------*/
#include <reg51.h>

sbit BEEP = P1^5;
sbit KEY = P2^0;

void DelayMs(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

// 叮声（高音）
void Ding(void)
{
    unsigned int i;
    for (i = 0; i < 300; i++)
    {
        BEEP = 0; DelayMs(1);
        BEEP = 1; DelayMs(1);
    }
}

// 咚声（低音）
void Dong(void)
{
    unsigned int i;
    for (i = 0; i < 400; i++)
    {
        BEEP = 0; DelayMs(1);
        BEEP = 1; DelayMs(1);
    }
}

void DoorBell(void)
{
    Ding(); DelayMs(100); Dong(); DelayMs(300);
}

void main(void)
{
    while (1)
    {
        if (KEY == 0)
        {
            DelayMs(20);
            if (KEY == 0)
            {
                DoorBell();
                while (!KEY);
            }
        }
    }
}
