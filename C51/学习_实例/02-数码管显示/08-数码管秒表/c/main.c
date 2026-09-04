/*------------------------------------------------
  功能：数码管秒表（00~59循环）
  硬件：2位数码管，P0口接段选
  说明：模拟秒表，每秒加1，到59后归零
------------------------------------------------*/
#include <reg51.h>

sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD};

void Delay(unsigned int t)
{
    while (--t);
}

void Display(unsigned char num)
{
    unsigned char ge = num % 10;
    unsigned char shi = num / 10;

    P0 = 0x00;
    LATCH1 = 1; LATCH1 = 0;
    P0 = weima[0];
    LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[shi];
    LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    P0 = 0x00;
    LATCH1 = 1; LATCH1 = 0;
    P0 = weima[1];
    LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[ge];
    LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    unsigned char sec = 0;
    unsigned int timer = 0;

    while (1)
    {
        Display(sec);
        timer++;

        if (timer >= 5000)
        {
            timer = 0;
            sec++;
            if (sec >= 60)
            {
                sec = 0;
            }
        }
    }
}
