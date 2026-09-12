/*
 * DS1621 温度传感器
 * 功能：通过I2C总线读取DS1621温度传感器数据
 * 硬件：P2.0-SCL, P2.1-SDA, DS1621地址0x90(写)/0x91(读)
 * 说明：DS1621是Dallas公司的I2C温度传感器
 *       精度0.5℃，测量范围-55~125℃
 *       与DS18B20（单总线）形成对比，适合理解不同总线协议
 */
#include <reg51.h>

sbit SCL = P2^0;
sbit SDA = P2^1;

#define DS1621_ADDR  0x90  // DS1621 I2C地址

void delay_us(unsigned char us)
{
    while (us--);
}

void i2c_start(void)
{
    SDA = 1; SCL = 1; delay_us(5);
    SDA = 0; delay_us(5); SCL = 0;
}

void i2c_stop(void)
{
    SDA = 0; SCL = 1; delay_us(5);
    SDA = 1; delay_us(5);
}

void i2c_ack(void)
{
    unsigned char timeout = 0;
    SDA = 1; SCL = 1;
    while (SDA && timeout < 250) timeout++;
    SCL = 0;
}

void i2c_write_byte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        SDA = (dat >> (7 - i)) & 0x01;
        SCL = 1; delay_us(5); SCL = 0; delay_us(5);
    }
    i2c_ack();
}

unsigned char i2c_read_byte(void)
{
    unsigned char i, dat = 0;
    SDA = 1;
    for (i = 0; i < 8; i++)
    {
        SCL = 1; delay_us(5);
        dat = (dat << 1) | SDA;
        SCL = 0; delay_us(5);
    }
    return dat;
}

/*
 * 启动DS1621温度转换
 */
void ds1621_start_convert(void)
{
    i2c_start();
    i2c_write_byte(DS1621_ADDR);     // 设备地址+写
    i2c_write_byte(0xEE);            // 启动转换命令
    i2c_stop();
}

/*
 * 读取DS1621温度值
 * 返回：温度值（有符号，单位0.5℃）
 */
signed char ds1621_read_temp(void)
{
    signed char temp;
    i2c_start();
    i2c_write_byte(DS1621_ADDR);     // 设备地址+写
    i2c_write_byte(0xAA);            // 读温度命令
    i2c_start();                     // 重复起始
    i2c_write_byte(DS1621_ADDR | 0x01); // 设备地址+读
    temp = i2c_read_byte();          // 读高字节
    i2c_stop();
    return temp;
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

unsigned char code SEG_TABLE[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void display_temp(signed char temp)
{
    unsigned char sig, ten, one;
    if (temp < 0) temp = -temp;
    one = temp % 10;
    ten = (temp / 10) % 10;
    sig = temp / 100;

    P2 = 0xFE; P0 = SEG_TABLE[sig]; delay_ms(2);
    P2 = 0xFD; P0 = SEG_TABLE[ten]; delay_ms(2);
    P2 = 0xFB; P0 = SEG_TABLE[one] | 0x80; delay_ms(2);  // 小数点
}

void main(void)
{
    signed char temperature;

    ds1621_start_convert();
    delay_ms(500);  // 等待转换完成

    while (1)
    {
        temperature = ds1621_read_temp();
        display_temp(temperature);
        P0 = temperature;  // 同时输出到P0口
        delay_ms(500);
    }
}
