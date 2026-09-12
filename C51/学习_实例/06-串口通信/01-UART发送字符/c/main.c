/*------------------------------------------------
  功能：UART发送字符
  硬件：P3.1接MAX232 TXD
  说明：通过串口发送字符，电脑端用串口助手接收
------------------------------------------------*/
#include <reg51.h>

void InitUART(void)
{
    SCON = 0x50;   // 模式1，允许接收
    TMOD |= 0x20;  // 定时器1，模式2（自动重装）
    TH1 = 0xFD;    // 9600bps@11.0592MHz
    TL1 = 0xFD;
    TR1 = 1;       // 启动定时器1
    ES = 1;        // 开串口中断
    EA = 1;        // 开总中断
}

void SendByte(unsigned char dat)
{
    SBUF = dat;
    while (!TI);   // 等待发送完成
    TI = 0;        // 清除发送标志
}

void main(void)
{
    InitUART();

    while (1)
    {
        SendByte('H');
        SendByte('e');
        SendByte('l');
        SendByte('l');
        SendByte('o');
        SendByte('\r');
        SendByte('\n');

        // 简单延时
        {
            unsigned int i;
            for (i = 0; i < 50000; i++);
        }
    }
}
