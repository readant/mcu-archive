/*------------------------------------------------
  功能：倒计时器（99→00）
  硬件：2位数码管，按键设置时间
  说明：可设置倒计时时间，到0报警
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;
sbit BEEP = P1^5;
sbit KEY_SET = P2^2;
sbit KEY_START = P2^3;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD};

unsigned char count = 30;  // 倒计时值
bit running = 0;

void Delay(unsigned int t) { while (--t); }

void Init_Timer0(void)
{
    TMOD |= 0x01;
    TH0 = (65536-50000)/256;
    TL0 = (65536-50000)%256;
    EA = 1; ET0 = 1; TR0 = 1;
}

void Timer0_isr(void) interrupt 1
{
    static unsigned char timer = 0;
    TH0 = (65536-50000)/256;
    TL0 = (65536-50000)%256;

    if (running)
    {
        timer++;
        if (timer >= 20)
        {
            timer = 0;
            if (count > 0) count--;
            else { running = 0; BEEP = 0; }
        }
    }
}

void Display(unsigned char num)
{
    P0=0x00; LATCH1=1; LATCH1=0;
    P0=weima[0]; LATCH2=1; LATCH2=0;
    P0=duanma[num/10]; LATCH1=1; LATCH1=0;
    Delay(5);

    P0=0x00; LATCH1=1; LATCH1=0;
    P0=weima[1]; LATCH2=1; LATCH2=0;
    P0=duanma[num%10]; LATCH1=1; LATCH1=0;
    Delay(5);
}

void main(void)
{
    Init_Timer0();

    while (1)
    {
        Display(count);

        if (KEY_SET == 0)
        {
            Delay(20);
            if (KEY_SET == 0)
            {
                running = 0;
                count += 10;
                if (count > 99) count = 0;
                while (!KEY_SET);
            }
        }

        if (KEY_START == 0)
        {
            Delay(20);
            if (KEY_START == 0)
            {
                running = !running;
                BEEP = 1;
                while (!KEY_START);
            }
        }
    }
}
