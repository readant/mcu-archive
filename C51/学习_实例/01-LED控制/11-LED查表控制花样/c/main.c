/*
 * LED查表控制花样
 * 功能：通过code数组查表法控制LED花样变化
 * 硬件：P1口接8个LED（低电平点亮）
 * 说明：查表法是C51编程的常用技巧
 *       将LED状态预先定义在code数组中，运行时查表输出
 *       比用if/switch更简洁高效
 */
#include <reg51.h>

/* LED花样表（code存储在ROM中，节省RAM） */
unsigned char code LED_TABLE[] = {
    0xFE, 0xFD, 0xFB, 0xF7,  // 逐个点亮
    0xEF, 0xDF, 0xBF, 0x7F,  // 继续逐个
    0x00, 0xFF,               // 全亮、全灭
    0xAA, 0x55,               // 交替亮灭
    0x0F, 0xF0                // 高低4位交替
};
#define TABLE_LEN (sizeof(LED_TABLE) / sizeof(LED_TABLE[0]))

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char i = 0;

    while (1)
    {
        P1 = LED_TABLE[i];
        i = (i + 1) % TABLE_LEN;
        delay_ms(300);
    }
}
