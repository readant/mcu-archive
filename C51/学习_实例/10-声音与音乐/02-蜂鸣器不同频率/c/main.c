/*------------------------------------------------
  功能：蜂鸣器不同频率发声
  硬件：无源蜂鸣器接P1.5
  说明：无源蜂鸣器需要给不同频率的方波
        频率越高音调越高
------------------------------------------------*/
#include <reg51.h>

sbit BEEP = P1^5;

// 音符频率对应的半周期（us）
// 1MHz定时器计数值
unsigned int code tone_tab[] = {
    382,   // C4: 262Hz
    340,   // D4: 294Hz
    303,   // E4: 330Hz
    285,   // F4: 349Hz
    254,   // G4: 392Hz
    226,   // A4: 440Hz
    202,   // B4: 494Hz
    191    // C5: 523Hz
};

void DelayMs(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

// 发出指定频率的声音
void BeepTone(unsigned char tone, unsigned int duration)
{
    unsigned int i;
    unsigned int half_period = tone_tab[tone];

    for (i = 0; i < duration * 100; i++)
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

void main(void)
{
    while (1)
    {
        BeepTone(0, 200);  // C4
        DelayMs(100);
        BeepTone(2, 200);  // E4
        DelayMs(100);
        BeepTone(4, 200);  // G4
        DelayMs(100);
        BeepTone(7, 400);  // C5
        DelayMs(500);
    }
}
