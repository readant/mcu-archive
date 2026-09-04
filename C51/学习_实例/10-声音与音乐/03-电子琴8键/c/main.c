/*------------------------------------------------
  功能：电子琴（8键）
  硬件：P2.0~P2.7接8个按键，P1.5接无源蜂鸣器
  说明：每个按键对应一个音符
------------------------------------------------*/
#include <reg51.h>

sbit BEEP = P1^5;

// 音符半周期值
unsigned int code tone_tab[] = {
    382, 340, 303, 285, 254, 226, 202, 191
};

// 按键对应的音符索引
unsigned char code key_tone[] = {0, 1, 2, 3, 4, 5, 6, 7};

void DelayMs(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

void PlayTone(unsigned char tone)
{
    unsigned int i;
    unsigned int half_period = tone_tab[tone];

    for (i = 0; i < 200; i++)
    {
        BEEP = 0;
        {
            unsigned int j = half_period;
            while (j--);
        }
        BEEP = 1;
        {
            unsigned int j = half_period;
            while (j--);
        }
    }
}

unsigned char KeyScan(void)
{
    unsigned char i;
    unsigned char temp = P2;

    for (i = 0; i < 8; i++)
    {
        if (!(temp & (0x01 << i)))
        {
            return i;
        }
    }
    return 0xFF;
}

void main(void)
{
    unsigned char key;

    while (1)
    {
        key = KeyScan();
        if (key != 0xFF)
        {
            PlayTone(key_tone[key]);
        }
        DelayMs(10);
    }
}
