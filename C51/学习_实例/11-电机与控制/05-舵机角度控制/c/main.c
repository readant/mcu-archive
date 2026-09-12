/*------------------------------------------------
  功能：舵机角度控制
  硬件：舵机信号线接P1.0
  说明：通过PWM控制舵机转到0°/90°/180°
        舵机PWM周期20ms
        0.5ms→0°, 1.5ms→90°, 2.5ms→180°
------------------------------------------------*/
#include <reg51.h>

sbit SERVO = P1^0;

void Delay(unsigned int t)
{
    while (--t);
}

// 控制舵机角度
// angle: 0~180度
void ServoAngle(unsigned char angle)
{
    unsigned int pulse_width;
    unsigned char i;

    // 脉宽 = 500 + angle * 11 (us)
    pulse_width = 500 + (unsigned int)angle * 11;

    for (i = 0; i < 50; i++)  // 重复多次保持位置
    {
        SERVO = 1;
        // 延时脉宽
        {
            unsigned int j = pulse_width;
            while (j--);
        }
        SERVO = 0;
        // 延时剩余周期
        {
            unsigned int j = 20000 - pulse_width;
            while (j--);
        }
    }
}

void main(void)
{
    while (1)
    {
        ServoAngle(0);      // 转到0°
        Delay(50000);

        ServoAngle(90);     // 转到90°
        Delay(50000);

        ServoAngle(180);    // 转到180°
        Delay(50000);
    }
}
