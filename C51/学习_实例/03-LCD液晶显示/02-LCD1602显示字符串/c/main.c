/*------------------------------------------------
  功能：LCD1602显示字符串
  硬件：P0口接数据口，P2.5=RS，P2.6=RW，P2.7=E
  说明：在指定位置显示字符串
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

void SetCursor(unsigned char row, unsigned char col)
{
    if (row == 0)
        WriteCmd(0x80 + col);       // 第一行
    else
        WriteCmd(0x80 + 0x40 + col); // 第二行
}

void ShowString(unsigned char row, unsigned char col, char *str)
{
    SetCursor(row, col);
    while (*str)
    {
        WriteData(*str++);
    }
}

void main(void)
{
    InitLCD1602();
    ShowString(0, 0, "Hello 51 MCU!");
    ShowString(1, 0, "LCD1602 Ready");
    while (1);
}
