/*------------------------------------------------
  功能：外部中断1急停按钮
  硬件：P3.3接按键（INT1），P1.0接LED，P1.1接电机
  说明：正常时LED闪烁+电机转动，按下急停立即停止
------------------------------------------------*/
#include <reg51.h>

sbit LED = P1^0;
sbit MOTOR = P1^1;

bit running = 1;  // 运行标志

void Init_INT1(void)
{
    IT1 = 1;  // 下降沿触发
    EX1 = 1;  // 开外部中断1
    EA = 1;
}

// 外部中断1服务函数（急停）
void INT1_isr(void) interrupt 2
{
    running = 0;   // 停止运行
    LED = 1;       // 关闭LED
    MOTOR = 1;     // 关闭电机
}

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    Init_INT1();

    while (1)
    {
        if (running)
        {
            LED = 0;       // 点亮LED
            MOTOR = 0;     // 启动电机
            Delay(50000);
            LED = 1;       // 熄灭LED
            MOTOR = 1;     // 停止电机
            Delay(50000);
        }
    }
}
