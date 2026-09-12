/*
 * 点阵滚动广告
 * 功能：8x8点阵实现文字滚动显示效果
 * 硬件：P0口接行线，P2口接列线
 * 说明：通过逐列移动数据实现滚动效果
 *       常用于广告屏、电梯显示等场景
 *       本例显示"HELLO"滚动效果
 */
#include <reg51.h>

/* "HELLO"字模（8x8，每列8位） */
unsigned char code HELLO[] = {
    /* H */ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00, 0x00,
    /* E */ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00, 0x00,
    /* L */ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00,
    /* L */ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00,
    /* O */ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00, 0x00
};

#define TEXT_LEN 40  // 5字符 x 8列

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned int scroll_pos = 0;
    unsigned char i, j;

    while (1)
    {
        /* 显示一帧（8列滚动） */
        for (i = 0; i < 8; i++)
        {
            P0 = 0x00;  // 消隐
            P2 = ~(0x01 << i);
            P0 = HELLO[(scroll_pos + i) % TEXT_LEN];
            delay_ms(2);
        }

        scroll_pos++;
        if (scroll_pos >= TEXT_LEN)
            scroll_pos = 0;

        delay_ms(50);  // 控制滚动速度
    }
}
