/*
 * FM1702 RFID 读写
 * 功能：通过FM1702模块读写Mifare IC卡
 * 硬件：SPI接口连接FM1702，天线线圈
 * 说明：FM1702是复旦微电子的13.56MHz RFID读写芯片
 *       支持Mifare Classic卡的读写操作
 *       ISO14443A协议，广泛用于门禁、公交卡等
 */
#include <reg51.h>

sbit FM1702_CS   = P1^0;
sbit FM1702_SCK  = P1^1;
sbit FM1702_MOSI = P1^2;
sbit FM1702_MISO = P1^3;
sbit FM1702_RST  = P1^4;

/* FM1702命令集 */
#define CMD_IDLE       0x00
#define CMD_READ_EOF   0x01
#define CMD_CALC_CRC   0x03
#define CMD_TRANSMIT   0x04
#define CMD_NOCMD      0x07
#define CMD_AUTH       0x0C
#define CMD_READ       0x0C
#define CMD_WRITE      0x0C

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

void fm1702_write_reg(unsigned char addr, unsigned char dat)
{
    unsigned char i;
    FM1702_CS = 0;
    for (i = 0; i < 8; i++)
    {
        FM1702_SCK = 0;
        FM1702_MOSI = (addr >> (7 - i)) & 0x01;
        FM1702_SCK = 1;
    }
    for (i = 0; i < 8; i++)
    {
        FM1702_SCK = 0;
        FM1702_MOSI = (dat >> (7 - i)) & 0x01;
        FM1702_SCK = 1;
    }
    FM1702_CS = 1;
}

unsigned char fm1702_read_reg(unsigned char addr)
{
    unsigned char i, dat = 0;
    FM1702_CS = 0;
    for (i = 0; i < 8; i++)
    {
        FM1702_SCK = 0;
        FM1702_MOSI = (addr >> (7 - i)) | 0x80;
        FM1702_SCK = 1;
    }
    for (i = 0; i < 8; i++)
    {
        FM1702_SCK = 0;
        FM1702_SCK = 1;
        dat = (dat << 1) | FM1702_MISO;
    }
    FM1702_CS = 1;
    return dat;
}

void fm1702_init(void)
{
    FM1702_RST = 0;
    delay_ms(10);
    FM1702_RST = 1;
    delay_ms(10);

    fm1702_write_reg(0x01, 0x0F);  // CommandReg
    fm1702_write_reg(0x02, 0x00);  // ComIEnReg
    fm1702_write_reg(0x03, 0x00);  // DivIEnReg
    fm1702_write_reg(0x04, 0x00);  // ComIrqReg
    fm1702_write_reg(0x05, 0x00);  // DivIrqReg
    fm1702_write_reg(0x06, 0x00);  // ErrorReg
    fm1702_write_reg(0x07, 0x08);  // Status1Reg
    fm1702_write_reg(0x08, 0x00);  // Status2Reg
    fm1702_write_reg(0x09, 0x00);  // FIFODataReg
    fm1702_write_reg(0x0A, 0x00);  // FIFOLevelReg
    fm1702_write_reg(0x0B, 0x00);  // WaterLevelReg
    fm1702_write_reg(0x0C, 0x03);  // ControlReg
    fm1702_write_reg(0x0D, 0xFF);  // BitFramingReg
    fm1702_write_reg(0x0E, 0x00);  // CollReg
    fm1702_write_reg(0x0F, 0x00);  // ModeReg
}

void main(void)
{
    fm1702_init();

    while (1)
    {
        /* 寻卡、防冲突、选卡、认证、读卡流程 */
        P0 = fm1702_read_reg(0x07);  // 读取状态
        delay_ms(500);
    }
}
