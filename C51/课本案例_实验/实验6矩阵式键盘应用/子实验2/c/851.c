/**************** 必要的变量定义 ****************/
#include <reg51.h>
#define uint unsigned int
#define uchar unsigned char

// 按键键值表 (对应 4x4 键盘 0~F 的扫描键值)
uchar code KEY_TABLE[] = {
    0xee, 0xde, 0xbe, 0x7e,
    0xed, 0xdd, 0xbd, 0x7d,
    0xeb, 0xdb, 0xbb, 0x7b,
    0xe7, 0xd7, 0xb7, 0x77
};

// 共阴极数码管编码表 (段码，对应显示 0~F)
uchar code TABLE[] = {
    0x3F, 0x06, 0x5B, 0x4F,
    0x66, 0x6D, 0x7D, 0x07,
    0x7F, 0x6F, 0x77, 0x7C,
    0x39, 0x5E, 0x79, 0x71
};

/**************** 延时子程序 ****************/
// 长延时：控制数码管显示刷新
void delay1()
{
    uint n = 50000;
    while(n--);
}

// 短延时：软件消抖
void delays()
{
    uint n = 10000;
    while(n--);
}

/**************** 主程序 ****************/
void main()
{
    uchar temp, key, num, i;
    while(1)
    {
        P3 = 0xf0;                 // 置行为0，列为1，准备读取列值
        if(P3 != 0xf0)              // 判断有无键盘被按下
        {
            delays();               // 软件消抖
            if(P3 != 0xf0)          // 二次确认，确认为有效按键
            {
                temp = P3;          // 储存列值
                P3 = 0x0f;          // 置列为0，行为1，准备读取行值
                key = temp | P3;    // 组合行、列值，得到唯一按键编码
                
                // 遍历键值表，匹配当前按键
                for(i = 0; i < 16; i++)
                {
                    if(key == KEY_TABLE[i])
                    {
                        num = i;    // 得到按键对应的数字 0~F
                        break;
                    }
                }
                
                P2 = TABLE[num];    // 输出对应段码，点亮数码管显示
                delay1();           // 延时保持显示
            }
        }
    }
}