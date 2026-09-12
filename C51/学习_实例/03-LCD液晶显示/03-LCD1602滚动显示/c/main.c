/*------------------------------------------------
  功能：LCD1602滚动显示
  硬件：P0口接数据口
  说明：文字从右向左滚动显示
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
    if (row == 0)
        WriteCmd(0x80 + col);
    else
        WriteCmd(0x80 + 0x40 + col);
    while (*str) WriteData(*str++);
}

void main(void)
{
    unsigned char i;
    char *msg = "    Welcome to 51 MCU World!    ";

    InitLCD1602();

    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            ShowString(0, i, msg + i);
            Delay(50000);
        }
    }
}
