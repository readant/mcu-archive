/*------------------------------------------------
  功能：DS1302可调时钟
  硬件：DS1302接P3.4~P3.6，按键接P2.0~P2.2
  说明：3个按键分别：切换字段、加1、减1
------------------------------------------------*/
#include <reg51.h>

sbit SCLK = P3^4;
sbit IO = P3^5;
sbit CE = P3^6;
sbit KEY_SET = P2^0;    // 设置键
sbit KEY_ADD = P2^1;    // 加1键
sbit KEY_DEC = P2^2;    // 减1键
sbit LATCH1 = P2^3;
sbit LATCH2 = P2^4;

unsigned char code duanma[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char code weima[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF};

unsigned char time_buf[3] = {0, 0, 12};  // 秒、分、时
unsigned char set_field = 0;  // 0=不设置，1=时，2=分，3=秒

void Delay(unsigned int t) { while (--t); }

void DS1302_WriteByte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++) { IO = dat & 0x01; SCLK = 1; SCLK = 0; dat >>= 1; }
}

unsigned char DS1302_ReadByte(void)
{
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++) { dat >>= 1; if (IO) dat |= 0x80; SCLK = 1; SCLK = 0; }
    return dat;
}

void DS1302_Write(unsigned char addr, unsigned char dat)
{
    CE = 0; SCLK = 0; CE = 1;
    DS1302_WriteByte(addr); DS1302_WriteByte(dat);
    CE = 0;
}

unsigned char DS1302_Read(unsigned char addr)
{
    unsigned char dat;
    CE = 0; SCLK = 0; CE = 1;
    DS1302_WriteByte(addr | 0x01); dat = DS1302_ReadByte();
    CE = 0;
    return dat;
}

void ReadTime(void)
{
    time_buf[0] = DS1302_Read(0x80);
    time_buf[1] = DS1302_Read(0x82);
    time_buf[2] = DS1302_Read(0x84);
}

void WriteTime(void)
{
    DS1302_Write(0x80, time_buf[0]);
    DS1302_Write(0x82, time_buf[1]);
    DS1302_Write(0x84, time_buf[2]);
}

void Display(void)
{
    unsigned char i;
    unsigned char disp[6];

    disp[0] = time_buf[2] / 16;
    disp[1] = time_buf[2] % 16;
    disp[2] = time_buf[1] / 16;
    disp[3] = time_buf[1] % 16;
    disp[4] = time_buf[0] / 16;
    disp[5] = time_buf[0] % 16;

    for (i = 0; i < 6; i++)
    {
        P0 = 0x00; LATCH1 = 1; LATCH1 = 0;
        P0 = weima[i]; LATCH2 = 1; LATCH2 = 0;
        P0 = duanma[disp[i]];
        // 设置字段闪烁
        if (set_field > 0 && (i/2 + 1) == set_field && (Delay(500), 1))
            P0 = 0x00;  // 熄灭表示选中
        LATCH1 = 1; LATCH1 = 0;
        Delay(5);
    }
}

void KeyScan(void)
{
    if (KEY_SET == 0)
    {
        Delay(20);
        if (KEY_SET == 0)
        {
            set_field++;
            if (set_field > 3)
            {
                set_field = 0;
                WriteTime();  // 保存设置
            }
            while (!KEY_SET);
        }
    }

    if (set_field > 0)
    {
        if (KEY_ADD == 0)
        {
            Delay(20);
            if (KEY_ADD == 0)
            {
                time_buf[set_field - 1]++;
                while (!KEY_ADD);
            }
        }
        if (KEY_DEC == 0)
        {
            Delay(20);
            if (KEY_DEC == 0)
            {
                time_buf[set_field - 1]--;
                while (!KEY_DEC);
            }
        }
    }
}

void main(void)
{
    DS1302_Write(0x8E, 0x00);

    while (1)
    {
        if (set_field == 0) ReadTime();
        Display();
        KeyScan();
    }
}
