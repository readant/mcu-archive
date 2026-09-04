#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int

// 位选控制引脚 P2.2 P2.3 P2.4
sbit BIT0 = P2^2;
sbit BIT1 = P2^3;
sbit BIT2 = P2^4;

// 共阴数码管段码表 (a对应P0.0, dp对应P0.7)
uchar code SegCode[8] = {
    0x30,  // I
    0x38,  // L
    0x3F,  // 0
    0x3E,  // U
    0x79,  // E
    0x76,//0x39,  // C
    0x6e,  // 0x6D,  // 5
    0x6e    //0x06   // 1
};

// 0x76 H
// 0x6e Y
uchar display_index = 0; // 当前扫描位

void Timer0_Init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x01; // 定时器0模式1
    TH0 = 0xF8;   // 2ms定时 (12MHz晶振)
    TL0 = 0x30;
    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xF8;
    TL0 = 0x30;

    // 1. 关闭所有位选 (让138译码器输出高电平，或直接关断)
    // 对于74HC138，置位P2.2-P2.4为1，Y输出高电平，共阴管关断
    P2 |= 0x1C; // 0001 1100，将P2.2,P2.3,P2.4置1
    
    // 2. 发送段码
    P0 = SegCode[7-display_index];

    // 3. 选中当前位
    // 将索引值写入P2.2-P2.4
    // 清除P2.2-P2.4
    P2 &= 0xE3; // 1110 0011
    // 设置新值 (index 0~7)
    P2 |= (display_index << 2);

    // 4. 索引递增
    display_index++;
    if(display_index >= 8) display_index = 0;
}

void main(void)
{
    // 若P0口无上拉电阻，此处需外部上拉或配置强推挽模式(若支持)
    // STC89C52 P0为开漏，建议外部上拉。
    P0 = 0x00;
    P2 = 0x00;
    
    Timer0_Init();
    
    while(1)
    {
        // 主循环空转，或添加其他功能
    }
}