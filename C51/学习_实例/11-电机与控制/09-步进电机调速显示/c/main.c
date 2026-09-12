/*
 * 步进电机调速+LCD显示
 * 功能：通过PWM调节步进电机转速，LCD显示当前转速
 * 硬件：P1.0-P1.3接步进电机驱动，LCD1602显示
 * 说明：步进电机转速由脉冲频率决定
 *       通过调节脉冲间隔实现调速
 *       LCD实时显示当前转速（RPM）
 */
#include <reg51.h>

sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;

/* 步进电机四相八拍驱动表 */
unsigned char code STEPS[] = {
    0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09
};

volatile unsigned int speed = 1000;  // 脉冲间隔（us）
volatile unsigned char step_index = 0;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void delay_us(unsigned int us)
{
    while (us--);
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

void main(void)
{
    unsigned char key;
    unsigned int rpm;

    lcd_init();

    while (1)
    {
        /* 读取按键调速 */
        key = P3 & 0x03;
        if (key == 0x01 && speed > 200) speed -= 100;
        if (key == 0x02 && speed < 3000) speed += 100;

        /* 步进一步 */
        P1 = STEPS[step_index];
        step_index = (step_index + 1) % 8;
        delay_us(speed);

        /* 计算并显示转速 */
        rpm = 60000000UL / (2048UL * speed);  // 简化计算
        lcd_write_cmd(0x80);
        lcd_write_data('R');
        lcd_write_data('P');
        lcd_write_data('M');
        lcd_write_data(':');
        lcd_write_data('0' + rpm / 1000);
        lcd_write_data('0' + rpm / 100 % 10);
        lcd_write_data('0' + rpm / 10 % 10);
        lcd_write_data('0' + rpm % 10);
    }
}
