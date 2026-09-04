/*
 * 74LS138 3-8线译码器
 * 功能：通过74LS138将3位地址译码为8路输出，控制8个LED
 * 硬件：P1.0-P1.2接A0-A2(地址), P1.3-P1.5接G1/G2A/G2B(使能)
 * 说明：74LS138低电平有效输出，常用于存储器地址译码和IO扩展
 *       3个IO控制8路输出
 */
#include <reg51.h>

sbit G1  = P1^3;  // 使能端1（高有效）
sbit G2A = P1^4;  // 使能端2A（低有效）
sbit G2B = P1^5;  // 使能端2B（低有效）

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 选择8路输出中的某一路
 * 74LS138输出低电平有效，Y0-Y7对应地址0-7
 */
void ls138_select(unsigned char ch)
{
    P1 &= 0xF8;         // 清除低3位
    P1 |= (ch & 0x07);  // 写入3位地址
    G1 = 1; G2A = 0; G2B = 0;  // 使能
    G1 = 1; G2A = 1; G2B = 1;  // 关闭
}

void main(void)
{
    unsigned char i = 0;

    G1 = 1; G2A = 1; G2B = 1;  // 初始关闭

    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            ls138_select(i);
            delay_ms(300);
        }
    }
}
