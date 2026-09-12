/*
 * 模拟串口（软件UART）
 * 功能：通过IO口软件模拟UART时序，实现串口通信
 * 硬件：P3.0-模拟RXD, P3.1-模拟TXD
 * 说明：当硬件串口不够用时，可用任意IO口模拟串口
 *       软件串口优点：可扩展多个串口
 *       软件串口缺点：占用CPU时间，波特率受限
 */
#include <reg51.h>

sbit SOFT_RXD = P3^0;  // 模拟接收脚
sbit SOFT_TXD = P3^1;  // 模拟发送脚

#define BAUD_9600  104   // 9600bps @11.0592MHz (1/9600 ≈ 104us)
#define BAUD_4800  208   // 4800bps

void delay_us(unsigned char us)
{
    while (us--);
}

/*
 * 软件发送一个字节（起始位+8数据位+停止位）
 */
void soft_uart_send(unsigned char dat)
{
    unsigned char i;

    SOFT_TXD = 0;           // 起始位
    delay_us(BAUD_9600);

    for (i = 0; i < 8; i++)
    {
        SOFT_TXD = dat & 0x01;  // 先发低位
        dat >>= 1;
        delay_us(BAUD_9600);
    }

    SOFT_TXD = 1;           // 停止位
    delay_us(BAUD_9600);
}

/*
 * 软件接收一个字节
 * 返回：接收到的数据，0表示无数据
 */
unsigned char soft_uart_recv(void)
{
    unsigned char i, dat = 0;

    /* 等待起始位 */
    while (SOFT_RXD);  // 等待变为低电平
    delay_us(BAUD_9600 / 2);  // 跳过起始位，在数据位中间采样

    for (i = 0; i < 8; i++)
    {
        delay_us(BAUD_9600);
        dat >>= 1;
        if (SOFT_RXD)
            dat |= 0x80;
    }

    delay_us(BAUD_9600);  // 等待停止位

    return dat;
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char recv_data;

    SOFT_TXD = 1;  // 初始高电平

    while (1)
    {
        /* 接收数据 */
        recv_data = soft_uart_recv();

        /* 回显并+1 */
        if (recv_data != 0)
        {
            soft_uart_send(recv_data + 1);
            P0 = recv_data;
        }

        delay_ms(100);
    }
}
