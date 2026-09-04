/*------------------------------------------------
  功能：UART控制LED
  硬件：P3.0=RXD，P3.1=TXD，P1口接LED
  说明：电脑发送命令控制单片机LED
        命令格式：L0~L7点亮，H0~H7熄灭
------------------------------------------------*/
#include <reg51.h>

sbit LED0 = P1^0;
sbit LED1 = P1^1;
sbit LED2 = P1^2;
sbit LED3 = P1^3;

void InitUART(void)
{
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
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

void UART_isr(void) interrupt 4
{
    unsigned char cmd;

    if (RI)
    {
        RI = 0;
        cmd = SBUF;

        switch (cmd)
        {
            case 'A': P1_0 = 0; SendString("LED0 ON\r\n"); break;
            case 'B': P1_0 = 1; SendString("LED0 OFF\r\n"); break;
            case 'C': P1_1 = 0; SendString("LED1 ON\r\n"); break;
            case 'D': P1_1 = 1; SendString("LED1 OFF\r\n"); break;
            case 'E': P1_2 = 0; SendString("LED2 ON\r\n"); break;
            case 'F': P1_2 = 1; SendString("LED2 OFF\r\n"); break;
            case 'G': P1_3 = 0; SendString("LED3 ON\r\n"); break;
            case 'H': P1_3 = 1; SendString("LED3 OFF\r\n"); break;
            default: SendString("Unknown cmd\r\n"); break;
        }
    }
}

void main(void)
{
    InitUART();
    P1 = 0xFF;  // 初始全灭
    SendString("UART LED Control Ready\r\n");
    SendString("A/B:LED0, C/D:LED1, E/F:LED2, G/H:LED3\r\n");

    while (1);
}
