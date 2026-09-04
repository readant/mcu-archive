/*
 * 热敏电阻温度检测
 * 功能：通过NTC热敏电阻+ADC采集温度
 * 硬件：热敏电阻分压电路接ADC0832通道0, 数码管显示温度
 * 说明：NTC热敏电阻阻值随温度升高而降低
 *       通过分压电路将阻值变化转换为电压变化
 *       ADC采集后查表或计算得到温度值
 *       与DS18B20数字传感器形成对比
 */
#include <reg51.h>

sbit ADC_CS  = P1^0;
sbit ADC_CLK = P1^1;
sbit ADC_DIO = P1^2;

unsigned char code SEG_TABLE[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

/* 温度-ADC值对照表（简化，实际需要根据热敏电阻参数标定） */
unsigned char code TEMP_TABLE[] = {
    250, 240, 230, 220, 210,  // 0-4℃
    200, 190, 180, 170, 160,  // 5-9℃
    150, 142, 134, 126, 118,  // 10-14℃
    110, 103, 96,  89,  82,   // 15-19℃
    76,  70,  64,  58,  53,   // 20-24℃
    48,  43,  38,  34,  30    // 25-29℃
};

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
 * ADC0832读取指定通道的ADC值
 * channel: 0或1
 */
unsigned char adc0832_read(unsigned char channel)
{
    unsigned char i, dat = 0;

    ADC_CS = 0;
    ADC_CLK = 0;
    ADC_DIO = 1; delay_us(2);

    /* 起始位 + 单端/差分 + 通道选择 */
    ADC_CLK = 1; delay_us(2); ADC_CLK = 0;  // 起始位
    ADC_DIO = 1; delay_us(2);                // SGL=1 单端
    ADC_CLK = 1; delay_us(2); ADC_CLK = 0;
    ADC_DIO = channel; delay_us(2);          // ODD/SIGN通道选择
    ADC_CLK = 1; delay_us(2); ADC_CLK = 0;
    ADC_DIO = 1; delay_us(2);

    /* 读取8位数据 */
    for (i = 0; i < 8; i++)
    {
        ADC_CLK = 1; delay_us(2);
        ADC_CLK = 0; delay_us(2);
        dat <<= 1;
        if (ADC_DIO) dat |= 0x01;
    }

    ADC_CS = 1;
    return dat;
}

/*
 * ADC值转换为温度（查表法）
 */
unsigned char adc_to_temp(unsigned char adc_val)
{
    unsigned char i;
    for (i = 0; i < 30; i++)
    {
        if (adc_val >= TEMP_TABLE[i])
            return i;
    }
    return 30;  // 超出范围
}

void display_temp(unsigned char temp)
{
    unsigned char ten, one;
    ten = temp / 10;
    one = temp % 10;

    P2 = 0xFE; P0 = SEG_TABLE[ten]; delay_ms(2);
    P2 = 0xFD; P0 = SEG_TABLE[one]; delay_ms(2);
    P2 = 0xFB; P0 = 0x39; delay_ms(2);  // 显示'C'
}

void main(void)
{
    unsigned char adc_val, temperature;

    while (1)
    {
        adc_val = adc0832_read(0);
        temperature = adc_to_temp(adc_val);

        display_temp(temperature);
        P0 = adc_val;  // ADC值输出到P0
        delay_ms(500);
    }
}
