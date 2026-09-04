/*
 * RAM扩展读写
 * 功能：通过外部总线扩展6264 SRAM
 * 硬件：P0口接数据总线（需锁存器），P2口高8位地址，ALE锁存
 * 说明：6264是8KB的静态RAM芯片
 *       51单片机只有128/256字节内部RAM
 *       外部扩展RAM可大大增加数据存储空间
 *       使用MOVX指令访问外部RAM
 */
#include <reg51.h>
#include <absacc.h>

#define RAM_BASE  0x0000  // 6264基地址
#define RAM_SIZE  0x2000  // 8KB

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned int addr;
    unsigned char write_data, read_data;
    unsigned char error = 0;

    /* 写入测试数据 */
    for (addr = 0; addr < 100; addr++)
    {
        write_data = addr & 0xFF;
        XBYTE[RAM_BASE + addr] = write_data;
    }

    /* 读取验证 */
    for (addr = 0; addr < 100; addr++)
    {
        read_data = XBYTE[RAM_BASE + addr];
        if (read_data != (addr & 0xFF))
        {
            error = 1;
            break;
        }
    }

    P0 = error ? 0xFF : 0x00;  // 0x00=成功, 0xFF=失败

    while (1)
    {
        delay_ms(1000);
    }
}
