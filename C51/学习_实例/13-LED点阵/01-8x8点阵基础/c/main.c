/*------------------------------------------------
  功能：8x8 LED点阵基础
  硬件：P0口接行线，P1口接列线
  说明：点亮8x8点阵上的单个LED
------------------------------------------------*/
#include <reg51.h>

#define ROW_PORT P0   // 行线
#define COL_PORT P1   // 列线

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    while (1)
    {
        ROW_PORT = 0x01;   // 选中第一行
        COL_PORT = 0xFE;   // 选中第一列（低电平有效）
        Delay(50000);

        ROW_PORT = 0x02;   // 第二行
        COL_PORT = 0xFD;   // 第二列
        Delay(50000);
    }
}
