/*------------------------------------------------
  功能：数字温度计
  硬件：DS18B20接P3.7，LCD1602显示
  说明：实时温度显示，超限报警
------------------------------------------------*/
#include <reg51.h>

sbit DS18B20 = P3^7;
sbit BEEP = P1^5;
sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;

#define DataPort P0

#define ALARM_HIGH 350  // 高温报警阈值（35.0°C）
#define ALARM_LOW  100  // 低温报警阈值（10.0°C）

void Delay(unsigned int t) { while (--t); }
void DelayUs(unsigned char us) { while (us--); }

void WriteCmd(unsigned char cmd)
{
    RS=0; RW=0; E=0; DataPort=cmd; E=1; Delay(5); E=0; Delay(5);
}

void WriteData(unsigned char dat)
{
    RS=1; RW=0; E=0; DataPort=dat; E=1; Delay(5); E=0; Delay(5);
}

void InitLCD1602(void)
{
    Delay(15000);
    WriteCmd(0x38); WriteCmd(0x0C); WriteCmd(0x06); WriteCmd(0x01);
    Delay(5000);
}

void ShowString(unsigned char row, unsigned char col, char *str)
{
    if (row==0) WriteCmd(0x80+col); else WriteCmd(0x80+0x40+col);
    while (*str) WriteData(*str++);
}

bit Init_DS18B20(void)
{
    bit presence;
    DS18B20=1; DelayUs(2); DS18B20=0; DelayUs(500);
    DS18B20=1; DelayUs(60); presence=DS18B20; DelayUs(500);
    return presence;
}

void WriteByte(unsigned char dat)
{
    unsigned char i;
    for (i=0; i<8; i++) { DS18B20=0; DelayUs(2); DS18B20=dat&0x01; DelayUs(60); DS18B20=1; dat>>=1; }
}

unsigned char ReadByte(void)
{
    unsigned char i, dat=0;
    for (i=0; i<8; i++) { DS18B20=0; DelayUs(2); DS18B20=1; DelayUs(5); dat>>=1; if(DS18B20) dat|=0x80; DelayUs(60); }
    return dat;
}

int ReadTemp(void)
{
    unsigned char low, high;
    int temp;
    Init_DS18B20(); WriteByte(0xCC); WriteByte(0x44);
    Delay(1000);
    Init_DS18B20(); WriteByte(0xCC); WriteByte(0xBE);
    low=ReadByte(); high=ReadByte();
    temp=(high<<8)|low;
    return temp;
}

void ShowTemp(int temp)
{
    char buf[8];
    int abs_temp;
    unsigned char i=0;

    if (temp<0) { buf[i++]='-'; abs_temp=-temp; }
    else abs_temp=temp;

    buf[i++]=abs_temp/100+'0';
    buf[i++]=abs_temp%100/10+'0';
    buf[i++]='.';
    buf[i++]=abs_temp%10+'0';
    buf[i++]='C';
    buf[i]='\0';

    ShowString(1, 0, "        ");
    ShowString(1, 0, buf);
}

void main(void)
{
    int temp;

    InitLCD1602();
    ShowString(0, 0, "Temperature:");

    while (1)
    {
        temp=ReadTemp();
        ShowTemp(temp);

        if (temp>ALARM_HIGH || temp<ALARM_LOW)
        {
            BEEP=0; Delay(5000); BEEP=1;
        }

        Delay(50000);
    }
}
