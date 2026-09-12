/*
 * 74HC595 串入并出控制8位LED
 * 功能：通过74HC595串行输入并行输出，控制8个LED流水灯效果
 * 硬件：P1.0-SER(数据), P1.1-SRCLK(移位时钟), P1.2-RCLK(锁存时钟)
 * 说明：74HC595只需3个IO即可控制8个输出，是经典的IO扩展方案
 */
#include <reg51.h>

sbit SER  = P1^0;  // 串行数据输入
sbit SRCLK = P1^1; // 移位寄存器时钟
sbit RCLK = P1^2;  // 输出锁存时钟

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 向74HC595写入一个字节
 * 先发送高位(MSB)，595是上升沿移位
 */
void hc595_write(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        SER = (dat >> i) & 0x01;  // 先发低位也行，看接线
        SRCLK = 0;
        SRCLK = 1;  // 上升沿移入一位
    }
    RCLK = 0;
    RCLK = 1;  // 上升沿锁存输出
}

void main(void)
{
    unsigned char i;
    unsigned char led = 0x01;

    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            hc595_write(led);
            led = (led << 1);  // 左移
            delay_ms(200);
        }
        led = 0x01;  // 重新开始
    }
}
