/*------------------------------------------------
  功能：LED流水灯（右移）
  硬件：P1口接8个LED，低电平点亮
  说明：LED从P1.7向P1.0依次点亮
------------------------------------------------*/
#include <reg51.h>

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    unsigned char i;
    unsigned char temp = 0x7F;  // 初始值：0111 1111，P1.7亮

    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            P1 = temp;
            Delay(50000);
            temp = (temp >> 1) | 0x80;  // 右移一位，高位补1
        }
        temp = 0x7F;  // 重新从P1.7开始
    }
}
