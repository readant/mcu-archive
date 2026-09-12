/*
 * ADC0809 模数转换
 * 功能：通过ADC0809采集8路模拟信号
 * 硬件：P0口接ADC0809数据总线，P2.0-ALE, P2.1-START, P2.2-EOC, P2.3-OE
 * 说明：ADC0809是8位8通道逐次逼近型ADC
 *       与ADC0832（SPI接口）不同，ADC0809是并行接口
 *       转换时间约100us，适合多通道采集
 */
#include <reg51.h>

sbit ADC_ALE  = P2^0;  // 地址锁存使能
sbit ADC_START = P2^1; // 启动转换
sbit ADC_EOC  = P2^2;  // 转换结束
sbit ADC_OE   = P2^3;  // 输出使能

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 读取ADC0809指定通道
 */
unsigned char adc0809_read(unsigned char channel)
{
    unsigned char dat;

    P0 = channel;       // 选择通道0-7
    ADC_ALE = 1;        // 锁存地址
    ADC_START = 1;      // 启动转换
    ADC_ALE = 0;
    ADC_START = 0;

    while (ADC_EOC == 0);  // 等待转换完成

    ADC_OE = 1;            // 使能输出
    dat = P0;              // 读取数据
    ADC_OE = 0;

    return dat;
}

void main(void)
{
    unsigned char adc_val;
    unsigned char channel = 0;

    while (1)
    {
        adc_val = adc0809_read(channel);
        P0 = adc_val;

        channel++;
        if (channel >= 8) channel = 0;

        delay_ms(500);
    }
}
