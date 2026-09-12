/*
 * 定时器2模式应用
 * 功能：演示定时器T2的三种工作模式
 * 硬件：P1.0口输出方波
 * 说明：51单片机的定时器2是16位定时器
 *       比T0/T1多了自动重装和捕获功能
 *       模式：16位自动重装、捕获、波特率发生器
 */
#include <reg51.h>

sbit TEST_PIN = P1^0;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 定时器2初始化（16位自动重装模式）
 * 产生1ms中断 @11.0592MHz
 */
void timer2_init(void)
{
    T2MOD = 0x00;  // T2OE=0, DCEN=0
    T2CON = 0x00;  // 定时器模式

    RCAP2H = 0xFC;  // 重装值高字节
    RCAP2L = 0x66;  // 重装值低字节 (65536-1000*12/11.0592≈64986=0xFC46)

    TH2 = 0xFC;
    TL2 = 0x66;

    ET2 = 1;  // 使能T2中断
    TR2 = 1;  // 启动T2
    EA = 1;   // 开总中断
}

/*
 * 定时器2中断服务函数
 */
void timer2_isr(void) interrupt 5
{
    TF2 = 0;  // 必须手动清除T2溢出标志
    TEST_PIN = ~TEST_PIN;  // 翻转测试引脚
}

void main(void)
{
    timer2_init();

    while (1)
    {
        /* 主循环可以做其他事情 */
        P0++;
        delay_ms(1000);
    }
}
