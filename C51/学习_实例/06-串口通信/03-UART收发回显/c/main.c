/*------------------------------------------------
  功能：UART收发回显（Echo）
  硬件：P3.0=RXD，P3.1=TXD
  说明：电脑发送什么，单片机就回传什么
        串口调试的基本测试程序
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

void UART_isr(void) interrupt 4
{
    if (RI)
    {
        RI = 0;
        SendByte(SBUF);  // 收到什么就回传什么
    }
}

void main(void)
{
    InitUART();
    while (1);
}
