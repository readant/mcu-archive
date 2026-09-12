/*------------------------------------------------
  功能：C51结构体演示
  硬件：P0口接LED
  说明：展示结构体的定义和使用
------------------------------------------------*/
#include <reg51.h>

/*--- 定义结构体 ---*/
struct LED_State
{
    unsigned char port;     // 端口
    unsigned char pattern;  // 花样
    unsigned char speed;    // 速度
};

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    /*--- 结构体变量初始化 ---*/
    struct LED_State led1;
    led1.port = 0;
    led1.pattern = 0xFE;
    led1.speed = 50;

    struct LED_State led2 = {1, 0xFD, 100};

    /*--- 结构体数组 ---*/
    struct LED_State leds[3] = {
        {0, 0xFE, 50},
        {0, 0xFD, 100},
        {0, 0xFB, 150}
    };

    unsigned char i;

    /*--- 访问结构体成员 ---*/
    while (1)
    {
        for (i = 0; i < 3; i++)
        {
            P0 = leds[i].pattern;
            Delay(leds[i].speed);
        }
    }
}
