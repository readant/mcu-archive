/*
 * 定时器精确波特率生成
 * 功能：使用定时器T1产生精确的串口波特率
 * 硬件：P3.0-RXD, P3.1-TXD
 * 说明：串口波特率由定时器T1的溢出率决定
 *       方式2（8位自动重装）最常用
 *       本例演示如何计算和配置精确波特率
 */
#include <reg51.h>

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 波特率计算公式：
 * 方式2: baud = (2^SMOD / 32) * (Fosc / 12) / (256 - TH1)
 * 
 * @11.0592MHz, SMOD=0:
 *   9600bps → TH1 = 256 - 11059200/(32*12*9600) = 256-3 = 253 = 0xFD
 *   4800bps → TH1 = 256 - 11059200/(32*12*4800) = 256-6 = 250 = 0xFA
 *   19200bps → TH1 = 256 - 11059200/(32*12*19200) = 256-1.5 ≈ 254 = 0xFE
 */

void uart_init(unsigned int baud)
{
    unsigned char th1_val;

    SCON = 0x50;  // 模式1

    /* 根据波特率计算重装值 */
    /* 简化计算：TH1 = 256 - (FOSC / (32 * 12 * baud)) */
    th1_val = 256 - (11059200UL / (32UL * 12UL * baud));

    TMOD &= 0x0F;
    TMOD |= 0x20;  // T1方式2

    TH1 = th1_val;
    TL1 = th1_val;

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
    unsigned char recv_data;

    uart_init(9600);  // 初始化为9600bps

    while (1)
    {
        recv_data = uart_recv();
        uart_send(recv_data + 1);  // 回显+1
        delay_ms(100);
    }
}
