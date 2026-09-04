/*------------------------------------------------
  功能：LED全亮全灭（不同节奏）
  硬件：P1口接8个LED，低电平点亮
  说明：用两种不同的延时控制闪烁节奏
------------------------------------------------*/
#include <reg51.h>

void DelayLong(void)
{
    unsigned int i;
    for (i = 0; i < 36000; i++);
}

void DelayShort(void)
{
    unsigned char i, j;
    for (i = 0; i < 200; i++)
        for (j = 0; j < 180; j++);
}

void main(void)
{
    unsigned char i;

    while (1)
    {
        /*--- 慢闪3次 ---*/
        for (i = 0; i < 3; i++)
        {
            P1 = 0xFE;     // 只亮P1.0
            DelayLong();
            P1 = 0xFF;     // 全灭
            DelayLong();
        }

        /*--- 快闪3次 ---*/
        for (i = 0; i < 3; i++)
        {
            P1 = 0xEF;     // 只亮P1.4
            DelayShort();
            P1 = 0xFF;     // 全灭
            DelayShort();
        }

        /*--- 全亮全灭各3次 ---*/
        for (i = 0; i < 3; i++)
        {
            P1 = 0x00;     // 全亮
            DelayLong();
            P1 = 0xFF;     // 全灭
            DelayLong();
        }
    }
}
