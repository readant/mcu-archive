/*
 * nRF905 无线收发
 * 功能：通过nRF905无线模块实现数据收发
 * 硬件：P1.0-TRX_CE, P1.1-PWR_UP, P1.2-TX_EN, P1.3-CSN
 *       P1.4-SCK, P1.5-MOSI, P1.6-MISO, P1.7-DR
 * 说明：nRF905是Nordic公司的433MHz无线收发芯片
 *       支持ShockBurst模式，自动前导码和CRC
 *       有效距离100-200米，适合物联网应用
 */
#include <reg51.h>

sbit TRX_CE = P1^0;
sbit PWR_UP = P1^1;
sbit TX_EN  = P1^2;
sbit CSN    = P1^3;
sbit SCK    = P1^4;
sbit MOSI   = P1^5;
sbit MISO   = P1^6;
sbit DR     = P1^7;

/* nRF905寄存器配置 */
#define NRF_CONFIG   0x00
#define NRF_TX_ADDR  0x10
#define NRF_TX_DATA  0x30
#define NRF_RX_DATA  0x31

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

unsigned char spi_transfer(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        MOSI = (dat >> 7) & 0x01;
        dat <<= 1;
        SCK = 1; delay_us(2);
        SCK = 0; delay_us(2);
        dat |= MISO;
    }
    return dat;
}

void nrf_write_reg(unsigned char reg, unsigned char dat)
{
    CSN = 0;
    spi_transfer(0x00 | reg);  // 写命令
    spi_transfer(dat);
    CSN = 1;
}

void nrf_write_buf(unsigned char reg, unsigned char *buf, unsigned char len)
{
    unsigned char i;
    CSN = 0;
    spi_transfer(0x00 | reg);
    for (i = 0; i < len; i++)
        spi_transfer(buf[i]);
    CSN = 1;
}

void nrf_init(void)
{
    PWR_UP = 1;
    TRX_CE = 0;
    TX_EN = 0;

    nrf_write_reg(NRF_CONFIG, 0x0C);  // 16位CRC, 无重发
    nrf_write_reg(0x01, 0x00);         // 4字节地址宽度
    nrf_write_reg(0x02, 0x00);         // 4字节地址宽度
    nrf_write_reg(0x03, 0x20);         // 4字节接收地址
    nrf_write_reg(0x04, 0x00);         // 4字节发送地址
}

void nrf_send(unsigned char *dat, unsigned char len)
{
    unsigned char status;
    nrf_write_buf(NRF_TX_DATA, dat, len);
    TX_EN = 1;
    TRX_CE = 1;
    delay_us(10);
    TRX_CE = 0;

    while (DR == 0);  // 等待发送完成
}

void main(void)
{
    unsigned char send_buf[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // "Hello"
    unsigned char recv_buf[5];

    nrf_init();

    while (1)
    {
        nrf_send(send_buf, 5);
        delay_ms(500);
    }
}
