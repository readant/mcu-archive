/*
 * RS485 半双工通信
 * 功能：通过MAX485芯片实现RS485半双工通信，发送和接收数据
 * 硬件：P3.0-RXD, P3.1-TXD, P3.2-DE(发送使能), P3.3-RE(接收使能)
 * 说明：RS485是工业现场总线标准，差分信号传输抗干扰强
 *       半双工模式下同一时刻只能发送或接收，需要控制方向
 */
#include <reg51.h>

sbit DE = P3^2;  // MAX485 发送使能（高有效）
sbit RE = P3^3;  // MAX485 接收使能（低有效）

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void uart_init(void)
{
    SCON = 0x50;  // 模式1, 允许接收
    TMOD |= 0x20; // T1方式2自动重装
    TH1 = 0xFD;   // 9600bps @11.0592MHz
    TL1 = 0xFD;
    TR1 = 1;
}

void uart_send(unsigned char dat)
{
    DE = 1; RE = 1;  // 切换到发送模式
    SBUF = dat;
    while (!TI);
    TI = 0;
    DE = 0; RE = 0;  // 切换回接收模式
}

unsigned char uart_recv(void)
{
    unsigned char dat;
    while (!RI);
    RI = 0;
    dat = SBUF;
    return dat;
}

void main(void)
{
    unsigned char recv_data;

    uart_init();
    DE = 0; RE = 0;  // 初始接收模式

    while (1)
    {
        /* 接收数据 */
        recv_data = uart_recv();

        /* 回显并加1返回 */
        uart_send(recv_data + 1);

        delay_ms(100);
    }
}
