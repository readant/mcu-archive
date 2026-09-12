/*
 * 8255 并行口扩展
 * 功能：通过Intel 8255 PPI扩展IO，控制PA/PB/PC口
 * 硬件：数据总线接P0，RD/WR/CS控制线，A0/A1地址线
 * 说明：8255提供3个8位并行IO口，可编程设置输入/输出模式
 *       PA口：方式0/1/2  PB口：方式0/1  PC口：分高低4位
 */
#include <reg51.h>
#include <absacc.h>

#define PA  XBYTE[0x7FFC]  // 8255 PA口地址 (CS=0, A1A0=00)
#define PB  XBYTE[0x7FFD]  // 8255 PB口地址 (CS=0, A1A0=01)
#define PC  XBYTE[0x7FFE]  // 8255 PC口地址 (CS=0, A1A0=10)
#define CMD XBYTE[0x7FFF]  // 8255 控制字地址 (CS=0, A1A0=11)

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char i;

    /* 控制字：90H = 10010000B
     * D7=1: 控制字标志
     * D6D5=00: PA口方式0
     * D4=1: PA口输入
     * D3=0: PC高4位输出
     * D2=0: PB口方式0
     * D1=0: PB口输出
     * D0=0: PC低4位输出
     */
    CMD = 0x90;

    while (1)
    {
        /* 读取PA口开关状态 */
        i = PA;
        /* 写入PB口控制LED */
        PB = i;
        /* PC口高4位流水灯 */
        PC = 0xF0;
        delay_ms(500);
        PC = 0x0F;
        delay_ms(500);
    }
}
