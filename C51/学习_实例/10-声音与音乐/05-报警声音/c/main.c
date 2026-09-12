/*------------------------------------------------
  功能：报警声音（多种模式）
  硬件：无源蜂鸣器接P1.5，按键P2.0切换模式
  说明：火灾报警、倒车雷达、防盗报警等模式
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

// 火灾报警（急促滴滴声）
void FireAlarm(void)
{
    unsigned char i;
    for (i = 0; i < 50; i++)
    {
        BEEP = 0; DelayMs(50);
        BEEP = 1; DelayMs(50);
    }
}

// 倒车雷达（间隔渐短）
void ParkingRadar(void)
{
    unsigned int d;
    for (d = 200; d > 20; d -= 20)
    {
        BEEP = 0; DelayMs(d);
        BEEP = 1; DelayMs(d);
    }
    BEEP = 0; DelayMs(500);
    BEEP = 1;
}

// 防盗报警（长鸣）
void AntiTheft(void)
{
    BEEP = 0; DelayMs(1000);
    BEEP = 1; DelayMs(500);
}

// 电话铃声
void PhoneRing(void)
{
    unsigned char i;
    for (i = 0; i < 10; i++)
    {
        BEEP = 0; DelayMs(200);
        BEEP = 1; DelayMs(100);
        BEEP = 0; DelayMs(200);
        BEEP = 1; DelayMs(400);
    }
}

void main(void)
{
    unsigned char mode = 0;

    while (1)
    {
        if (KEY == 0)
        {
            DelayMs(20);
            if (KEY == 0)
            {
                mode++;
                if (mode > 3) mode = 0;
                while (!KEY);
            }
        }

        switch (mode)
        {
            case 0: FireAlarm(); break;
            case 1: ParkingRadar(); break;
            case 2: AntiTheft(); break;
            case 3: PhoneRing(); break;
        }
    }
}
