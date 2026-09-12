/*------------------------------------------------
  功能：红外遥控电机
  硬件：红外接收头接P3.2，L298N接P1口
  说明：遥控器控制电机正转/反转/停止
------------------------------------------------*/
#include <reg51.h>

sbit IR = P3^2;
sbit IN1 = P1^0;
sbit IN2 = P1^1;
sbit ENA = P1^2;

void Delay(unsigned int t) { while (--t); }

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

    while (1)
    {
        if (IR == 0)
        {
            ir_data = ReceiveIR();
            cmd = ir_data & 0xFF;

            switch (cmd)
            {
                case 0x0C: IN1=1; IN2=0; ENA=1; break;  // 正转
                case 0x18: IN1=0; IN2=1; ENA=1; break;  // 反转
                case 0x5E: IN1=0; IN2=0; ENA=0; break;  // 停止
            }
        }
    }
}
