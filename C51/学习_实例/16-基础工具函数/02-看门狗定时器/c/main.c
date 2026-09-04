/*
 * 看门狗定时器
 * 功能：演示51单片机看门狗的使用方法
 * 硬件：P1.0接喂狗指示灯
 * 说明：看门狗（WDT）是防止程序跑飞的安全机制
 *       程序必须在规定时间内"喂狗"，否则WDT复位
 *       工业应用中必须使用看门狗
 */
#include <reg51.h>

sbit WDT_LED = P1^0;

/* 宏晶STC单片机看门狗寄存器（不同型号地址不同） */
/* #define WDT_CONTR 0x00E7 */

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 软件看门狗实现
 * 使用定时器中断定期检查程序状态
 */
volatile unsigned char wdt_counter = 0;
volatile unsigned char wdt_timeout = 0;

void timer0_init(void)
{
    TMOD |= 0x01;
    TH0 = 0x3C;   // 50ms @11.0592MHz
    TL0 = 0xB0;
    ET0 = 1;
    TR0 = 1;
    EA = 1;
}

void timer0_isr(void) interrupt 1
{
    TH0 = 0x3C;
    TL0 = 0xB0;

    wdt_counter++;
    if (wdt_counter >= 20)  // 1秒超时
    {
        wdt_counter = 0;
        wdt_timeout = 1;  // 标记超时
    }
}

/*
 * 喂狗函数
 */
void wdt_feed(void)
{
    wdt_counter = 0;
    wdt_timeout = 0;
}

void main(void)
{
    timer0_init();

    while (1)
    {
        /* 正常工作：定期喂狗 */
        wdt_feed();
        WDT_LED = ~WDT_LED;

        /* 模拟程序正常运行 */
        delay_ms(500);

        /* 检查是否超时 */
        if (wdt_timeout)
        {
            /* 超时处理：可以复位或报警 */
            WDT_LED = 0;  // 指示异常
            /* 实际应用中这里应该复位系统 */
        }
    }
}
