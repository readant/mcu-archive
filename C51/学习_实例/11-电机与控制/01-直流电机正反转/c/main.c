/*------------------------------------------------
  功能：直流电机正反转
  硬件：L298N IN1=P1.0, IN2=P1.1, ENA=P1.2
  说明：正转→停止→反转→停止循环
------------------------------------------------*/
#include <reg51.h>

sbit IN1 = P1^0;
sbit IN2 = P1^1;
sbit ENA = P1^2;

void Delay(unsigned int t)
{
    while (--t);
}

void MotorForward(void)
{
    IN1 = 1; IN2 = 0; ENA = 1;  // 正转
}

void MotorReverse(void)
{
    IN1 = 0; IN2 = 1; ENA = 1;  // 反转
}

void MotorStop(void)
{
    IN1 = 0; IN2 = 0; ENA = 0;  // 停止
}

void main(void)
{
    while (1)
    {
        MotorForward();
        Delay(50000);

        MotorStop();
        Delay(50000);

        MotorReverse();
        Delay(50000);

        MotorStop();
        Delay(50000);
    }
}
