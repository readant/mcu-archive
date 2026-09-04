/*
 * 光耦隔离电机驱动
 * 功能：通过光耦隔离控制电机启停
 * 硬件：P1.0-光耦输入端，光耦输出端接继电器/电机驱动
 * 说明：光耦实现电气隔离，保护单片机
 *       强电和弱电分开，防止干扰和损坏
 *       工业控制中的标准做法
 */
#include <reg51.h>

sbit OPTO_IN = P1^0;   // 光耦控制输出
sbit MOTOR_EN = P2^0;   // 电机使能
sbit MOTOR_DIR = P2^1;  // 电机方向
sbit LED_RUN = P2^2;    // 运行指示灯

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char motor_state = 0;

    OPTO_IN = 1;  // 初始关闭光耦

    while (1)
    {
        /* 启动电机（通过光耦隔离） */
        OPTO_IN = 0;       // 光耦导通
        delay_ms(10);      // 稳定时间
        MOTOR_EN = 0;      // 使能电机
        MOTOR_DIR = 0;     // 正转
        LED_RUN = 0;       // 运行灯亮

        delay_ms(3000);    // 运行3秒

        /* 停止电机 */
        MOTOR_EN = 1;      // 禁止电机
        OPTO_IN = 1;       // 光耦截止
        LED_RUN = 1;       // 运行灯灭

        delay_ms(1000);    // 停止1秒

        /* 反转 */
        OPTO_IN = 0;
        delay_ms(10);
        MOTOR_EN = 0;
        MOTOR_DIR = 1;     // 反转
        LED_RUN = 0;

        delay_ms(3000);

        MOTOR_EN = 1;
        OPTO_IN = 1;
        LED_RUN = 1;

        delay_ms(1000);
    }
}
