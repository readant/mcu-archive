/*------------------------------------------------
  功能：长按短按检测
  硬件：P2.0接按键，P1.0接LED1（短按），P1.1接LED2（长按）
  说明：短按（<1秒）点亮LED1，长按（>1秒）点亮LED2
------------------------------------------------*/
#include <reg51.h>

sbit KEY = P2^0;
sbit LED1 = P1^0;
sbit LED2 = P1^1;

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    unsigned int hold_time = 0;

    LED1 = 1;
    LED2 = 1;

    while (1)
    {
        if (KEY == 0)  // 按键按下
        {
            Delay(20);
            if (KEY == 0)
            {
                hold_time = 0;
                while (KEY == 0 && hold_time < 100)
                {
                    Delay(10);
                    hold_time++;
                }

                if (hold_time >= 100)  // 长按（约1秒）
                {
                    LED2 = 0;          // 点亮长按LED
                    while (KEY == 0);  // 等待松手
                    LED2 = 1;
                }
                else  // 短按
                {
                    LED1 = 0;          // 点亮短按LED
                    Delay(50000);      // 保持一段时间
                    LED1 = 1;
                }
            }
        }
    }
}
