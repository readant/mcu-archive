/*
 * DAC0832 锯齿波输出
 * 功能：通过DAC0832输出锯齿波电压信号
 * 硬件：P0口接DAC0832数据总线，P2.0-CS, P2.1-WR
 * 说明：DAC0832是8位D/A转换芯片
 *       可输出0-VREF的模拟电压
 *       通过软件生成各种波形：锯齿波、三角波、方波
 */
#include <reg51.h>

sbit DAC_CS  = P2^0;
sbit DAC_WR  = P2^1;

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

/*
 * 输出一个DAC值
 */
void dac_output(unsigned char dat)
{
    DAC_CS = 0;
    DAC_WR = 0;
    P0 = dat;
    DAC_WR = 1;
    DAC_CS = 1;
}

void main(void)
{
    unsigned char i = 0;

    while (1)
    {
        /* 锯齿波 */
        for (i = 0; i < 255; i++)
        {
            dac_output(i);
            delay_us(10);
        }

        /* 三角波（先升后降） */
        for (i = 0; i < 255; i++)
        {
            dac_output(i);
            delay_us(10);
        }
        for (i = 255; i > 0; i--)
        {
            dac_output(i);
            delay_us(10);
        }
    }
}
