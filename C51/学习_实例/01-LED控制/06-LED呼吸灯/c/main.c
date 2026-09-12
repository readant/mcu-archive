/*------------------------------------------------
  功能：LED呼吸灯（软件PWM）
  硬件：P1.0口接LED，低电平点亮
  说明：通过改变占空比实现亮度渐变
        占空比从0%→100%→0%循环
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;

void Delay(unsigned char t)
{
    while (--t);
}

void main(void)
{
    unsigned char period = 200;  // PWM周期
    unsigned char duty;          // 占空比
    signed char step = 5;       // 步进值（正=变亮，负=变暗）

    duty = 0;

    while (1)
    {
        /*--- 一个PWM周期 ---*/
        LED = 0;                // 点亮
        Delay(duty);
        LED = 1;                // 熄灭
        Delay(period - duty);

        /*--- 调整占空比 ---*/
        duty += step;
        if (duty >= period)
        {
            step = -5;          // 到最亮，开始变暗
        }
        else if (duty == 0)
        {
            step = 5;           // 到最暗，开始变亮
        }
    }
}
