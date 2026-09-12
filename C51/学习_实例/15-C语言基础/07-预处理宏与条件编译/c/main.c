/*
 * 预处理宏与条件编译
 * 功能：演示C51预处理指令的实际应用
 * 硬件：无（纯软件示例）
 * 说明：宏定义可以简化代码、提高可移植性
 *       条件编译可以实现代码裁剪和调试
 *       这是工程化编程的基础技能
 */
#include <reg51.h>

/* 基础宏定义 */
#define LED_PORT P1
#define LED_ALL_ON  0x00
#define LED_ALL_OFF 0xFF

/* 带参数的宏 */
#define LED_ON(n)    (LED_PORT &= ~(0x01 << (n)))
#define LED_OFF(n)   (LED_PORT |= (0x01 << (n)))
#define LED_TOGGLE(n) (LED_PORT ^= (0x01 << (n)))

/* 条件编译：调试模式 */
#define DEBUG_MODE 1

#if DEBUG_MODE
    #define DBG_PRINT(x) /* 串口打印调试信息 */
#else
    #define DBG_PRINT(x)
#endif

/* 条件编译：功能裁剪 */
#define FEATURE_TIMER  1
#define FEATURE_UART   1
#define FEATURE_ADC    0  // 关闭ADC功能

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/* 根据条件编译不同的函数 */
#if FEATURE_TIMER
void timer_init(void)
{
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x18;
    TR0 = 1;
}
#endif

#if FEATURE_UART
void uart_init(void)
{
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1;
}
#endif

void main(void)
{
    unsigned char i;

    LED_PORT = LED_ALL_OFF;

    /* 使用宏操作LED */
    for (i = 0; i < 8; i++)
    {
        LED_ON(i);
        delay_ms(200);
        LED_OFF(i);
    }

    /* 根据条件编译调用不同的初始化函数 */
    #if FEATURE_TIMER
    timer_init();
    #endif

    #if FEATURE_UART
    uart_init();
    #endif

    while (1)
    {
        LED_TOGGLE(0);
        delay_ms(500);
    }
}
