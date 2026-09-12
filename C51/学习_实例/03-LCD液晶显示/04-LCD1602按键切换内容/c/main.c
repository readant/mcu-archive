/*------------------------------------------------
  功能：LCD1602按键切换显示内容
  硬件：P2.0接按键，P0口接LCD数据口
  说明：按按键切换显示不同信息页面
------------------------------------------------*/
#include <reg51.h>

sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;
sbit KEY = P2^0;

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

void ShowPage(unsigned char page)
{
    WriteCmd(0x01);  // 清屏
    Delay(5000);
    switch (page)
    {
        case 0:
            ShowString(0, 0, "Page 1: Hello!");
            ShowString(1, 0, "Press KEY to go on");
            break;
        case 1:
            ShowString(0, 0, "Page 2: Weather");
            ShowString(1, 0, "Temp: 25.6C");
            break;
        case 2:
            ShowString(0, 0, "Page 3: Time");
            ShowString(1, 0, "12:00:00");
            break;
    }
}

void main(void)
{
    unsigned char page = 0;

    InitLCD1602();
    ShowPage(page);

    while (1)
    {
        if (KEY == 0)
        {
            Delay(20);
            if (KEY == 0)
            {
                page++;
                if (page > 2) page = 0;
                ShowPage(page);
                while (!KEY);
            }
        }
    }
}
