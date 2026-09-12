/*------------------------------------------------
  功能：电子密码锁
  硬件：4x4矩阵键盘接P1，LCD1602显示，P0.0接锁
  说明：输入4位密码，正确开锁
------------------------------------------------*/
#include <reg51.h>

sbit LOCK = P0^0;
sbit RS = P2^5;
sbit RW = P2^6;
sbit E = P2^7;

#define DataPort P0

unsigned char code keymap[] = {
    1,2,3,10, 4,5,6,11, 7,8,9,12, 0,0,0,13
};

unsigned char password[] = {1,2,3,4};
unsigned char input[4];
unsigned char pos = 0;

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

unsigned char KeyScan(void)
{
    unsigned char row, col, temp;
    for (row=0; row<4; row++)
    {
        P1=~(0x01<<row);
        temp=P1&0xF0;
        if (temp!=0xF0)
        {
            for (col=0; col<4; col++)
            {
                if (!(temp&(0x10<<col)))
                    return keymap[row*4+col];
            }
        }
    }
    return 0xFF;
}

void main(void)
{
    unsigned char key;
    unsigned char i;
    bit correct;

    LOCK = 1;
    InitLCD1602();
    ShowString(0, 0, "Enter Password:");

    while (1)
    {
        key = KeyScan();
        if (key != 0xFF)
        {
            Delay(20);
            if (KeyScan() == key)
            {
                if (key == 13)  // 确认
                {
                    correct = 1;
                    for (i=0; i<4; i++)
                    {
                        if (input[i] != password[i]) { correct=0; break; }
                    }
                    LOCK = correct ? 0 : 1;
                    ShowString(1, 0, correct ? "OPEN!  " : "ERROR! ");
                    pos = 0;
                }
                else if (pos < 4)
                {
                    input[pos] = key;
                    pos++;
                    ShowString(1, 0, "****");
                }
                while (KeyScan() != 0xFF);
            }
        }
    }
}
