/*------------------------------------------------
  功能：定时器PWM输出
  硬件：P1.0口接LED
  说明：用定时器中断模拟PWM，实现呼吸灯效果
        比软件PWM更精确
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;

unsigned char pwm_duty = 0;   // 占空比 0~200
signed char pwm_step = 1;     // 步进值

void Init_Timer0(void)
{
    TMOD |= 0x01;
    TH0 = (65536 - 100) / 256;   // 100us，PWM周期=20ms
    TL0 = (65536 - 100) % 256;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void Timer0_isr(void) interrupt 1
{
    static unsigned char pwm_cnt = 0;

    TH0 = (65536 - 100) / 256;
    TL0 = (65536 - 100) % 256;

    pwm_cnt++;

    // PWM输出
    if (pwm_cnt < pwm_duty)
        LED = 0;     // 点亮
    else
        LED = 1;     // 熄灭

    // 一个PWM周期结束（200 x 100us = 20ms）
    if (pwm_cnt >= 200)
    {
        pwm_cnt = 0;

        // 调整占空比
        pwm_duty += pwm_step;
        if (pwm_duty >= 200)
            pwm_step = -1;
        else if (pwm_duty == 0)
            pwm_step = 1;
    }
}

void main(void)
{
    Init_Timer0();
    while (1);
}
