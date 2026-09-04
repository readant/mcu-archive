/*------------------------------------------------
  功能：继电器控制
  硬件：继电器控制端接P1.0，按键P2.0
  说明：按下按键切换继电器开关状态
        可控制220V家用电器（注意安全！）
------------------------------------------------*/
#include <reg51.h>

sbit RELAY = P1^0;
sbit KEY = P2^0;

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    RELAY = 0;  // 初始断开

    while (1)
    {
        if (KEY == 0)
        {
            Delay(20);
            if (KEY == 0)
            {
                RELAY = !RELAY;  // 切换状态
                while (!KEY);
            }
        }
    }
}
