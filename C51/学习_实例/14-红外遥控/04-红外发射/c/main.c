/*------------------------------------------------
  功能：红外发射
  硬件：红外LED接P3.0
  说明：发射NEC格式红外信号
------------------------------------------------*/
#include <reg51.h>

sbit IR_TX = P3^0;

void DelayUs(unsigned char us)
{
    while (us--);
}

// 发射38KHz载波
void SendCarrier(unsigned int time)
{
    unsigned int i;
    for (i = 0; i < time; i++)
    {
        IR_TX = 0;
        DelayUs(13);    // 约38KHz
        IR_TX = 1;
        DelayUs(13);
    }
}

// 发射空闲（无载波）
void SendIdle(unsigned int time)
{
    unsigned int i;
    for (i = 0; i < time; i++)
    {
        DelayUs(26);
    }
}

// 发射一位数据
void SendBit(bit dat)
{
    SendCarrier(21);   // 9ms引导码的载波部分
    SendIdle(21);      // 4.5ms空闲

    if (dat)
        SendIdle(56);  // 1：1.69ms空闲
    else
        SendIdle(17);  // 0：0.56ms空闲
    SendCarrier(21);   // 0.56ms载波
}

// 发射32位数据
void SendIR(unsigned long dat)
{
    unsigned char i;

    // 引导码
    SendCarrier(90);   // 9ms
    SendIdle(45);      // 4.5ms

    // 数据
    for (i = 0; i < 32; i++)
    {
        SendBit(dat & 0x80000000);
        dat <<= 1;
    }

    // 结束位
    SendCarrier(21);
}

void main(void)
{
    while (1)
    {
        SendIR(0x00FF00FF);  // 示例：地址00，命令FF
        {
            unsigned int i;
            for (i = 0; i < 50000; i++);
        }
    }
}
