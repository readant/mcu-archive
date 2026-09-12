/*
 * 常用C51库函数汇总
 * 功能：演示C51特有的库函数用法
 * 硬件：无（纯软件示例）
 * 说明：C51编译器提供了特有的库函数
 *       intrins.h: _nop_(), _crol_(), _cror_(), _chkfloat_()
 *       stdlib.h:  abs(), srand(), rand()
 *       ctype.h:   isalpha(), isdigit(), toupper()
 *       string.h:  strlen(), strcpy(), strcmp()
 */
#include <reg51.h>
#include <stdlib.h>
#include <string.h>

/* intrins.h函数在reg51.h中已包含 */
/* _nop_()   - 空操作，延时1个机器周期 */
/* _crol_()  - 循环左移 */
/* _cror_()  - 循环右移 */
/* _testbit_() - 测试位变量并清零 */

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void main(void)
{
    unsigned char led = 0x01;
    unsigned int rand_num;
    char str1[] = "Hello";
    char str2[] = "World";

    while (1)
    {
        /* _crol_ 循环左移 */
        led = _crol_(led, 1);  // 等价于 (led<<1)|(led>>7)
        P0 = led;
        delay_ms(200);

        /* _nop_ 精确延时 */
        _nop_(); _nop_(); _nop_(); _nop_();  // 4个机器周期

        /* abs 绝对值 */
        {
            signed char a = -10;
            unsigned char b = abs(a);  // b = 10
        }

        /* rand 随机数 */
        srand(12345);  // 种子
        rand_num = rand() % 256;  // 0-255随机数
        P1 = (unsigned char)rand_num;

        /* strlen 字符串长度 */
        {
            unsigned char len = strlen(str1);  // len = 5
        }

        /* strcmp 字符串比较 */
        if (strcmp(str1, str2) < 0)
        {
            P2_0 = 0;  // str1 < str2
        }

        delay_ms(500);
    }
}
