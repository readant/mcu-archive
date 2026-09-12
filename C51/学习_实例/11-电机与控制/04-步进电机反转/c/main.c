/*------------------------------------------------
  功能：步进电机反转
  硬件：28BYJ-48步进电机，ULN2003驱动
  说明：反转只需要将驱动序列倒序即可
------------------------------------------------*/
#include <reg51.h>

#define MOTOR_PORT P1

// 反转序列（正序的反向）
unsigned char code motor_seq[] = {
    0x09,  // 1001
    0x08,  // 1000
    0x0C,  // 1100
    0x04,  // 0100
    0x06,  // 0110
    0x02,  // 0010
    0x03,  // 0011
    0x01   // 0001
};

void Delay(unsigned int t)
{
    while (--t);
}

void MotorReverse(void)
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
    while (1)
    {
        MotorReverse();
    }
}
