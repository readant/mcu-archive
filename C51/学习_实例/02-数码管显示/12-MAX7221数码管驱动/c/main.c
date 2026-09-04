/*
 * MAX7221 数码管驱动
 * 功能：通过MAX7221/MAX7219专用芯片驱动8位数码管
 * 硬件：P1.0-DIN, P1.1-CS, P1.2-CLK (SPI三线)
 * 说明：MAX7221是串行输入/输出的LED驱动芯片
 *       内置BCD译码器、扫描电路、段驱动和位驱动
 *       只需3个IO即可控制8位数码管，大大简化电路
 */
#include <reg51.h>

sbit DIN  = P1^0;
sbit CS   = P1^1;
sbit CLK  = P1^2;

/* MAX7221寄存器地址 */
#define REG_NOOP       0x00
#define REG_DIGIT0     0x01
#define REG_DIGIT1     0x02
#define REG_DIGIT2     0x03
#define REG_DIGIT3     0x04
#define REG_DIGIT4     0x05
#define REG_DIGIT5     0x06
#define REG_DIGIT6     0x07
#define REG_DIGIT7     0x08
#define REG_DECODE     0x09  // BCD译码模式
#define REG_INTENSITY  0x0A  // 亮度
#define REG_SCANLIMIT  0x0B  // 扫描位数
#define REG_SHUTDOWN   0x0C  // 关断模式
#define REG_DISPLAYTEST 0x0F

unsigned char code SEG_TABLE[] = {
    0x7E, 0x30, 0x6D, 0x79, 0x33,  // 0-4 (带小数点的段码)
    0x5B, 0x5F, 0x70, 0x7F, 0x7B   // 5-9
};

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 向MAX7221写入一个16位数据
 */
void max7221_write(unsigned char addr, unsigned char dat)
{
    unsigned char i;
    CS = 0;  // 片选使能
    for (i = 0; i < 8; i++)
    {
        CLK = 0;
        DIN = (addr >> (7 - i)) & 0x01;
        CLK = 1;
    }
    for (i = 0; i < 8; i++)
    {
        CLK = 0;
        DIN = (dat >> (7 - i)) & 0x01;
        CLK = 1;
    }
    CS = 1;  // 锁存数据
}

void max7221_init(void)
{
    max7221_write(REG_DISPLAYTEST, 0x00);  // 正常模式
    max7221_write(REG_SCANLIMIT, 0x07);    // 扫描8位
    max7221_write(REG_DECODE, 0xFF);       // BCD译码模式
    max7221_write(REG_INTENSITY, 0x08);    // 中等亮度
    max7221_write(REG_SHUTDOWN, 0x01);     // 正常工作
}

void main(void)
{
    unsigned char i;

    max7221_init();

    while (1)
    {
        /* 显示00000001-00000008 */
        for (i = 0; i < 8; i++)
        {
            max7221_write(REG_DIGIT0 + i, i + 1);
        }
        delay_ms(1000);

        /* 显示88888888 */
        for (i = 0; i < 8; i++)
        {
            max7221_write(REG_DIGIT0 + i, 8);
        }
        delay_ms(1000);
    }
}
