/*------------------------------------------------
  功能：电子钟（DS1302+LCD1602）
  硬件：DS1302接P3.4~P3.6，LCD1602显示，按键调时
  说明：完整电子钟，显示年月日时分秒
------------------------------------------------*/
#include <reg51.h>

sbit SCLK = P3^4;
sbit IO = P3^5;
sbit CE = P3^6;
sbit KEY_SET = P2^0;
sbit KEY_ADD = P2^1;
sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;

#define DataPort P0

unsigned char set_field = 0;  // 0=不设置，1=时，2=分，3=秒
unsigned char time_buf[3];    // 时、分、秒

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

void DS1302_WriteByte(unsigned char dat) { unsigned char i; for(i=0;i<8;i++){IO=dat&0x01;SCLK=1;SCLK=0;dat>>=1;} }
unsigned char DS1302_ReadByte(void) { unsigned char i,dat=0; for(i=0;i<8;i++){dat>>=1;if(IO)dat|=0x80;SCLK=1;SCLK=0;} return dat; }
void DS1302_Write(unsigned char addr, unsigned char dat) { CE=0;SCLK=0;CE=1;DS1302_WriteByte(addr);DS1302_WriteByte(dat);CE=0; }
unsigned char DS1302_Read(unsigned char addr) { unsigned char dat; CE=0;SCLK=0;CE=1;DS1302_WriteByte(addr|0x01);dat=DS1302_ReadByte();CE=0; return dat; }

void ReadTime(void)
{
    time_buf[2] = DS1302_Read(0x84);  // 时
    time_buf[1] = DS1302_Read(0x82);  // 分
    time_buf[0] = DS1302_Read(0x80);  // 秒
}

void WriteTime(void)
{
    DS1302_Write(0x8E, 0x00);
    DS1302_Write(0x84, time_buf[2]);
    DS1302_Write(0x82, time_buf[1]);
    DS1302_Write(0x80, time_buf[0]);
    DS1302_Write(0x8E, 0x80);
}

void ShowTime(void)
{
    char buf[9];
    buf[0] = time_buf[2]/16+'0';
    buf[1] = time_buf[2]%16+'0';
    buf[2] = ':';
    buf[3] = time_buf[1]/16+'0';
    buf[4] = time_buf[1]%16+'0';
    buf[5] = ':';
    buf[6] = time_buf[0]/16+'0';
    buf[7] = time_buf[0]%16+'0';
    buf[8] = '\0';
    ShowString(1, 0, buf);
}

void main(void)
{
    DS1302_Write(0x8E, 0x00);
    InitLCD1602();
    ShowString(0, 0, "Time:");

    while (1)
    {
        if (set_field == 0) ReadTime();
        ShowTime();

        if (KEY_SET == 0)
        {
            Delay(20);
            if (KEY_SET == 0)
            {
                set_field++;
                if (set_field > 3) { set_field=0; WriteTime(); }
                while (!KEY_SET);
            }
        }

        if (set_field > 0 && KEY_ADD == 0)
        {
            Delay(20);
            if (KEY_ADD == 0)
            {
                time_buf[set_field-1]++;
                while (!KEY_ADD);
            }
        }
    }
}
