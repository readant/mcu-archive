/*------------------------------------------------
  功能：矩阵键盘密码锁
  硬件：P1口接4x4矩阵键盘，P0.0接LED（开锁指示）
  说明：输入4位密码，正确则LED亮
------------------------------------------------*/
#include <reg51.h>

sbit LOCK = P0^0;

unsigned char code keymap[] = {
    1, 2, 3, 10,     // 10=A
    4, 5, 6, 11,     // 11=B
    7, 8, 9, 12,     // 12=C
    0, 0, 0, 13      // 13=D（确认）
};

unsigned char password[] = {1, 2, 3, 4};  // 正确密码
unsigned char input[4];                     // 输入缓冲
unsigned char pos = 0;                      // 当前输入位置

unsigned char KeyScan(void)
{
    unsigned char row, col;
    unsigned char temp;

    for (row = 0; row < 4; row++)
    {
        P1 = ~(0x01 << row);
        temp = P1 & 0xF0;
        if (temp != 0xF0)
        {
            for (col = 0; col < 4; col++)
            {
                if (!(temp & (0x10 << col)))
                {
                    return keymap[row * 4 + col];
                }
            }
        }
    }
    return 0xFF;
}

void Delay(unsigned int t)
{
    while (--t);
}

void main(void)
{
    unsigned char key;
    unsigned char i;
    bit correct;

    LOCK = 1;  // 初始锁定

    while (1)
    {
        key = KeyScan();
        if (key != 0xFF)
        {
            Delay(20);  // 消抖
            if (KeyScan() == key)
            {
                if (key == 13)  // 按下确认键
                {
                    correct = 1;
                    for (i = 0; i < 4; i++)
                    {
                        if (input[i] != password[i])
                        {
                            correct = 0;
                            break;
                        }
                    }
                    LOCK = correct ? 0 : 1;  // 密码正确开锁
                    pos = 0;
                }
                else if (pos < 4)
                {
                    input[pos] = key;
                    pos++;
                }
                while (KeyScan() != 0xFF);  // 等待松手
            }
        }
    }
}
