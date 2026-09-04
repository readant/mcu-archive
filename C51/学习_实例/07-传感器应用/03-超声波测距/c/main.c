/*------------------------------------------------
  功能：超声波测距（HC-SR04）
  硬件：Trig接P1.0，Echo接P1.1，P0口接数码管
  说明：发送触发信号，测量回波时间，计算距离
------------------------------------------------*/
#include <reg51.h>

sbit Trig = P1^0;
sbit Echo = P1^1;
sbit LATCH1 = P2^0;
sbit LATCH2 = P2^1;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void Delay(unsigned int t)
{
    while (--t);
}

// 微秒级延时（12MHz晶振）
void DelayUs(unsigned char us)
{
    while (us--);
}

// 发送触发信号
void SendTrig(void)
{
    Trig = 1;
    DelayUs(15);
    Trig = 0;
}

// 测量回波高电平时间（us）
unsigned int MeasureEcho(void)
{
    unsigned int timeout = 0;

    while (!Echo && timeout < 1000) timeout++;  // 等待高电平
    timeout = 0;

    while (Echo && timeout < 30000) timeout++;  // 计时

    return timeout;
}

void Display(unsigned int distance)
{
    unsigned char bai = distance / 100;
    unsigned char shi = distance % 100 / 10;
    unsigned char ge = distance % 10;

    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xFB; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[bai]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xF7; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[shi]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);

    P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
    P0 = 0xEF; LATCH2 = 1; LATCH2 = 0;
    P0 = duanma[ge]; LATCH1 = 1; LATCH1 = 0;
    Delay(5);
}

void main(void)
{
    unsigned int time, distance;

    while (1)
    {
        SendTrig();
        time = MeasureEcho();

        // 距离 = 时间 * 声速 / 2
        // 声速约340m/s = 0.034cm/us
        distance = (unsigned int)(time * 0.017);  // 单位：cm

        if (distance > 999) distance = 999;

        Display(distance);
        Delay(50000);
    }
}
