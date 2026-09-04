/*------------------------------------------------
  功能：红外接收基础
  硬件：红外接收头接P3.2，P1口接LED
  说明：接收红外信号，LED显示低8位数据
------------------------------------------------*/
#include <reg51.h>

sbit IR = P3^2;

void Delay(unsigned int t) { while (--t); }
void DelayUs(unsigned char us) { while (us--); }

unsigned int MeasurePulse(void)
{
    unsigned int count = 0;
    while (IR == 0 && count < 1000) count++;  // 等待高电平
    count = 0;
    while (IR == 1 && count < 1000) count++;  // 测量高电平时间
    return count;
}

unsigned long ReceiveIR(void)
{
    unsigned char i;
    unsigned long dat = 0;

    while (IR == 0);     // 等待引导码低电平
    MeasurePulse();       // 跳过引导码高电平（4.5ms）

    for (i = 0; i < 32; i++)
    {
        dat <<= 1;
        if (MeasurePulse() > 100)
            dat |= 1;
    }

    return dat;
}

void main(void)
{
    unsigned long ir_data;

    while (1)
    {
        if (IR == 0)
        {
            ir_data = ReceiveIR();
            P1 = ir_data & 0xFF;  // 低8位显示在LED上
        }
    }
}
