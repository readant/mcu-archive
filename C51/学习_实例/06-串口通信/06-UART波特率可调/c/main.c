/*------------------------------------------------
  功能：UART波特率可调
  硬件：P3.0=RXD，P3.1=TXD，P2.0~P2.3接按键
  说明：通过按键切换波特率：9600/4800/19200/115200
------------------------------------------------*/
#include <reg51.h>

sbit KEY = P2^0;

// 常用波特率的定时器重装值（11.0592MHz）
unsigned char code baudrate_tab[] = {
    0xFD,   // 9600bps
    0xFA,   // 4800bps
    0xF8,   // 19200bps (近似)
    0xFF    // 115200bps (近似)
};

unsigned char baud_index = 0;  // 默认9600

void InitUART(void)
{
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = baudrate_tab[baud_index];
    TL1 = baudrate_tab[baud_index];
    TR1 = 1;
    ES = 1;
    EA = 1;
}

void SendByte(unsigned char dat)
{
    SBUF = dat;
    while (!TI);
    TI = 0;
}

void SendString(char *str)
{
    while (*str) SendByte(*str++);
}

void Delay(unsigned int t)
{
    while (--t);
}

void UART_isr(void) interrupt 4
{
    if (RI)
    {
        RI = 0;
        SendByte(SBUF);  // 回显
    }
}

void main(void)
{
    InitUART();

    while (1)
    {
        if (KEY == 0)
        {
            Delay(20);
            if (KEY == 0)
            {
                baud_index++;
                if (baud_index >= 4) baud_index = 0;

                // 切换波特率
                TR1 = 0;
                TH1 = baudrate_tab[baud_index];
                TL1 = baudrate_tab[baud_index];
                TR1 = 1;

                while (!KEY);
            }
        }
    }
}
