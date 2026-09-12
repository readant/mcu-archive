/*
 * 无线遥控应用
 * 功能：通过无线模块实现遥控LED和电机
 * 硬件：无线接收模块→P3.2中断脚，4路遥控按键
 * 说明：无线遥控是物联网和智能家居的基础
 *       本例使用简易无线模块，配合DS1302+DS18B20实现综合遥控系统
 */
#include <reg51.h>

sbit wireless_data = P3^2;  // 无线接收数据脚
sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit MOTOR = P2^2;

volatile unsigned char recv_code = 0;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 外部中断0接收无线信号
 */
void int0_isr(void) interrupt 0
{
    unsigned char i, code_val = 0;

    /* 简易协议：8位数据，每位1ms */
    for (i = 0; i < 8; i++)
    {
        delay_ms(1);
        code_val >>= 1;
        if (wireless_data)
            code_val |= 0x80;
    }
    recv_code = code_val;
}

void main(void)
{
    IT0 = 1;  // 下降沿触发
    EX0 = 1;  // 使能外部中断0
    EA = 1;   // 开总中断

    while (1)
    {
        switch (recv_code)
        {
            case 0x01:  // 按键1：LED1开关
                LED1 = ~LED1;
                recv_code = 0;
                break;
            case 0x02:  // 按键2：LED2开关
                LED2 = ~LED2;
                recv_code = 0;
                break;
            case 0x04:  // 按键3：电机开关
                MOTOR = ~MOTOR;
                recv_code = 0;
                break;
            case 0x08:  // 按键4：全开
                LED1 = 0; LED2 = 0; MOTOR = 0;
                recv_code = 0;
                break;
        }
        delay_ms(100);
    }
}
