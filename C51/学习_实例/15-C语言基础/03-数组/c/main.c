/*------------------------------------------------
  功能：C51数组演示
  硬件：P0口接LED
  说明：展示数组的定义、初始化、访问
------------------------------------------------*/
#include <reg51.h>

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    /*--- 数组定义与初始化 ---*/
    unsigned char arr1[5] = {1, 2, 3, 4, 5};
    unsigned char arr2[] = {0xFE, 0xFD, 0xFB, 0xF7};  // 自动确定大小
    unsigned char arr3[8] = {0};  // 全部初始化为0

    /*--- 一维数组访问 ---*/
    unsigned char i;
    for (i = 0; i < 5; i++)
    {
        P0 = arr1[i];
        Delay(50000);
    }

    /*--- 数组作为LED花样表 ---*/
    unsigned char code pattern[] = {
        0xFE, 0xFD, 0xFB, 0xF7,
        0xEF, 0xDF, 0xBF, 0x7F
    };

    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            P0 = pattern[i];
            Delay(50000);
        }
    }
}
