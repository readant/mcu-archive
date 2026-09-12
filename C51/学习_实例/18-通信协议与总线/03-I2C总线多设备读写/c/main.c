/*
 * I2C总线多设备读写
 * 功能：通过软件模拟I2C时序，读写多个AT24C02 EEPROM
 * 硬件：P2.0-SCL, P2.1-SDA, 多个AT24C02挂载同一总线
 * 说明：I2C是飞利浦公司开发的两线式串行总线
 *       每个设备有7位地址，主机通过地址选择通信对象
 *       AT24C02地址：0xA0(写)/0xA1(读)
 */
#include <reg51.h>

sbit SCL = P2^0;
sbit SDA = P2^1;

void delay_us(unsigned char us)
{
    while (us--);
}

void i2c_start(void)
{
    SDA = 1; SCL = 1;
    delay_us(5);
    SDA = 0;  // SCL高时SDA下降沿=起始
    delay_us(5);
    SCL = 0;
}

void i2c_stop(void)
{
    SDA = 0; SCL = 1;
    delay_us(5);
    SDA = 1;  // SCL高时SDA上升沿=停止
    delay_us(5);
}

void i2c_ack(void)
{
    unsigned char timeout = 0;
    SDA = 1;  // 释放总线
    SCL = 1;
    while (SDA && timeout < 250) timeout++;  // 等待应答
    SCL = 0;
}

void i2c_write_byte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        SDA = (dat >> (7 - i)) & 0x01;
        SCL = 1; delay_us(5);
        SCL = 0; delay_us(5);
    }
    i2c_ack();
}

unsigned char i2c_read_byte(void)
{
    unsigned char i, dat = 0;
    SDA = 1;  // 释放总线
    for (i = 0; i < 8; i++)
    {
        SCL = 1; delay_us(5);
        dat = (dat << 1) | SDA;
        SCL = 0; delay_us(5);
    }
    return dat;
}

/*
 * 向AT24C02指定地址写入一个字节
 * addr: 设备地址(0xA0/0xA2/0xA4/0xA6)
 * reg:  内存地址(0x00-0xFF)
 * dat:  数据
 */
void at24c02_write(unsigned char addr, unsigned char reg, unsigned char dat)
{
    i2c_start();
    i2c_write_byte(addr);      // 设备地址+写
    i2c_write_byte(reg);       // 内存地址
    i2c_write_byte(dat);       // 数据
    i2c_stop();
}

/*
 * 从AT24C02指定地址读取一个字节
 */
unsigned char at24c02_read(unsigned char addr, unsigned char reg)
{
    unsigned char dat;
    i2c_start();
    i2c_write_byte(addr);      // 设备地址+写
    i2c_write_byte(reg);       // 内存地址
    i2c_start();               // 重复起始
    i2c_write_byte(addr | 0x01); // 设备地址+读
    dat = i2c_read_byte();
    i2c_stop();
    return dat;
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char read_data;

    /* 写入测试数据到地址0x00 */
    at24c02_write(0xA0, 0x00, 0x55);
    delay_ms(10);  // 等待写入完成(约5ms)

    /* 读取验证 */
    read_data = at24c02_read(0xA0, 0x00);
    P0 = read_data;  // 0x55显示在LED上

    while (1)
    {
        delay_ms(1000);
    }
}
