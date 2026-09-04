/*------------------------------------------------
  功能：LCD1602自定义字符
  硬件：P0口接数据口
  说明：在CGRAM中创建自定义字符并显示
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

// 自定义字符：心形
unsigned char code Heart[] = {
    0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00
};

// 自定义字符：笑脸
unsigned char code Smile[] = {
    0x00, 0x0A, 0x0A, 0x00, 0x11, 0x0E, 0x00, 0x00
};

void CreateChar(unsigned char addr, unsigned char *dat)
{
    unsigned char i;
    WriteCmd(0x40 + addr * 8);  // 设置CGRAM地址
    for (i = 0; i < 8; i++)
    {
        WriteData(dat[i]);
    }
}

void main(void)
{
    InitLCD1602();

    // 创建自定义字符
    CreateChar(0, Heart);   // 心形 → 位置0
    CreateChar(1, Smile);   // 笑脸 → 位置1

    // 显示自定义字符
    WriteCmd(0x80);         // 第一行开头
    WriteData(0);           // 显示心形
    WriteData(1);           // 显示笑脸
    WriteData(0);           // 再显示心形

    while (1);
}
