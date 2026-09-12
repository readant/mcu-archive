/*
 * 74LS164 移位寄存器驱动数码管
 * 功能：通过74LS164串入并出方式驱动数码管显示数字
 * 硬件：P1.0-DSA/DSB(数据输入), P1.1-CP(时钟)
 * 说明：74LS164无锁存，数据直接输出，适合连续刷新场景
 */
#include <reg51.h>

sbit DSA = P1^0;  // 数据输入A
sbit CP  = P1^1;  // 时钟脉冲

unsigned char code SEG_TABLE[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99,  // 0-4
    0x92, 0x82, 0xF8, 0x80, 0x90   // 5-9
};

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 向74LS164写入一个字节（共阴数码管段码）
 */
void ls164_write(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DSA = (dat >> i) & 0x01;
        CP = 0;
        CP = 1;  // 上升沿移入
    }
}

void main(void)
{
    unsigned char num = 0;

    while (1)
    {
        ls164_write(SEG_TABLE[num]);
        delay_ms(1000);
        num++;
        if (num >= 10)
            num = 0;
    }
}
