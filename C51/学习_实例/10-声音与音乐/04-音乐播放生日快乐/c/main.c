/*------------------------------------------------
  功能：音乐播放（生日快乐）
  硬件：无源蜂鸣器接P1.5
  说明：自动播放生日快乐歌
------------------------------------------------*/
#include <reg51.h>

sbit BEEP = P1^5;

// 音符频率（半周期us）
#define T1  382  // C4
#define T2  340  // D4
#define T3  303  // E4
#define T4  285  // F4
#define T5  254  // G4
#define T6  226  // A4
#define T7  202  // B4
#define T8  191  // C5

// 生日快乐歌谱（音符，节拍）
unsigned char code song_notes[] = {
    T5, T5, T6, T5, T8, T7,   // 祝你生日快乐
    T5, T5, T6, T5, T8, T7,   // 祝你生日快乐
    T5, T5, T8, T6, T3, T1, T2,  // 祝你生日快乐
    T4, T4, T3, T1, T5, T3    // 祝你生日快乐
};

// 节拍（每个音符持续时间）
unsigned char code song_beats[] = {
    4, 4, 8, 8, 8, 8,
    4, 4, 8, 8, 8, 8,
    4, 4, 8, 8, 8, 8, 8,
    4, 4, 8, 8, 8, 8
};

#define SONG_LEN sizeof(song_notes)

void DelayMs(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

void PlayNote(unsigned char freq, unsigned char beat)
{
    unsigned int i;
    unsigned int half_period;

    if (freq == 0)
    {
        DelayMs(beat * 100);
        return;
    }

    half_period = freq;

    for (i = 0; i < beat * 50; i++)
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

void PlaySong(void)
{
    unsigned char i;
    for (i = 0; i < SONG_LEN; i++)
    {
        PlayNote(song_notes[i], song_beats[i]);
        DelayMs(50);  // 音符间隔
    }
}

void main(void)
{
    while (1)
    {
        PlaySong();
        DelayMs(2000);
    }
}
