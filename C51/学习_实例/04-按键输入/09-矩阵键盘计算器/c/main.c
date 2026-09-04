/*
 * 矩阵键盘计算器
 * 功能：4x4矩阵键盘实现简易计算器（0-9加减乘除）
 * 硬件：P1口接4x4矩阵键盘，LCD1602显示结果
 * 说明：矩阵键盘是按键输入的进阶应用
 *       本例实现简单的四则运算计算器
 */
#include <reg51.h>

sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;

unsigned char key_value = 0;
unsigned char calc_state = 0;  // 0:输入第一个数, 1:输入运算符, 2:输入第二个数
unsigned char num1 = 0, num2 = 0;
unsigned char operator = 0;

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
    lcd_write_cmd(0x38);
    lcd_write_cmd(0x0C);
    lcd_write_cmd(0x06);
    lcd_write_cmd(0x01);
    delay_ms(5);
}

/*
 * 扫描4x4矩阵键盘
 * 返回按键值（0-15），无按键返回16
 */
unsigned char key_scan(void)
{
    unsigned char row, col;
    unsigned char key = 16;

    P1 = 0xF0;  // 行输出低，列输入高
    if (P1 != 0xF0)
    {
        delay_ms(10);  // 消抖
        if (P1 != 0xF0)
        {
            for (row = 0; row < 4; row++)
            {
                P1 = ~(0x01 << row);  // 逐行扫描
                for (col = 0; col < 4; col++)
                {
                    if (!(P1 & (0x10 << col)))
                    {
                        key = row * 4 + col;
                        while (!(P1 & (0x10 << col)));  // 等待释放
                    }
                }
            }
        }
    }
    return key;
}

void main(void)
{
    unsigned char key;
    unsigned char result;

    lcd_init();

    while (1)
    {
        key = key_scan();
        if (key < 16)
        {
            if (key < 10)  // 数字键
            {
                lcd_write_data('0' + key);
                if (calc_state == 0) num1 = key;
                else if (calc_state == 2) num2 = key;
            }
            else if (key >= 10 && key < 14)  // 运算符
            {
                calc_state = 1;
                operator = key - 10;  // 0:+, 1:-, 2:*, 3:/
            }
            else if (key == 14)  // 等号
            {
                switch (operator)
                {
                    case 0: result = num1 + num2; break;
                    case 1: result = num1 - num2; break;
                    case 2: result = num1 * num2; break;
                    case 3: result = num1 / num2; break;
                }
                lcd_write_data('=');
                lcd_write_data('0' + result);
                calc_state = 0;
            }
            else if (key == 15)  // 清除
            {
                lcd_write_cmd(0x01);
                num1 = num2 = 0;
                calc_state = 0;
            }
        }
        delay_ms(100);
    }
}
