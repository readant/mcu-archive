/*------------------------------------------------
  功能：LCD12864基础显示
  硬件：P0口接数据口，P2.5=RS，P2.6=RW，P2.7=E，P2.3=PSB
  说明：LCD12864显示中文和图形
------------------------------------------------*/
#include <reg51.h>

sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;
sbit PSB = P2^3;

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

void InitLCD12864(void)
{
    PSB = 1;            // 并口模式
    Delay(5000);
    WriteCmd(0x30);     // 基本指令集
    WriteCmd(0x0C);     // 显示开
    WriteCmd(0x01);     // 清屏
    Delay(5000);
}

void SetPosition(unsigned char row, unsigned char col)
{
    unsigned char addr;
    switch (row)
    {
        case 0: addr = 0x80 + col; break;
        case 1: addr = 0x90 + col; break;
        case 2: addr = 0x88 + col; break;
        case 3: addr = 0x98 + col; break;
    }
    WriteCmd(addr);
}

// 显示中文（GB编码的高位字节）
void ShowChinese(unsigned char row, unsigned char col, unsigned char high, unsigned char low)
{
    SetPosition(row, col);
    WriteData(high);
    WriteData(low);
}

void main(void)
{
    InitLCD12864();

    // 第一行：显示中文（示例编码）
    SetPosition(0, 0);
    WriteData(0xCE);
    WriteData(0xD2);  // "你"
    WriteData(0xBA);
    WriteData(0xC3);  // "好"

    // 第二行：显示英文
    SetPosition(1, 0);
    WriteData('H');
    WriteData('e');
    WriteData('l');
    WriteData('l');
    WriteData('o');

    while (1);
}
