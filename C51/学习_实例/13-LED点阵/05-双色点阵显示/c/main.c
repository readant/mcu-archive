/*
 * 双色点阵显示
 * 功能：8x8双色LED点阵显示不同颜色的图形
 * 硬件：P0口接行线，P2口接列线，额外引脚控制颜色
 * 说明：双色点阵有红绿两种颜色
 *       通过控制颜色引脚可以显示红、绿、橙（红+绿）三种颜色
 *       与单色点阵相比增加了色彩表现力
 */
#include <reg51.h>

sbit COLOR_R = P3^0;  // 红色使能
sbit COLOR_G = P3^1;  // 绿色使能

unsigned char code RED_HEART[] = {
    0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18
};

unsigned char code GREEN_STAR[] = {
    0x00, 0x08, 0x1C, 0x7F, 0x1C, 0x08, 0x00, 0x00
};

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void display_symbol(unsigned char *pattern, unsigned char color, unsigned int duration)
{
    unsigned char i, j;
    unsigned int cnt;

    for (cnt = 0; cnt < duration; cnt++)
    {
        for (i = 0; i < 8; i++)
        {
            P0 = 0xFF;  // 先消隐
            P2 = ~(0x01 << i);  // 扫描行

            if (color == 0)  // 红色
            {
                COLOR_R = 0; COLOR_G = 1;
                P0 = pattern[i];
            }
            else if (color == 1)  // 绿色
            {
                COLOR_R = 1; COLOR_G = 0;
                P0 = pattern[i];
            }
            else  // 橙色（红+绿）
            {
                COLOR_R = 0; COLOR_G = 0;
                P0 = pattern[i];
            }

            delay_ms(2);
        }
    }

    COLOR_R = 1; COLOR_G = 1;  // 关闭
}

void main(void)
{
    COLOR_R = 1; COLOR_G = 1;

    while (1)
    {
        display_symbol(RED_HEART, 0, 500);   // 红色爱心
        display_symbol(GREEN_STAR, 1, 500);  // 绿色星星
        display_symbol(RED_HEART, 2, 500);   // 橙色爱心
    }
}
