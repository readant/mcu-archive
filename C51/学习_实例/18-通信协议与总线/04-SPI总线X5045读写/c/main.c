/*
 * SPI总线 X5045 读写
 * 功能：通过SPI总线读写X5045芯片（看门狗+EEPROM+电压监控）
 * 硬件：P1.0-CS, P1.1-SCK, P1.2-SI, P1.3-SO
 * 说明：X5045集成了4Kbit EEPROM、看门狗定时器、电源监控
 *       SPI是全双工同步串行总线，速率比I2C快
 *       指令集：WREN(06H), WRDI(04H), RDSR(05H), WRSR(01H), READ(03H), WRITE(02H)
 */
#include <reg51.h>

sbit X5045_CS = P1^0;
sbit X5045_SCK = P1^1;
sbit X5045_SI = P1^2;
sbit X5045_SO = P1^3;

#define WREN   0x06  // 写使能
#define WRDI   0x04  // 写禁止
#define RDSR   0x05  // 读状态寄存器
#define WRSR   0x01  // 写状态寄存器
#define READ   0x03  // 读数据
#define WRITE  0x02  // 写数据

void delay_us(unsigned char us)
{
    while (us--);
}

unsigned char spi_transfer(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        X5045_SI = (dat >> 7) & 0x01;
        dat <<= 1;
        X5045_SCK = 1; delay_us(2);
        X5045_SCK = 0; delay_us(2);
        dat |= X5045_SO;
    }
    return dat;
}

void x5045_write_enable(void)
{
    X5045_CS = 0;
    spi_transfer(WREN);
    X5045_CS = 1;
}

unsigned char x5045_read_status(void)
{
    unsigned char status;
    X5045_CS = 0;
    spi_transfer(RDSR);
    status = spi_transfer(0xFF);
    X5045_CS = 1;
    return status;
}

void x5045_write_byte(unsigned int addr, unsigned char dat)
{
    x5045_write_enable();
    X5045_CS = 0;
    spi_transfer(WRITE);
    spi_transfer((unsigned char)(addr >> 8));
    spi_transfer((unsigned char)(addr & 0xFF));
    spi_transfer(dat);
    X5045_CS = 1;
    while (x5045_read_status() & 0x01);  // 等待写完
}

unsigned char x5045_read_byte(unsigned int addr)
{
    unsigned char dat;
    X5045_CS = 0;
    spi_transfer(READ);
    spi_transfer((unsigned char)(addr >> 8));
    spi_transfer((unsigned char)(addr & 0xFF));
    dat = spi_transfer(0xFF);
    X5045_CS = 1;
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

    x5045_write_byte(0x0000, 0xAA);
    delay_ms(10);

    read_data = x5045_read_byte(0x0000);
    P0 = read_data;  // 0xAA显示在LED上

    while (1)
    {
        delay_ms(1000);
    }
}
