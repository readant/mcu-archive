/*------------------------------------------------
  功能：C51函数演示
  硬件：P0口接LED
  说明：展示函数的定义、调用、参数传递
------------------------------------------------*/
#include <reg51.h>

/*--- 函数声明 ---*/
void Delay(unsigned int t);
void LED_Blink(void);
unsigned char Add(unsigned char a, unsigned char b);

/*--- 延时函数 ---*/
void Delay(unsigned int t)
{
    while (--t);
}

/*--- LED闪烁函数 ---*/
void LED_Blink(void)
{
    P0 = 0x00;
    Delay(50000);
    P0 = 0xFF;
    Delay(50000);
}

/*--- 带返回值的函数 ---*/
unsigned char Add(unsigned char a, unsigned char b)
{
    return a + b;
}

/*--- 主函数 ---*/
void main(void)
{
    unsigned char result;

    /*--- 函数调用 ---*/
    LED_Blink();

    /*--- 带参数的函数调用 ---*/
    result = Add(10, 20);  // result = 30
    P0 = result;

    /*--- 无返回值函数 ---*/
    while (1)
    {
        LED_Blink();
    }
}
