/*
 * 停车场计数系统 - AT89C51
 * 12MHz晶振，两位共阴数码管显示（0~30），红外传感器模拟按键输入
 */

#include <reg51.h>

#define MAX_PARKING  30      // 最大车位
#define DEBOUNCE_MS  20      // 消抖延时

/* 端口定义 */
sbit DIG_TEN   = P2^0;       // 数码管十位位选
sbit DIG_ONE   = P2^1;       // 数码管个位位选
sbit LED_RED   = P1^0;       // 红灯：满位报警
sbit LED_GREEN = P1^1;       // 绿灯：正常状态

/* 全局变量 */
unsigned char g_carCount = 0;    // 当前车辆计数

/* 消抖标志位：0-已消抖可处理，1-等待消抖结束 */
static bit key0_debounce = 0;
static bit key1_debounce = 0;

/* 共阴数码管段码表 */
unsigned char code segCode[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

/* 函数声明 */
void delay_ms(unsigned int ms);
void display_number(unsigned char num);
void led_status_update(void);
void sys_init(void);

/* 延时函数（12MHz晶振） */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

/* 数码管动态显示 */
void display_number(unsigned char num)
{
    unsigned char ten = num / 10;
    unsigned char one = num % 10;

    P0 = 0x00;
    DIG_ONE = 1;
    DIG_TEN = 0;
    P0 = segCode[ten];
    delay_ms(5);

    P0 = 0x00;
    DIG_TEN = 1;
    DIG_ONE = 0;
    P0 = segCode[one];
    delay_ms(5);
}

/* LED状态更新 */
void led_status_update(void)
{
    if (g_carCount >= MAX_PARKING) {
        LED_RED = 0;
        LED_GREEN = 1;
    } else {
        LED_RED = 1;
        LED_GREEN = 0;
    }
}

/* 系统初始化 */
void sys_init(void)
{
    P0 = 0x00;
    P1 = 0xFF;
    P2 = 0x00;

    IT0 = 1;     // 下降沿触发
    IT1 = 1;
    EX0 = 1;
    EX1 = 1;
    EA = 1;

    g_carCount = 0;
    led_status_update();
}

/* 外部中断0 - 车辆驶入 */
void ext0_isr(void) interrupt 0
{
    if (key0_debounce) return;       // 消抖中，跳过
    if (g_carCount < MAX_PARKING) {
        g_carCount++;
        led_status_update();
    }
    key0_debounce = 1;              // 启动消抖
}

/* 外部中断1 - 车辆驶出 */
void ext1_isr(void) interrupt 2
{
    if (key1_debounce) return;
    if (g_carCount > 0) {
        g_carCount--;
        led_status_update();
    }
    key1_debounce = 1;
}

/* 消抖定时器（定时器0中断） */
void timer0_isr(void) interrupt 1
{
    static unsigned int count0 = 0;
    static unsigned int count1 = 0;

    if (key0_debounce) {
        if (++count0 >= DEBOUNCE_MS) {
            count0 = 0;
            key0_debounce = 0;
        }
    }

    if (key1_debounce) {
        if (++count1 >= DEBOUNCE_MS) {
            count1 = 0;
            key1_debounce = 0;
        }
    }
}

/* 主函数 */
void main(void)
{
    sys_init();

    // 配置定时器0用于消抖（模式1，12MHz下约1ms中断一次）
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = 0xFC;
    TL0 = 0x18;
    ET0 = 1;
    TR0 = 1;

    while (1) {
        display_number(g_carCount);
    }
}
