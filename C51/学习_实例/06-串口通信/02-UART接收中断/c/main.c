/*------------------------------------------------
  功能：UART接收中断
  硬件：P3.0接MAX232 RXD，P1.0接LED
  说明：电脑发送字符，单片机接收后点亮LED
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;

void InitUART(void)
{
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = 0xFD;    // 9600bps@11.0592MHz
    TL1 = 0xFD;
    TR1 = 1;
    ES = 1;
    EA = 1;
}

// 串口中断服务函数
void UART_isr(void) interrupt 4
{
    unsigned char dat;

    if (RI)     // 接收到数据
    {
        RI = 0;          // 清除接收标志
        dat = SBUF;      // 读取数据

        if (dat == '1')
            LED = 0;     // 点亮LED
        else if (dat == '0')
            LED = 1;     // 熄灭LED
    }
}

void main(void)
{
    InitUART();
    LED = 1;  // 初始熄灭

    while (1);
}
