/*------------------------------------------------
  功能：交通灯系统
  硬件：P1.0~P1.2接红黄绿灯（东西），P1.3~P1.5接红黄绿灯（南北）
  说明：模拟十字路口交通灯切换
------------------------------------------------*/
#include <reg51.h>

// 东西方向
sbit RED_EW = P1^0;
sbit YELLOW_EW = P1^1;
sbit GREEN_EW = P1^2;

// 南北方向
sbit RED_NS = P1^3;
sbit YELLOW_NS = P1^4;
sbit GREEN_NS = P1^5;

void Delay(unsigned int t)
{
    while (--t);
}

void TrafficLight(unsigned char state)
{
    switch (state)
    {
        case 0:  // 东西绿灯，南北红灯
            GREEN_EW = 0; YELLOW_EW = 1; RED_EW = 1;
            GREEN_NS = 1; YELLOW_NS = 1; RED_NS = 0;
            Delay(50000);
            break;

        case 1:  // 东西黄灯闪，南北红灯
            GREEN_EW = 1; YELLOW_EW = 0; RED_EW = 1;
            GREEN_NS = 1; YELLOW_NS = 1; RED_NS = 0;
            Delay(5000);
            break;

        case 2:  // 东西红灯，南北绿灯
            GREEN_EW = 1; YELLOW_EW = 1; RED_EW = 0;
            GREEN_NS = 0; YELLOW_NS = 1; RED_NS = 1;
            Delay(50000);
            break;

        case 3:  // 东西红灯，南北黄灯闪
            GREEN_EW = 1; YELLOW_EW = 1; RED_EW = 0;
            GREEN_NS = 1; YELLOW_NS = 0; RED_NS = 1;
            Delay(5000);
            break;
    }
}

void main(void)
{
    while (1)
    {
        TrafficLight(0);
        TrafficLight(1);
        TrafficLight(2);
        TrafficLight(3);
    }
}
