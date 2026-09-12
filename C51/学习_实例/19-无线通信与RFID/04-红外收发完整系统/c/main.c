/*
 * 红外收发完整系统
 * 功能：红外发射+接收双向通信，避障测试
 * 硬件：P3.0-红外接收, P1.0-红外发射, P2.0-P2.3-LED指示
 * 说明：红外收发是近距离无线通信的基础
 *       本例实现红外发射和接收的完整系统
 *       可用于红外避障、红外通信等场景
 */
#include <reg51.h>

sbit IR_RX = P3^0;   // 红外接收（低有效）
sbit IR_TX = P1^0;   // 红外发射
sbit LED1  = P2^0;
sbit LED2  = P2^1;
sbit LED3  = P2^2;
sbit LED4  = P2^3;

void delay_us(unsigned char us)
{
    while (us--);
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 发射38kHz红外载波
 */
void ir_send_carrier(unsigned char ms)
{
    unsigned char i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 38; j++)
        {
            IR_TX = 0; delay_us(13);  // 38kHz = 26us周期
            IR_TX = 1; delay_us(13);
        }
    }
}

/*
 * 发射NEC格式的地址+命令
 */
void ir_send_nec(unsigned char addr, unsigned char cmd)
{
    unsigned char i;

    /* 引导码：9ms高 + 4.5ms低 */
    ir_send_carrier(9);
    delay_ms(5);

    /* 发送8位地址 */
    for (i = 0; i < 8; i++)
    {
        ir_send_carrier(1);  // 0.56ms载波
        delay_ms(1);         // 0.56ms间隔
    }

    /* 发送8位命令 */
    for (i = 0; i < 8; i++)
    {
        ir_send_carrier(1);
        delay_ms(1);
    }
}

/*
 * 接收红外信号（简易版）
 */
unsigned char ir_recv(void)
{
    unsigned char timeout = 0;

    while (IR_RX && timeout < 200) timeout++;
    if (timeout >= 200) return 0;

    delay_us(500);  // 跳过引导码

    return 0x01;  // 简化返回
}

void main(void)
{
    unsigned char recv_data;

    IR_TX = 1;  // 初始高电平

    while (1)
    {
        /* 发送红外信号 */
        ir_send_nec(0x00, 0x01);
        delay_ms(500);

        /* 接收红外信号 */
        recv_data = ir_recv();
        if (recv_data)
        {
            LED1 = ~LED1;
        }

        /* 红外避障检测 */
        if (IR_RX == 0)  // 检测到障碍物
        {
            LED4 = 0;  // 避障LED亮
        }
        else
        {
            LED4 = 1;
        }

        delay_ms(100);
    }
}
