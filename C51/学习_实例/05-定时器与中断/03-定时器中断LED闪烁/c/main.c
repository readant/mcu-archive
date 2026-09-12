/*------------------------------------------------
  功能：定时器中断控制LED闪烁
  硬件：P1口接8个LED
  说明：定时器0中断，LED依次闪烁
------------------------------------------------*/
#include <reg51.h>

unsigned char led_state = 0xFE;  // 初始：1111 1110

void Init_Timer0(void)
{
    TMOD |= 0x01;
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void Timer0_isr(void) interrupt 1
{
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    P1 = led_state;
    led_state = (led_state << 1) | 0x01;  // 左移

    if (led_state == 0xFF)  // 移完了
    {
        led_state = 0xFE;   // 重新开始
    }
}

void main(void)
{
    Init_Timer0();
    while (1);
}
