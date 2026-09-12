/*------------------------------------------------
  功能：步进电机正转
  硬件：28BYJ-48步进电机，ULN2003驱动
        IN1=P1.0, IN2=P1.1, IN3=P1.2, IN4=P1.3
  说明：四相八拍驱动，正转
------------------------------------------------*/
#include <reg51.h>

#define MOTOR_PORT P1

// 八拍驱动序列
unsigned char code motor_seq[] = {
    0x01,  // 0001
    0x03,  // 0011
    0x02,  // 0010
    0x06,  // 0110
    0x04,  // 0100
    0x0C,  // 1100
    0x08,  // 1000
    0x09   // 1001
};

void Delay(unsigned int t)
{
    while (--t);
}

void MotorForward(void)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        MOTOR_PORT = motor_seq[i];
        Delay(1000);
    }
}

void main(void)
{
    unsigned int steps = 0;

    while (1)
    {
        MotorForward();
        steps++;
        if (steps >= 512)  // 一圈约512步
        {
            steps = 0;
            MOTOR_PORT = 0x00;  // 停止
            Delay(50000);
        }
    }
}
