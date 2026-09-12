/*
 * ST7920 字库液晶显示
 * 功能：通过ST7920字库芯片驱动LCD12864显示中文
 * 硬件：SPI接口连接ST7920
 * 说明：ST7920内置中文字库，可直接显示汉字
 *       不需要取模，简化中文显示方案
 *       与普通12864（无字库）形成对比
 */
#include <reg51.h>

sbit ST7920_CS  = P1^0;
sbit ST7920_SID = P1^1;
sbuf ST7920_CLK = P1^2;

void delay_us(unsigned char us)
{
    while (us--);
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void st7920_write_byte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        ST7920_CLK = 0;
        ST7920_SID = (dat >> (7 - i)) & 0x01;
        ST7920_CLK = 1;
    }
}

void st7920_write_cmd(unsigned char cmd)
{
    ST7920_CS = 1;
    st7920_write_byte(0xF8);  // 控制字：命令
    st7920_write_byte(cmd & 0xF0);
    st7920_write_byte((cmd << 4) & 0xF0);
    ST7920_CS = 0;
}

void st7920_write_data(unsigned char dat)
{
    ST7920_CS = 1;
    st7920_write_byte(0xFA);  // 控制字：数据
    st7920_write_byte(dat & 0xF0);
    st7920_write_byte((dat << 4) & 0xF0);
    ST7920_CS = 0;
}

void st7920_init(void)
{
    st7920_write_cmd(0x30);  // 基本指令集
    st7920_write_cmd(0x0C);  // 显示开
    st7920_write_cmd(0x01);  // 清屏
    delay_ms(5);
    st7920_write_cmd(0x36);  // 扩展指令集，开图形
}

/*
 * 显示中文字符串（GB2312编码）
 */
void st7920_display_string(unsigned char x, unsigned char y, unsigned char *str)
{
    unsigned char i;
    st7920_write_cmd(0x80 + y * 0x20 + x);
    for (i = 0; str[i] != '\0'; i++)
    {
        st7920_write_data(str[i]);
    }
}

void main(void)
{
    st7920_init();

    while (1)
    {
        st7920_display_string(0, 0, "单片机学习");
        st7920_display_string(0, 1, "ST7920字库");
        st7920_display_string(0, 2, "中文显示测试");
        delay_ms(1000);
    }
}
