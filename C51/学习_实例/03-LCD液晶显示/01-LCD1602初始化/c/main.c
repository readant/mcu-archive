/*------------------------------------------------
  功能：LCD1602初始化
  硬件：P0口接数据口，P2.5=RS，P2.6=RW，P2.7=E
  说明：LCD1602最基本的初始化流程
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
    E = 1;
    Delay(5);
    E = 0;
    Delay(5);
}

void WriteData(unsigned char dat)
{
    RS = 1;  RW = 0;  E = 0;
    DataPort = dat;
    E = 1;
    Delay(5);
    E = 0;
    Delay(5);
}

void InitLCD1602(void)
{
    Delay(15000);     // 上电延时>15ms
    WriteCmd(0x38);   // 8位数据，2行，5x7
    WriteCmd(0x0C);   // 显示开，光标关
    WriteCmd(0x06);   // 写入后地址+1
    WriteCmd(0x01);   // 清屏
    Delay(5000);
}

void main(void)
{
    InitLCD1602();
    WriteData('H');
    WriteData('i');
    while (1);
}
