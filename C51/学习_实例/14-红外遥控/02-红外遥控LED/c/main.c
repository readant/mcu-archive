/*------------------------------------------------
  功能：红外遥控LED
  硬件：红外接收头接P3.2，P0口接LED
  说明：用遥控器数字键控制LED
------------------------------------------------*/
#include <reg51.h>

sbit IR = P3^2;

void Delay(unsigned int t) { while (--t); }
void DelayUs(unsigned char us) { while (us--); }

unsigned int MeasurePulse(void)
{
    unsigned int count = 0;
    while (IR == 0 && count < 1000) count++;
    count = 0;
    while (IR == 1 && count < 1000) count++;
    return count;
}

unsigned long ReceiveIR(void)
{
    unsigned char i;
    unsigned long dat = 0;
    while (IR == 0);
    MeasurePulse();
    for (i = 0; i < 32; i++)
    {
        dat <<= 1;
        if (MeasurePulse() > 100) dat |= 1;
    }
    return dat;
}

void main(void)
{
    unsigned long ir_data;
    unsigned char cmd;

    P0 = 0xFF;

    while (1)
    {
        if (IR == 0)
        {
            ir_data = ReceiveIR();
            cmd = ir_data & 0xFF;

            switch (cmd)
            {
                case 0x0C: P0 = 0xFE; break;  // 1
                case 0x18: P0 = 0xFD; break;  // 2
                case 0x5E: P0 = 0xFB; break;  // 3
                case 0x08: P0 = 0xF7; break;  // 4
                case 0x1C: P0 = 0xFF; break;  // 关
            }
        }
    }
}
