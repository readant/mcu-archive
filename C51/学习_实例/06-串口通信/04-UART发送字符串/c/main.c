/*------------------------------------------------
  功能：UART发送字符串
  硬件：P3.1=TXD
  说明：封装字符串发送函数，方便调用
------------------------------------------------*/
#include <reg51.h>

void InitUART(void)
{
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1;
    ES = 1;
    EA = 1;
}

void SendByte(unsigned char dat)
{
    SBUF = dat;
    while (!TI);
    TI = 0;
}

void SendString(char *str)
{
    while (*str)
    {
        SendByte(*str++);
    }
}

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    InitUART();

    while (1)
    {
        SendString("Hello, this is 51 MCU!\r\n");
        Delay(50000);
    }
}
