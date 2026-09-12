/*
 * 8按键电子琴（完整版）
 * 功能：8个按键对应8个音阶，按下按键发出对应音调
 * 硬件：P1.0-P1.7接8个按键，P2.0接蜂鸣器
 * 说明：8个音阶频率：C4=262, D4=294, E4=330, F4=349,
 *       G4=392, A4=440, B4=494, C5=523
 *       按键按下时持续发声，松开停止
 */
#include <reg51.h>

sbit BUZZER = P2^0;

/* 8个音阶频率（Hz） */
unsigned int code TONE_TABLE[] = {
    262, 294, 330, 349, 392, 440, 494, 523
};

void delay_us(unsigned int us)
{
    while (us--);
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 发出指定频率的音调
 */
void play_tone(unsigned int freq)
{
    unsigned int period_us, i;
    if (freq == 0) return;

    period_us = 1000000UL / freq / 2;
    for (i = 0; i < 100; i++)  // 短暂发声
    {
        BUZZER = 0;
        delay_us(period_us);
        BUZZER = 1;
        delay_us(period_us);
    }
}

/*
 * 扫描按键，返回按键编号（0-7），无按键返回8
 */
unsigned char key_scan(void)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        if (!(P1 & (0x01 << i)))
        {
            delay_ms(10);  // 消抖
            if (!(P1 & (0x01 << i)))
                return i;
        }
    }
    return 8;  // 无按键
}

void main(void)
{
    unsigned char key;

    while (1)
    {
        key = key_scan();
        if (key < 8)
        {
            play_tone(TONE_TABLE[key]);
        }
        delay_ms(10);
    }
}
