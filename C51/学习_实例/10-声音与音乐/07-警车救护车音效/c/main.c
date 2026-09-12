/*
 * 警车/救护车/消防车音效
 * 功能：通过蜂鸣器模拟多种报警音效
 * 硬件：P1.0接有源蜂鸣器（或P1.0接无源蜂鸣器+驱动电路）
 * 说明：不同报警器的音效频率和节奏不同
 *       警车：高低交替（频率变化快）
 *       救护车：高低交替（频率变化慢）
 *       消防车：长鸣+间歇
 */
#include <reg51.h>

sbit BUZZER = P1^0;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 蜂鸣器发声
 * freq: 频率（Hz）
 * ms:   持续时间（ms）
 */
void buzzer_tone(unsigned int freq, unsigned int ms)
{
    unsigned int period_us;
    unsigned int i;

    period_us = 1000000UL / freq / 2;

    for (i = 0; i < ms * 1000UL / (period_us * 2); i++)
    {
        BUZZER = 0;
        delay_us(period_us);
        BUZZER = 1;
        delay_us(period_us);
    }
}

/*
 * 警车音效
 */
void police_siren(void)
{
    unsigned int i;
    for (i = 0; i < 20; i++)
    {
        buzzer_tone(800, 50);
        buzzer_tone(600, 50);
    }
}

/*
 * 救护车音效
 */
void ambulance_siren(void)
{
    unsigned int i;
    for (i = 0; i < 10; i++)
    {
        buzzer_tone(500, 100);
        buzzer_tone(400, 100);
    }
}

/*
 * 消防车音效
 */
void fire_truck_siren(void)
{
    unsigned int i;
    for (i = 0; i < 5; i++)
    {
        buzzer_tone(700, 200);
        delay_ms(100);
    }
}

void delay_us(unsigned int us)
{
    while (us--);
}

void main(void)
{
    while (1)
    {
        police_siren();
        delay_ms(500);
        ambulance_siren();
        delay_ms(500);
        fire_truck_siren();
        delay_ms(500);
    }
}
