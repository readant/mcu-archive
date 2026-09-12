/*
 * 51单片机多机通信（三机通讯）
 * 功能：3个51单片机通过串口多机通信模式互联
 * 硬件：主机TXD→所有从机RXD，每个从机TXD→主机RXD
 *       P2.0-P2.2分别接3个从机的使能端
 * 说明：51单片机串口方式2/3支持多机通信
 *       TB8=1表示地址帧，TB8=0表示数据帧
 *       从机SM2=1时只接收地址帧，匹配后清SM2接收数据
 */
#include <reg51.h>

#define SLAVE1  0x01  // 从机1地址
#define SLAVE2  0x02  // 从机2地址
#define SLAVE3  0x03  // 从机3地址

sbit SLAVE1_EN = P2^0;
sbit SLAVE2_EN = P2^1;
sbit SLAVE3_EN = P2^2;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void uart_init(void)
{
    SCON = 0xD0;  // 方式3, 允许接收, TB8/RB8可用
    TMOD |= 0x20;
    TH1 = 0xFD;   // 9600bps
    TL1 = 0xFD;
    TR1 = 1;
}

void uart_send_addr(unsigned char addr)
{
    TB8 = 1;  // 地址帧标志
    SBUF = addr;
    while (!TI);
    TI = 0;
}

void uart_send_data(unsigned char dat)
{
    TB8 = 0;  // 数据帧标志
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

/*
 * 主机发送数据到指定从机
 */
void master_send(unsigned char slave_addr, unsigned char dat)
{
    /* 选择从机 */
    SLAVE1_EN = (slave_addr == SLAVE1) ? 0 : 1;
    SLAVE2_EN = (slave_addr == SLAVE2) ? 0 : 1;
    SLAVE3_EN = (slave_addr == SLAVE3) ? 0 : 1;

    /* 发送地址帧 */
    uart_send_addr(slave_addr);
    delay_ms(10);

    /* 发送数据帧 */
    uart_send_data(dat);

    /* 关闭所有从机 */
    SLAVE1_EN = 1;
    SLAVE2_EN = 1;
    SLAVE3_EN = 1;
}

void main(void)
{
    unsigned char recv;

    uart_init();

    while (1)
    {
        /* 主机轮流向3个从机发送数据 */
        master_send(SLAVE1, 0x11);
        delay_ms(100);
        master_send(SLAVE2, 0x22);
        delay_ms(100);
        master_send(SLAVE3, 0x33);
        delay_ms(500);

        /* 接收从机应答 */
        recv = uart_recv();
        P0 = recv;
    }
}
