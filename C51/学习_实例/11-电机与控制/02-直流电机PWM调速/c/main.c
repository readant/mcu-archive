/*------------------------------------------------
  功能：直流电机PWM调速
  硬件：L298N IN1=P1.0, IN2=P1.1, ENA=P1.2
  说明：通过PWM调节电机转速
------------------------------------------------*/
#include <reg51.h>

sbit IN1 = P1^0;
sbit IN2 = P1^1;
sbit ENA = P1^2;

void Delay(unsigned int t)
{
    while (--t);
}

// PWM输出
void PWM_Output(unsigned char duty)
{
    unsigned char period = 100;
    unsigned char i;

    for (i = 0; i < 50; i++)  // 重复50次让转速稳定
    {
        ENA = 1;
        Delay(duty);
        ENA = 0;
        Delay(period - duty);
    }
}

void main(void)
{
    IN1 = 1; IN2 = 0;  // 正转方向

    while (1)
    {
        PWM_Output(20);   // 20%速度
        PWM_Output(50);   // 50%速度
        PWM_Output(80);   // 80%速度
        PWM_Output(100);  // 全速
    }
}
