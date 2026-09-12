/*
 * RS485 全双工通信
 * 功能：RS485全双工模式，同时收发数据
 * 硬件：使用两片MAX485或全双工485芯片，TX/RX独立
 * 说明：全双工485需要4根信号线（A+/B-发送，A+/B-接收）
 *       适用于需要同时收发的场景，如实时数据采集
 */
#include <reg51.h>

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void uart_init(void)
{
    SCON = 0x50;  // 模式1, 允许接收
    TMOD |= 0x20;
    TH1 = 0xFD;   // 9600bps
    TL1 = 0xFD;
    TR1 = 1;
}

void uart_send(unsigned char dat)
{
    SBUF = dat;
    while (!TI);
    TI = 0;
}

unsigned char uart_recv(void)
{
    while (!RI);
    RI = 0;
    return SBUF;
}

void main(void)
{
    unsigned char send_cnt = 0;
    unsigned char recv_data;

    uart_init();

    while (1)
    {
        /* 发送计数 */
        uart_send(send_cnt++);
        if (send_cnt > 255) send_cnt = 0;

        /* 接收并显示在P0口 */
        recv_data = uart_recv();
        P0 = recv_data;

        delay_ms(500);
    }
}
