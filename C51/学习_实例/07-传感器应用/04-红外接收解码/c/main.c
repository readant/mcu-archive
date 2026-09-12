/*------------------------------------------------
  功能：红外接收解码（NEC协议）
  硬件：红外接收头接P3.2，P1口接LED
  说明：接收遥控器信号，解码后控制LED
------------------------------------------------*/
#include <reg51.h>

sbit IR = P3^2;

void Delay(unsigned int t)
{
    while (--t);
}

// 微秒级延时
void DelayUs(unsigned char us)
{
    while (us--);
}

// 测量高电平持续时间
unsigned int MeasureHigh(void)
{
    unsigned int count = 0;
    while (IR && count < 10000) count++;
    return count;
}

// 测量低电平持续时间
unsigned int MeasureLow(void)
{
    unsigned int count = 0;
    while (!IR && count < 10000) count++;
    return count;
}

// 接收32位数据
unsigned long ReceiveIR(void)
{
    unsigned char i;
    unsigned long dat = 0;
    unsigned int high_time, low_time;

    // 等待引导码（9ms低 + 4.5ms高）
    while (!IR);            // 等待低电平
    MeasureHigh();          // 测量9ms低电平
    MeasureLow();           // 测量4.5ms高电平

    // 接收32位数据
    for (i = 0; i < 32; i++)
    {
        high_time = MeasureHigh();
        low_time = MeasureLow();

        dat <<= 1;
        if (high_time > low_time)
            dat |= 1;      // 高电平时间长表示1
    }

    return dat;
}

void main(void)
{
    unsigned long ir_data;
    unsigned char addr, cmd;

    while (1)
    {
        if (!IR)  // 检测到信号
        {
            ir_data = ReceiveIR();

            addr = (ir_data >> 24) & 0xFF;  // 地址码
            cmd = ir_data & 0xFF;           // 命令码

            switch (cmd)
            {
                case 0x0C: P1 = 0xFE; break;  // 按键1
                case 0x18: P1 = 0xFD; break;  // 按键2
                case 0x5E: P1 = 0xFB; break;  // 按键3
                case 0x08: P1 = 0xF7; break;  // 按键4
                case 0x1C: P1 = 0xFF; break;  // 关闭
            }
        }
    }
}
