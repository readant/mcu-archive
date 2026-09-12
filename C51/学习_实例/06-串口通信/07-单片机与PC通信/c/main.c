/*
 * 单片机与PC通信（含上位机）
 * 功能：通过串口与PC通信，接收命令控制LED
 * 硬件：P3.0-RXD, P3.1-TXD, MAX232电平转换
 * 说明：单片机与PC通信需要电平转换（TTL→RS232）
 *       MAX232是经典的电平转换芯片
 *       配合串口调试助手可实现完整通信
 */
#include <reg51.h>

sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit LED3 = P2^2;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void uart_init(void)
{
    SCON = 0x50;  // 模式1, 允许接收
    TMOD |= 0x20; // T1方式2
    TH1 = 0xFD;   // 9600bps @11.0592MHz
    TL1 = 0xFD;
    TR1 = 1;
    ES = 1;       // 使能串口中断
    EA = 1;
}

void uart_send(unsigned char dat)
{
    SBUF = dat;
    while (!TI);
    TI = 0;
}

void uart_send_string(unsigned char *str)
{
    while (*str)
    {
        uart_send(*str++);
    }
}

/*
 * 串口中断服务函数
 */
void uart_isr(void) interrupt 4
{
    unsigned char recv_data;

    if (RI)
    {
        RI = 0;
        recv_data = SBUF;

        switch (recv_data)
        {
            case '1': LED1 = ~LED1; uart_send_string("LED1 Toggle\r\n"); break;
            case '2': LED2 = ~LED2; uart_send_string("LED2 Toggle\r\n"); break;
            case '3': LED3 = ~LED3; uart_send_string("LED3 Toggle\r\n"); break;
            case 'a': LED1 = 0; LED2 = 0; LED3 = 0; uart_send_string("All ON\r\n"); break;
            case 'b': LED1 = 1; LED2 = 1; LED3 = 1; uart_send_string("All OFF\r\n"); break;
            default: uart_send_string("Unknown cmd\r\n"); break;
        }
    }
}

void main(void)
{
    uart_init();
    uart_send_string("MCU Ready!\r\n");
    uart_send_string("Cmd: 1/2/3=toggle LED, a=all on, b=all off\r\n");

    while (1)
    {
        delay_ms(100);
    }
}
