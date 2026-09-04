/*
 * 数码管滚动显示
 * 功能：8位数码管实现数字滚动显示效果
 * 硬件：P0口接段选, P2口接位选, 8位共阴数码管
 * 说明：通过逐位移动显示内容，实现类似LED屏的滚动效果
 *       常用于显示时间、温度等需要滚动展示的场合
 */
#include <reg51.h>

unsigned char code SEG_TABLE[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,  // 0-4
    0x6D, 0x7D, 0x07, 0x7F, 0x6F   // 5-9
};

/* 滚动数据：要显示的数字序列 */
unsigned char scroll_data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
unsigned char scroll_len = 10;  // 数据长度
unsigned char scroll_pos = 0;   // 当前滚动位置

unsigned char disp_buf[8];  // 显示缓冲区

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 更新显示缓冲区（将滚动数据填入缓冲区）
 */
void update_disp_buf(void)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        unsigned char idx = (scroll_pos + i) % scroll_len;
        disp_buf[i] = scroll_data[idx];
    }
}

/*
 * 扫描显示8位数码管
 */
void display_scan(void)
{
    unsigned char i;
    unsigned char位选[] = {0xFE, 0xFD, 0xFB, 0xF7,
                           0xEF, 0xDF, 0xBF, 0x7F};

    for (i = 0; i < 8; i++)
    {
        P0 = 0x00;  // 消隐
        P2 = 0xFF;
        P0 = SEG_TABLE[disp_buf[i]];
        P2 = 位选[i];
        delay_ms(2);
    }
}

void main(void)
{
    unsigned int cnt = 0;

    update_disp_buf();

    while (1)
    {
        display_scan();
        cnt++;
        if (cnt >= 3000)  // 控制滚动速度
        {
            cnt = 0;
            scroll_pos++;
            if (scroll_pos >= scroll_len)
                scroll_pos = 0;
            update_disp_buf();
        }
    }
}
