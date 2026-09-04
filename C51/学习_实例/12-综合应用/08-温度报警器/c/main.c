/*------------------------------------------------
  功能：温度报警器
  硬件：DS18B20接P3.7，LCD1602显示，蜂鸣器P1.5，按键P2.0/P2.1
  说明：可设置上下限温度，超限报警
------------------------------------------------*/
#include <reg51.h>

sbit DS18B20 = P3^7;
sbit BEEP = P1^5;
sbit KEY_UP = P2^0;
sbit KEY_DOWN = P2^1;
sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;

#define DataPort P0

int alarm_high = 350;  // 高温阈值（35.0°C）
int alarm_low = 100;   // 低温阈值（10.0°C）

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

bit Init_DS18B20(void) { bit p; DS18B20=1;DelayUs(2);DS18B20=0;DelayUs(500);DS18B20=1;DelayUs(60);p=DS18B20;DelayUs(500);return p; }
void WriteByte(unsigned char dat) { unsigned char i; for(i=0;i<8;i++){DS18B20=0;DelayUs(2);DS18B20=dat&0x01;DelayUs(60);DS18B20=1;dat>>=1;} }
unsigned char ReadByte(void) { unsigned char i,dat=0; for(i=0;i<8;i++){DS18B20=0;DelayUs(2);DS18B20=1;DelayUs(5);dat>>=1;if(DS18B20)dat|=0x80;DelayUs(60);} return dat; }

int ReadTemp(void)
{
    unsigned char low,high; int temp;
    Init_DS18B20(); WriteByte(0xCC); WriteByte(0x44); Delay(1000);
    Init_DS18B20(); WriteByte(0xCC); WriteByte(0xBE);
    low=ReadByte(); high=ReadByte();
    temp=(high<<8)|low; return temp;
}

void ShowTemp(int temp)
{
    char buf[8]; int abs_t; unsigned char i=0;
    if (temp<0){buf[i++]='-';abs_t=-temp;}else abs_t=temp;
    buf[i++]=abs_t/100+'0'; buf[i++]=abs_t%100/10+'0';
    buf[i++]='.'; buf[i++]=abs_t%10+'0'; buf[i++]='C'; buf[i]='\0';
    ShowString(0, 6, buf);
}

void ShowThreshold(void)
{
    char buf[8]; unsigned char i=0;
    buf[i++]=alarm_high/100+'0'; buf[i++]=alarm_high%100/10+'0';
    buf[i++]='~';
    buf[i++]=alarm_low/100+'0'; buf[i++]=alarm_low%100/10+'0';
    buf[i++]='C'; buf[i]='\0';
    ShowString(1, 0, buf);
}

void main(void)
{
    int temp;

    InitLCD1602();
    ShowString(0, 0, "Temp:");
    ShowString(1, 0, "Set:");

    while (1)
    {
        temp = ReadTemp();
        ShowTemp(temp);
        ShowThreshold();

        if (KEY_UP == 0) { Delay(20); if(KEY_UP==0){alarm_high+=10;while(!KEY_UP);} }
        if (KEY_DOWN == 0) { Delay(20); if(KEY_DOWN==0){alarm_high-=10;while(!KEY_DOWN);} }

        if (temp > alarm_high || temp < alarm_low)
        {
            BEEP = 0;
        }
        else
        {
            BEEP = 1;
        }

        Delay(50000);
    }
}
