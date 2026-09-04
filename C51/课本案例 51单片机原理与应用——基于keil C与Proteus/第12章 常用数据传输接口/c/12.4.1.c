/************************ 必要的变量定义 ************************/
#include <reg51.h>
#define uchar unsigned char
#define uint unsigned int

uchar code table[] = 
{0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,0x7a,0x9e,0x8e};
sbit sda = P3^1;
sbit scl = P3^0;
bit flag = 0;               // 用户自定义定时溢出标志位
uchar sec, tcnt;            // 秒变量sec，定时中断计数变量tcnt

/************************ 短延时子程序 ************************/
void delay()                // 两个机器周期，做总线的延时用
{
    ; ;
}

/************************ 长延时子程序 ************************/
void delayms(uint m)        // 做数码管显示延时用
{
    uint x, y;
    for(x = m; x > 0; x--)
        for(y = 110; y > 0; y--);
}

/************************ 开始信号子程序 ************************/
void start()
{
    sda = 1;
    delay();
    scl = 1;
    delay();
    sda = 0;
    delay();
    scl = 0;
    delay();
}

/************************ 停止信号子程序 ************************/
void stop()
{
    sda = 0;
    delay();
    scl = 1;
    delay();
    sda = 1;
    delay();
}

/************************ 应答信号子程序 ************************/
void respons()
{
    uchar i;
    scl = 1;
    delay();
    while((sda == 1) && (i < 250))
        i++;
    scl = 0;
    delay();
}

/************************ 写数据子程序 ************************/
void write_byte(uchar date)
{
    uchar i, temp;
    temp = date;
    for(i = 0; i < 8; i++)
    {
        temp = temp << 1;
        scl = 0;
        delay();
        sda = CY;
        delay();
        scl = 1;
        delay();
    }
    scl = 0; delay();
    sda = 1; delay();
}

/************************ 读数据子程序 ************************/
uchar read_byte()
{
    uchar i, k;
    scl = 0;
    delay();
    sda = 1;
    delay();
    for(i = 0; i < 8; i++)
    {
        scl = 1;
        delay();
        k = (k << 1) | sda;
        scl = 0;
        delay();
    }
    return k;
}

/************************ 写地址子程序 ************************/
void write_add(uchar address, uchar date)
{
    start();
    write_byte(0xa0);
    respons();
    write_byte(address);
    respons();
    write_byte(date);
    respons();
    stop();
}

/************************ 读地址子程序 ************************/
uchar read_add(uchar address)
{
    uchar byte;
    start();
    write_byte(0xa0);
    respons();
    write_byte(address);
    respons();
    start();
    write_byte(0xa1);
    respons();
    byte = read_byte();
    stop();
    return byte;
}

/************************ 数码管显示子程序 ************************/
void Disp()
{
    P2 = 0xfe;
    P1 = table[sec % 10];
    delayms(5);
    P2 = 0xfd;
    P1 = table[sec / 10 % 10];
    delayms(5);
    P2 = 0xfb;
    P1 = table[sec / 100];
    delayms(5);
}

/************************ 初始化子程序 ************************/
void init()
{
    sda = 1;
    delay();
    scl = 1;
    delay();
}

/************************ 主程序 ************************/
void main()
{
    init();
    sec = 0;
    TMOD = 0x01;
    ET0 = 1;
    EA = 1;
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;
    TR0 = 1;
    while(1)
    {
        Disp();
        if(flag == 1)
        {
            flag = 0;
            write_add(2, sec);
        }
    }
}

/************************ 定时中断0子程序 ************************/
void time() interrupt 1
{
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;
    tcnt++;
    if(tcnt == 20)
    {
        tcnt = 0;           // tcnt每到达一次20，tcnt重新计时，将时间控制在1s
        sec++;
        flag = 1;
        if(sec == 60)       // sec控制在59
            sec = 0;
    }
}