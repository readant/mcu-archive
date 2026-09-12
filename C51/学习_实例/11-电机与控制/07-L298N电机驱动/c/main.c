/*------------------------------------------------
  功能：L298N电机驱动（双路）
  硬件：L298N模块
        ENA=P1.0, IN1=P1.1, IN2=P1.2 (电机A)
        ENB=P1.3, IN3=P1.4, IN4=P1.5 (电机B)
  说明：同时控制两个直流电机
------------------------------------------------*/
#include <reg51.h>

// 电机A
sbit ENA = P1^0;
sbit IN1 = P1^1;
sbit IN2 = P1^2;

// 电机B
sbit ENB = P1^3;
sbit IN3 = P1^4;
sbit IN4 = P1^5;

void Delay(unsigned int t)
{
    while (--t);
}

// 电机A控制
void MotorA_Forward(void)  { IN1=1; IN2=0; ENA=1; }
void MotorA_Reverse(void)  { IN1=0; IN2=1; ENA=1; }
void MotorA_Stop(void)     { IN1=0; IN2=0; ENA=0; }

// 电机B控制
void MotorB_Forward(void)  { IN3=1; IN4=0; ENB=1; }
void MotorB_Reverse(void)  { IN3=0; IN4=1; ENB=1; }
void MotorB_Stop(void)     { IN3=0; IN4=0; ENB=0; }

void main(void)
{
    while (1)
    {
        MotorA_Forward();
        MotorB_Reverse();
        Delay(50000);

        MotorA_Reverse();
        MotorB_Forward();
        Delay(50000);

        MotorA_Stop();
        MotorB_Stop();
        Delay(50000);
    }
}
