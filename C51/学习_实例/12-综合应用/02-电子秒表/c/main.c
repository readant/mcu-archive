/*------------------------------------------------
  功能：电子秒表（00:00~59:59）
  硬件：4位数码管，P0口接段选
  说明：精确到0.01秒的秒表
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;
sbit KEY_START = P2^2;
sbit KEY_STOP = P2^3;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7};

bit running = 0;
unsigned char min = 0, sec = 0, ms10 = 0;

void Delay(unsigned int t)
{
    while (--t);
}

void Init_Timer0(void)
{
    TMOD |= 0x01;
    TH0 = (65536 - 50000) / 256;  // 50ms
    TL0 = (65536 - 50000) % 256;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void Timer0_isr(void) interrupt 1
{
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    if (running)
    {
        ms10++;
        if (ms10 >= 20)     // 20 x 50ms = 1秒
        {
            ms10 = 0;
            sec++;
            if (sec >= 60)
            {
                sec = 0;
                min++;
                if (min >= 60) min = 0;
            }
        }
    }
}

void Display(void)
{
    unsigned char buf[4];
    unsigned char i;

    buf[0] = min / 10;
    buf[1] = min % 10;
    buf[2] = sec / 10;
    buf[3] = sec % 10;

    for (i = 0; i < 4; i++)
    {
        P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
        P0 = weima[i]; LATCH2 = 1; LATCH2 = 0;
        P0 = duanma[buf[i]];
        if (i == 1) P0 |= 0x80;  // 加冒号
        LATCH1 = 1; LATCH1 = 0;
        Delay(5);
    }
}

void main(void)
{
    Init_Timer0();

    while (1)
    {
        Display();

        if (KEY_START == 0)
        {
            Delay(20);
            if (KEY_START == 0) { running = 1; while (!KEY_START); }
        }

        if (KEY_STOP == 0)
        {
            Delay(20);
            if (KEY_STOP == 0) { running = 0; while (!KEY_STOP); }
        }
    }
}
