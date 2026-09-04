/*
 * P3口流水灯
 * 功能：通过P3口实现流水灯效果，演示不同端口操作
 * 硬件：P3口接8个LED（低电平点亮）
 * 说明：P3口有特殊功能（RXD/TXD/INT0/INT1等）
 *       作为普通IO使用时与P1/P2没有区别
 *       但需要注意P3口的第二功能冲突问题
 */
#include <reg51.h>

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char i;
    unsigned char led = 0xFE;  // 初始：P3.0亮

    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            P3 = led;
            delay_ms(200);
            led = (led << 1) | 0x01;  // 左移，最低位补1
            if (led == 0xFF) led = 0xFE;  // 循环
        }

        /* 反向流水 */
        led = 0x7F;
        for (i = 0; i < 8; i++)
        {
            P3 = led;
            delay_ms(200);
            led = (led >> 1) | 0x80;  // 右移，最高位补1
            if (led == 0xFF) led = 0x7F;
        }
    }
}
