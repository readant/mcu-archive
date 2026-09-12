/*------------------------------------------------
  功能：LED花样灯（ROM查表法）
  硬件：P1口接8个LED，低电平点亮
  说明：预定义16种花样，循环显示
------------------------------------------------*/
#include <reg51.h>

// 花样数据表（ROM存储）
unsigned char code pattern[] = {
    0xFE, 0xFD, 0xFB, 0xF7,  // 逐个点亮
    0xEF, 0xDF, 0xBF, 0x7F,  // 继续逐个
    0xFF, 0x00,               // 全灭、全亮
    0x55, 0xAA,               // 交替闪烁
    0x0F, 0xF0,               // 高低4位交替
    0xCC, 0x33                // 奇偶位交替
};

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    unsigned char i;

    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            P1 = pattern[i];
            Delay(30000);
        }
    }
}
