/*------------------------------------------------
  功能：LED跑马灯（intrins.h库函数版）
  硬件：P0口接8个LED，低电平点亮
  说明：使用C51库自带的循环移位函数
        _crol_()：循环左移
        _cror_()：循环右移
------------------------------------------------*/
#include <reg51.h>
#include <intrins.h>

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    unsigned char i;

    while (1)
    {
        /*--- 从右向左跑 ---*/
        P0 = 0x7F;              // 初始：0111 1111
        for (i = 0; i < 7; i++)
        {
            P0 = _cror_(P0, 1); // 循环右移1位
            Delay(50000);
        }

        /*--- 从左向右跑 ---*/
        P0 = 0xFE;              // 初始：1111 1110
        for (i = 0; i < 7; i++)
        {
            P0 = _crol_(P0, 1); // 循环左移1位
            Delay(50000);
        }
    }
}
