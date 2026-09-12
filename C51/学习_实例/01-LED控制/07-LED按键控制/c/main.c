/*------------------------------------------------
  功能：按键控制LED
  硬件：P2.0接按键（低电平按下），P0.0接LED
  说明：按一下LED状态翻转一次
------------------------------------------------*/
#include <reg51.h>

sbit KEY = P2^0;
sbit LED = P0^0;

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    P0 = 0xFF;     // 初始状态：LED熄灭

    while (1)
    {
        if (KEY == 0)           // 检测按键按下
        {
            Delay(20);         // 消抖延时
            if (KEY == 0)      // 再次确认
            {
                LED = !LED;    // 翻转LED状态
                while (!KEY);  // 等待按键松开
            }
        }
    }
}
