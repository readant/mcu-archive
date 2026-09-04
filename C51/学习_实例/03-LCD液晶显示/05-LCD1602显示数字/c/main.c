/*------------------------------------------------
  功能：LCD1602显示数字
  硬件：P0口接数据口
  说明：将整数转换为字符串显示在LCD上
------------------------------------------------*/
#include <reg51.h>

sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;

#define DataPort P0

void Delay(unsigned int t)
{
    while (--t);
}

void WriteCmd(unsigned char cmd)
{
    RS = 0;  RW = 0;  E = 0;
    DataPort = cmd;
    E = 1;  Delay(5);  E = 0;
    Delay(5);
}

void WriteData(unsigned char dat)
{
    RS = 1;  RW = 0;  E = 0;
    DataPort = dat;
    E = 1;  Delay(5);  E = 0;
    Delay(5);
}

void InitLCD1602(void)
{
    Delay(15000);
    WriteCmd(0x38);
    WriteCmd(0x0C);
    WriteCmd(0x06);
    WriteCmd(0x01);
    Delay(5000);
}

void ShowString(unsigned char row, unsigned char col, char *str)
{
    if (row == 0) WriteCmd(0x80 + col);
    else WriteCmd(0x80 + 0x40 + col);
    while (*str) WriteData(*str++);
}

// 整数转字符串（支持负数）
void IntToString(int num, char *str)
{
    unsigned char i = 0;
    unsigned char j;
    char temp;

    if (num < 0)
    {
        str[i++] = '-';
        num = -num;
    }

    if (num >= 10000) str[i++] = num / 10000 + '0';
    if (num >= 1000) str[i++] = num % 10000 / 1000 + '0';
    if (num >= 100) str[i++] = num % 1000 / 100 + '0';
    if (num >= 10) str[i++] = num % 100 / 10 + '0';
    str[i++] = num % 10 + '0';
    str[i] = '\0';
}

void main(void)
{
    int count = 0;
    char buf[8];

    InitLCD1602();
    ShowString(0, 0, "Counter:");

    while (1)
    {
        IntToString(count, buf);
        ShowString(1, 0, "        ");  // 清除旧数据
        ShowString(1, 0, buf);

        count++;
        if (count > 9999) count = 0;

        Delay(50000);
    }
}
