/*------------------------------------------------
  功能：人体红外感应（HC-SR501）
  硬件：HC-SR501输出接P3.2，P1.0接LED
  说明：检测到人体移动LED亮，否则灭
------------------------------------------------*/
#include <reg51.h>

sbit SENSOR = P3^2;
sbit LED = P1^0;

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    LED = 1;  // 初始熄灭

    while (1)
    {
        if (SENSOR == 1)   // 检测到人体
        {
            LED = 0;       // 点亮LED
        }
        else
        {
            LED = 1;       // 熄灭LED
        }

        Delay(1000);
    }
}
