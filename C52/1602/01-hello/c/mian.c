#include <reg52.h>
#include <intrins.h>  // 用于_nop_()空操作函数

// -------------------- 硬件引脚定义 --------------------
#define LCD_DATA P0      // P0口作为数据总线（D0-D7）

sbit LCD_RW = P2^5;      // 读写控制：0=写入，1=读取
sbit LCD_RE = P2^6;      // 寄存器选择（标准名称为RS）：0=指令，1=数据
sbit LCD_EN = P2^7;      // 使能信号，高脉冲有效

// -------------------- 延时函数（12MHz晶振） --------------------
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

// -------------------- 检测忙信号（重要：确保指令执行完成） --------------------
void LcdBusy(void) {
    unsigned char busy_flag;
    LCD_DATA = 0xFF;      // 读取前先拉高P0口（P0为准双向口，需先写1）
    LCD_RE = 0;           // 选择指令寄存器
    LCD_RW = 1;           // 切换为读模式
    do {
        LCD_EN = 1;       // 使能高电平
        _nop_();          // 短暂延时，等待电平稳定
        busy_flag = LCD_DATA & 0x80; // 读取最高位（DB7），0=空闲，1=忙
        LCD_EN = 0;       // 使能低电平
    } while (busy_flag);  // 一直等待直到忙标志位清零
}

// -------------------- 写入指令（RS=0） --------------------
void LcdWriteCmd(unsigned char cmd) {
    LcdBusy();            // 每次写入前检测忙状态
    LCD_RE = 0;           // 指令模式
    LCD_RW = 0;           // 写入模式
    LCD_DATA = cmd;       // 放置指令数据
    LCD_EN = 1;           // 产生高脉冲（EN高）
    _nop_();              // 使能高电平保持时间
    LCD_EN = 0;           // EN拉低，锁存数据
}

// -------------------- 写入数据（RS=1） --------------------
void LcdWriteData(unsigned char dat) {
    LcdBusy();            // 每次写入前检测忙状态
    LCD_RE = 1;           // 数据模式
    LCD_RW = 0;           // 写入模式
    LCD_DATA = dat;       // 放置显示数据
    LCD_EN = 1;           // 产生高脉冲
    _nop_();
    LCD_EN = 0;
}

// -------------------- 1602 LCD 初始化 --------------------
void LcdInit(void) {
    delay_ms(15);         // 上电延时，等待LCD电源稳定（>15ms）
    
    // 标准8位初始化流程（由于我们使用8位总线，不需要切换4位模式）
    LcdWriteCmd(0x38);    // 8位数据接口，2行显示，5x7点阵
    LcdWriteCmd(0x0C);    // 显示开，光标关，闪烁关
    LcdWriteCmd(0x06);    // 写入新数据后光标右移，显示不移动
    LcdWriteCmd(0x01);    // 清屏指令
    delay_ms(2);          // 清屏需要一定时间
}

// -------------------- 指定位置显示字符串（x=0-15列，y=0-1行） --------------------
void LcdShowString(unsigned char x, unsigned char y, unsigned char *str) {
    unsigned char addr;
    
    // 计算DDRAM地址（第一行起始0x80，第二行起始0xC0）
    if (y == 0) {
        addr = 0x80 + x;  // 第一行：0x80 + 列数
    } else {
        addr = 0xC0 + x;  // 第二行：0xC0 + 列数
    }
    
    LcdWriteCmd(addr);    // 设置显示地址
    
    while (*str != '\0') { // 循环发送字符串内容直到遇到结束符
        LcdWriteData(*str);
        str++;
    }
}

// -------------------- 主函数 --------------------
void main(void) {
    // 初始化LCD
    LcdInit();
    
    // 在第一行第0列显示信息
    LcdShowString(0, 0, "Hello 1602!");
    
    // 在第二行第0列显示信息（注意修改字符内容以验证效果）
    LcdShowString(0, 1, "STC89C52 OK!");
    
    // 程序停在这里，保持显示（你也可以加入主循环做其他事情）
    while (1);
}