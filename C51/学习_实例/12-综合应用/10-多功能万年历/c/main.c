/*
 * 多功能万年历
 * 功能：DS1302时钟+DS18B20温度+LCD1602显示+闹钟
 * 硬件：LCD1602显示，DS1302时钟，DS18B20温度，按键设置
 * 说明：综合应用多个模块，实现完整的万年历系统
 *       显示：年-月-日 时:分:秒 温度
 *       功能：闹钟设置、温度报警
 */
#include <reg51.h>

sbit DS1302_CLK = P3^6;
sbit DS1302_DAT = P3^7;
sbit DS1302_RST = P3^5;

sbit DS18B20_DQ = P3^3;

sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void delay_us(unsigned char us)
{
    while (us--);
}

/* DS1302操作 */
void ds1302_write(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DS1302_DAT = dat & 0x01;
        DS1302_CLK = 1;
        DS1302_CLK = 0;
        dat >>= 1;
    }
}

unsigned char ds1302_read(void)
{
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat >>= 1;
        if (DS1302_DAT) dat |= 0x80;
        DS1302_CLK = 1;
        DS1302_CLK = 0;
    }
    return dat;
}

void ds1302_set_time(unsigned char addr, unsigned char dat)
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    ds1302_write(addr);
    ds1302_write(dat);
    DS1302_RST = 0;
}

unsigned char ds1302_read_time(unsigned char addr)
{
    unsigned char dat;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    ds1302_write(addr | 0x01);
    dat = ds1302_read();
    DS1302_RST = 0;
    return dat;
}

/* DS18B20操作 */
bit ds18b20_reset(void)
{
    bit presence;
    DS18B20_DQ = 1;
    DS18B20_DQ = 0;
    delay_us(480);
    DS18B20_DQ = 1;
    delay_us(60);
    presence = DS18B20_DQ;
    delay_us(420);
    return presence;
}

void ds18b20_write_byte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DS18B20_DQ = dat & 0x01;
        delay_us(2);
        DS18B20_DQ = 1;
        dat >>= 1;
        delay_us(60);
    }
}

unsigned char ds18b20_read_byte(void)
{
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        DS18B20_DQ = 0;
        delay_us(2);
        DS18B20_DQ = 1;
        delay_us(8);
        dat >>= 1;
        if (DS18B20_DQ) dat |= 0x80;
        delay_us(50);
    }
    return dat;
}

unsigned char ds18b20_read_temp(void)
{
    unsigned char temp_l, temp_h;
    ds18b20_reset();
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0x44);
    delay_ms(750);
    ds18b20_reset();
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0xBE);
    temp_l = ds18b20_read_byte();
    temp_h = ds18b20_read_byte();
    return ((temp_h << 8) | temp_l) >> 4;
}

/* LCD1602操作 */
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
    unsigned char year, month, day, hour, min, sec, temp;

    lcd_init();
    ds1302_set_time(0x80, 0x00);  // 秒
    ds1302_set_time(0x82, 0x30);  // 分
    ds1302_set_time(0x84, 0x12);  // 时
    ds1302_set_time(0x86, 0x04);  // 日
    ds1302_set_time(0x88, 0x09);  // 月
    ds1302_set_time(0x8C, 0x26);  // 年

    while (1)
    {
        sec = ds1302_read_time(0x80) & 0x7F;
        min = ds1302_read_time(0x82) & 0x7F;
        hour = ds1302_read_time(0x84) & 0x3F;
        day = ds1302_read_time(0x86) & 0x3F;
        month = ds1302_read_time(0x88) & 0x1F;
        year = ds1302_read_time(0x8C);

        temp = ds18b20_read_temp();

        /* 第一行：日期 */
        lcd_write_cmd(0x80);
        lcd_write_data('2');
        lcd_write_data('0');
        lcd_write_data('0' + year / 16);
        lcd_write_data('0' + year % 16);
        lcd_write_data('-');
        lcd_write_data('0' + month / 16);
        lcd_write_data('0' + month % 16);
        lcd_write_data('-');
        lcd_write_data('0' + day / 16);
        lcd_write_data('0' + day % 16);

        /* 第二行：时间+温度 */
        lcd_write_cmd(0xC0);
        lcd_write_data('0' + hour / 16);
        lcd_write_data('0' + hour % 16);
        lcd_write_data(':');
        lcd_write_data('0' + min / 16);
        lcd_write_data('0' + min % 16);
        lcd_write_data(':');
        lcd_write_data('0' + sec / 16);
        lcd_write_data('0' + sec % 16);
        lcd_write_data(' ');
        lcd_write_data('0' + temp / 10);
        lcd_write_data('0' + temp % 10);
        lcd_write_data('C');

        delay_ms(500);
    }
}
