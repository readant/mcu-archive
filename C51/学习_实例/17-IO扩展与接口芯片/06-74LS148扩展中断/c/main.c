/*
 * 74LS148 扩展外部中断
 * 功能：通过74LS148优先编码器扩展8路外部中断源
 * 硬件：8路按键接74LS148输入端，输出接单片机IO口
 * 说明：74LS148是8-3线优先编码器，可将8个中断源编码为3位二进制
 *       当多个中断同时触发时，优先级高的先输出
 *       编码输出取反后送单片机，单片机根据编码判断中断源
 */
#include <reg51.h>

sbit EI = P3^2;  // 74LS148使能输入（低有效）

unsigned char code INT_TABLE[] = {
    0xFF, 0xFF, 0xFF, 0xFF,  // 0-3: 无中断
    0xFF, 0xFF, 0xFF, 0xFF   // 4-7: 无中断
};

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char key_num;

    EI = 0;  // 使能74LS148

    while (1)
    {
        /* 读取编码输出 P2.0-P2.2 */
        key_num = P2 & 0x07;

        /* 编码值0-7对应中断源优先级 */
        switch (key_num)
        {
            case 0: P0 = 0xFE; break;  // 中断源0
            case 1: P0 = 0xFD; break;  // 中断源1
            case 2: P0 = 0xFB; break;  // 中断源2
            case 3: P0 = 0xF7; break;  // 中断源3
            case 4: P0 = 0xEF; break;  // 中断源4
            case 5: P0 = 0xDF; break;  // 中断源5
            case 6: P0 = 0xBF; break;  // 中断源6
            case 7: P0 = 0x7F; break;  // 中断源7
            default: P0 = 0xFF; break;
        }
        delay_ms(100);
    }
}
