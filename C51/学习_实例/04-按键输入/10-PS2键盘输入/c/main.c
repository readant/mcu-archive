/*
 * PS2键盘输入
 * 功能：通过PS/2接口读取标准电脑键盘输入
 * 硬件：P3.2-CLK(外部中断0), P3.3-DATA
 * 说明：PS/2键盘使用同步串行协议
 *       11位数据帧：1起始+8数据+1校验+1停止
 *       时钟由键盘提供，单片机在时钟下降沿采样
 */
#include <reg51.h>

sbit PS2_CLK  = P3^2;  // 时钟（接外部中断0）
sbit PS2_DATA = P3^3;  // 数据

volatile unsigned char ps2_buf = 0;
volatile unsigned char ps2_ready = 0;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 外部中断0接收PS/2数据
 */
void int0_isr(void) interrupt 0
{
    static unsigned char bit_cnt = 0;
    static unsigned char data_buf = 0;

    if (bit_cnt < 8)
    {
        data_buf >>= 1;
        if (PS2_DATA)
            data_buf |= 0x80;
        bit_cnt++;
    }
    else if (bit_cnt == 8)
    {
        ps2_buf = data_buf;
        ps2_ready = 1;
        bit_cnt = 0;
    }
}

/*
 * PS/2扫描码转ASCII
 */
unsigned char ps2_to_ascii(unsigned char scancode)
{
    unsigned char code PS2_TABLE[] = {
        0, 0, '1', '2', '3', '4', '5', '6',
        '7', '8', '9', '0', '-', '=', 0, 0,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
        'o', 'p', '[', ']', 0, 0, 'a', 's'
    };

    if (scancode < 0x40)
        return PS2_TABLE[scancode];
    return 0;
}

void main(void)
{
    unsigned char ascii;
    unsigned char scan_code;

    IT0 = 1;  // 下降沿触发
    EX0 = 1;
    EA = 1;

    while (1)
    {
        if (ps2_ready)
        {
            scan_code = ps2_buf & 0x7F;  // 去掉最高位
            ascii = ps2_to_ascii(scan_code);
            if (ascii)
            {
                P0 = ascii;  // 输出到LED
            }
            ps2_ready = 0;
        }
        delay_ms(10);
    }
}
