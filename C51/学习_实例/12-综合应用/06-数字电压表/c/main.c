/*------------------------------------------------
  功能：数字电压表
  硬件：ADC0832接P1口，LCD1602显示
  说明：精确显示电压值，支持0~5V
------------------------------------------------*/
#include <reg51.h>

sbit CS = P1^3;
sbit CLK = P1^2;
sbit DIO = P1^1;
sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;

#define DataPort P0

void Delay(unsigned int t) { while (--t); }
void DelayUs(unsigned char us) { while (us--); }

void WriteCmd(unsigned char cmd) { RS=0; RW=0; E=0; DataPort=cmd; E=1; Delay(5); E=0; Delay(5); }
void WriteData(unsigned char dat) { RS=1; RW=0; E=0; DataPort=dat; E=1; Delay(5); E=0; Delay(5); }

void InitLCD1602(void) { Delay(15000); WriteCmd(0x38); WriteCmd(0x0C); WriteCmd(0x06); WriteCmd(0x01); Delay(5000); }

void ShowString(unsigned char row, unsigned char col, char *str)
{
    if (row==0) WriteCmd(0x80+col); else WriteCmd(0x80+0x40+col);
    while (*str) WriteData(*str++);
}

unsigned char Read_ADC0832(unsigned char channel)
{
    unsigned char i, dat=0;
    CS=0; CLK=0; DIO=1; CLK=1; CLK=0;
    DIO=channel; CLK=1; CLK=0;
    DIO=1; CLK=1; CLK=0;
    for (i=0; i<8; i++) { CLK=1; CLK=0; dat<<=1; if(DIO) dat|=0x01; }
    CS=1;
    return dat;
}

void ShowVoltage(unsigned char ad_val)
{
    unsigned int voltage;
    char buf[8];
    unsigned char i=0;

    voltage = (unsigned int)ad_val * 500 / 255;

    buf[i++] = voltage/100 + '0';
    buf[i++] = '.';
    buf[i++] = voltage%100/10 + '0';
    buf[i++] = voltage%10 + '0';
    buf[i++] = 'V';
    buf[i] = '\0';

    ShowString(1, 0, "        ");
    ShowString(1, 0, buf);
}

void main(void)
{
    unsigned char ad_val;

    InitLCD1602();
    ShowString(0, 0, "Voltage:");

    while (1)
    {
        ad_val = Read_ADC0832(0);
        ShowVoltage(ad_val);
        Delay(50000);
    }
}
