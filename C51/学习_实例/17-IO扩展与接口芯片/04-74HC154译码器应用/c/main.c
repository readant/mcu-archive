/*
 * 74HC154 4-16线译码器
 * 功能：通过74HC154将4位地址译码为16路输出，控制16个LED
 * 硬件：P1.0-P1.3接A0-A3(地址输入), P1.4接E1(使能1), P1.5接E2(使能2)
 * 说明：74HC154低电平有效输出，使能端都低时工作
 *       4个IO控制16路输出，IO扩展效率极高
 */
#include <reg51.h>

sbit E1 = P1^4;  // 使能端1（低有效）
sbit E2 = P1^5;  // 使能端2（低有效）

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 选择16路输出中的某一路（低电平有效）
 */
void hc154_select(unsigned char ch)
{
    P1 &= 0xF0;       // 清除低4位
    P1 |= (ch & 0x0F); // 写入4位地址
    E1 = 0; E2 = 0;   // 使能
    E1 = 1; E2 = 1;   // 关闭（锁存）
}

void main(void)
{
    unsigned char i = 0;

    E1 = 1; E2 = 1;  // 初始关闭

    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            hc154_select(i);
            delay_ms(200);
        }
    }
}
