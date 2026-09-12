/*------------------------------------------------
  功能：LCD1602显示温度（模拟）
  硬件：P0口接LCD数据口
  说明：模拟温度值显示，实际项目替换为DS18B20
------------------------------------------------*/
#include <reg51.h>

sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;

#define DataPort P0

// 模拟温度值
int temperature = 256;  // 25.6°C（放大10倍）

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

void ShowTemp(int temp)
{
    char buf[8];
    int abs_temp;
    unsigned char i = 0;

    if (temp < 0)
    {
        buf[i++] = '-';
        abs_temp = -temp;
    }
    else
    {
        abs_temp = temp;
    }

    buf[i++] = abs_temp / 100 + '0';
    buf[i++] = abs_temp % 100 / 10 + '0';
    buf[i++] = '.';
    buf[i++] = abs_temp % 10 + '0';
    buf[i++] = 'C';
    buf[i] = '\0';

    ShowString(1, 0, "        ");
    ShowString(1, 0, buf);
}

void main(void)
{
    InitLCD1602();
    ShowString(0, 0, "Temperature:");

    while (1)
    {
        ShowTemp(temperature);

        // 模拟温度变化
        temperature++;
        if (temperature > 300) temperature = 200;

        Delay(50000);
    }
}
