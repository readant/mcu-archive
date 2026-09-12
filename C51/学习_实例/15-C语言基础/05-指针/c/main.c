/*------------------------------------------------
  功能：C51指针演示
  硬件：P0口接LED
  说明：展示指针的基本用法
------------------------------------------------*/
#include <reg51.h>

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    unsigned char a = 10;
    unsigned char b = 20;
    unsigned char *ptr;  // 定义指针

    /*--- 指针赋值 ---*/
    ptr = &a;    // ptr指向a的地址

    /*--- 通过指针访问变量 ---*/
    P0 = *ptr;   // 读取指针指向的值，P0 = 10

    *ptr = 50;   // 通过指针修改a的值，a = 50

    /*--- 指针运算 ---*/
    ptr = &b;    // ptr指向b
    P0 = *ptr;   // P0 = 20

    /*--- 指针与数组 ---*/
    unsigned char arr[] = {0xFE, 0xFD, 0xFB, 0xF7};
    unsigned char *p = arr;  // p指向数组首地址

    while (1)
    {
        P0 = *p;      // 取指针指向的值
        p++;          // 指针加1，指向下一个元素
        if (p > &arr[3])
            p = arr;  // 超出范围，回到开头
        Delay(50000);
    }
}
