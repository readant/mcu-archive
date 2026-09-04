#include <reg51.h>

sbit BUZZER = P1^5;                    // 定义蜂鸣器控制引脚（根据实际电路修改）
#define SYSTEM_OSC 11059200            // 系统晶振频率，假设为11.0592MHz

// 预定义音符频率对应的定时器重载值
unsigned int code tone[] = {
    0, 0xFC0E, 0xFC4B, 0xFC9B, 0xFCE8, 0xFD31, 0xFD76, 0xFDAE,  // 低音 1-7
    0xFE0E, 0xFE4B, 0xFE9B, 0xFEE8, 0xFF31, 0xFF76, 0xFFAE,  // 中音 1-7
    0x001E, 0x004B, 0x009B, 0x00E8, 0x0131, 0x0176, 0x01AE   // 高音 1-7
};
// 《两只老虎》的乐谱数据（音符，节拍）
unsigned char code music[] = {
    1,2, 2,2, 3,2, 1,2,   // 1 2 3 1
    3,2, 4,2, 5,4,        // 3 4 5
    5,2, 6,2, 5,2, 4,2,   // 5 6 5 4
    3,2, 1,2, 2,2, 5,1,   // 3 1 2 5
    1,2, 2,2, 3,2, 1,2,   // 1 2 3 1
    3,2, 4,2, 5,4,        // 3 4 5
    5,2, 6,2, 5,2, 4,2,   // 5 6 5 4
    3,2, 1,2, 0,4         // 3 1
};

unsigned char music_index; // 全局音乐索引
unsigned int reload_val;   // 定时器重载值

// 定时器中断服务函数
void timer0_isr() interrupt 1 {
    // 重置定时器初值
    TH0 = reload_val >> 8;
    TL0 = reload_val & 0xFF;
    BUZZER = !BUZZER; // 翻转蜂鸣器引脚，产生方波
}

// 延时函数，用于控制节拍
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i=0; i<ms; i++)
        for(j=0; j<100; j++);
}

void main() {
    unsigned char note, beat;
    // 设置定时器为模式1
    TMOD = 0x01;
    // 开启定时器0中断和总中断
    ET0 = 1;
    EA = 1;
    music_index = 0;
    while(1) {
        // 从乐谱数组中获取一个音符的数据（每两个字节一组）
        note = music[music_index];   
        beat = music[music_index + 1];
        // 索引后移两位
        music_index += 2; 
        // 如果note是0（休止符），就关闭定时器并延时
        if(note == 0) {
            TR0 = 0;
            BUZZER = 1; // 确保蜂鸣器关闭
        } else {
            // 从频率表中获取重载值
            reload_val = tone[note];
            // 设置定时器初值
            TH0 = reload_val >> 8;
            TL0 = reload_val & 0xFF;
            // 启动定时器，开始产生方波
            TR0 = 1;
        }
        // 根据节拍值延时，这里假设单位节拍是400毫秒
        delay_ms(beat * 400);
        // 音符播放结束后，停止定时器，关闭蜂鸣器
        TR0 = 0;
        BUZZER = 1;
        // 在两个音符之间添加短暂停顿
        delay_ms(5);
        // 如果播放完所有音符，重置索引以循环播放
        if(music_index >= sizeof(music)) {
            music_index = 0;
        }
    }
}