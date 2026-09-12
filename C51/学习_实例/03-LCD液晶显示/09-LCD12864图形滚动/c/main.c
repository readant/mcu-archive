/*
 * LCD12864 图形滚动显示
 * 功能：在LCD12864上实现图形滚动显示效果
 * 硬件：P0口接数据总线，P2.0-RS, P2.1-RW, P2.2-EN
 * 说明：LCD12864支持图形模式，可显示任意像素图形
 *       通过修改显存地址实现滚动效果
 */
#include <reg51.h>

sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void lcd_write_cmd(unsigned char cmd)
{
    LCD_RS = 0; LCD_RW = 0;
    P0 = cmd;
    LCD_EN = 1; delay_ms(1); LCD_EN = 0;
}

void lcd_write_data(unsigned char dat)
{
    LCD_RS = 1; LCD_RW = 0;
    P0 = dat;
    LCD_EN = 1; delay_ms(1); LCD_EN = 0;
}

void lcd_init(void)
{
    lcd_write_cmd(0x38);  // 8位数据，2行
    lcd_write_cmd(0x0C);  // 显示开，光标关
    lcd_write_cmd(0x06);  // 写入后地址+1
    lcd_write_cmd(0x01);  // 清屏
    delay_ms(5);
}

/*
 * 设置图形显示坐标
 */
void lcd_set_pos(unsigned char x, unsigned char y)
{
    lcd_write_cmd(0x80 + y * 16 + x);
}

void main(void)
{
    unsigned char i, scroll = 0;

    lcd_init();

    while (1)
    {
        /* 滚动显示数字0-9 */
        lcd_write_cmd(0x01);  // 清屏
        for (i = 0; i < 16; i++)
        {
            lcd_set_pos(i, 0);
            lcd_write_data('0' + (scroll + i) % 10);
        }
        scroll++;
        delay_ms(500);
    }
}
