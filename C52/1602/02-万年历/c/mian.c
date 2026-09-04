#include <reg52.h>

//====================引脚定义====================
#define LCD_DataPort P0
sbit LCD_RW = P2^5;
sbit LCD_RS = P2^6;
sbit LCD_EN = P2^7;

sbit DS1302_IO   = P3^4;
sbit DS1302_RST  = P3^5;
sbit DS1302_SCLK = P3^6;

sbit KEY_SET    = P3^0;   // K1 进入/退出设置并保存
sbit KEY_SHIFT  = P3^1;   // K2 移位切换选项
sbit KEY_INC    = P3^2;   // K3 加
sbit KEY_DEC    = P3^3;   // K4 减

sbit DS18B20_DQ = P3^7;   // DS18B20 数据线

//DS1302寄存器地址
#define DS1302_SEC_WR   0x80
#define DS1302_MIN_WR   0x82
#define DS1302_HOUR_WR  0x84
#define DS1302_DAY_WR   0x86
#define DS1302_MON_WR   0x88
#define DS1302_WEEK_WR  0x8A
#define DS1302_YEAR_WR  0x8C
#define DS1302_WP       0x8E

#define DS1302_SEC_RD   0x81
#define DS1302_MIN_RD   0x83
#define DS1302_HOUR_RD  0x85
#define DS1302_DAY_RD   0x87
#define DS1302_MON_RD   0x89
#define DS1302_YEAR_RD  0x8D

//DS18B20 命令
#define DS18B20_SKIP_ROM     0xCC
#define DS18B20_CONVERT_T    0x44
#define DS18B20_READ_SCRATCH 0xBE

//====================默认初始时间（2026-08-18 00:00:00）====================
#define DEFAULT_YEAR  26    // 后两位年份
#define DEFAULT_MONTH 8
#define DEFAULT_DAY   18
#define DEFAULT_HOUR  0
#define DEFAULT_MIN   0
#define DEFAULT_SEC   0

//全局时间数组：0秒 1分 2时 3日 4月 5年(后两位)
unsigned char time_buf[6] = {DEFAULT_SEC, DEFAULT_MIN, DEFAULT_HOUR, DEFAULT_DAY, DEFAULT_MONTH, DEFAULT_YEAR};
unsigned char set_mode = 0;   //0正常显示，1设置模式
unsigned char set_sel  = 0;   //选中项：0秒 1分 2时 3日 4月 5年

//温度变量（放大10倍，例如255表示25.5℃）
int temperature = 0;

//DS18B20 状态机变量
unsigned char ds18b20_state = 0; // 0=空闲，1=转换中，2=转换完成待读取
unsigned int  ds18b20_wait  = 0;
unsigned int  ds18b20_timer = 0; // 定时触发转换

//光标位置表：与显示位置对应（秒、分、时、日、月、年）
unsigned char code pos_table[] = {6, 3, 0, 8, 5, 2};

//====================延时函数====================
void DelayMs(unsigned int ms)
{
    unsigned int i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

//10微秒延时（用于DS18B20，12MHz晶振）
void Delay10us(unsigned char us)
{
    unsigned char i;
    while(us--)
    {
        for(i = 0; i < 10; i++);
    }
}

//====================LCD1602驱动====================
void LCD1602_WriteCmd(unsigned char cmd)
{
    LCD_EN = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DataPort = cmd;
    DelayMs(1);
    LCD_EN = 1;
    DelayMs(1);
    LCD_EN = 0;
}

void LCD1602_WriteData(unsigned char dat)
{
    LCD_EN = 0;
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DataPort = dat;
    DelayMs(1);
    LCD_EN = 1;
    DelayMs(1);
    LCD_EN = 0;
}

void LCD1602_Init(void)
{
    LCD_EN = 0;
    LCD1602_WriteCmd(0x38);
    DelayMs(5);
    LCD1602_WriteCmd(0x0C);
    LCD1602_WriteCmd(0x06);
    LCD1602_WriteCmd(0x01);
    DelayMs(10);
}

void LCD1602_SetPos(unsigned char line, unsigned char col)
{
    if(line == 1)
        LCD1602_WriteCmd(0x80 + col);
    else
        LCD1602_WriteCmd(0x80 + 0x40 + col);
}

//在指定行、列显示2位数字
void LCD_ShowNum2(unsigned char line, unsigned char col, unsigned char num)
{
    LCD1602_SetPos(line, col);
    LCD1602_WriteData(num / 10 + '0');
    LCD1602_WriteData(num % 10 + '0');
}

//====================DS1302驱动====================
unsigned char BCD_TO_DEC(unsigned char bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

unsigned char DEC_TO_BCD(unsigned char dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

void DS1302_WriteByte(unsigned char dat)
{
    unsigned char i;
    for(i = 0; i < 8; i++)
    {
        DS1302_SCLK = 0;
        DS1302_IO = dat & 0x01;
        dat >>= 1;
        DS1302_SCLK = 1;
    }
}

unsigned char DS1302_ReadByte(void)
{
    unsigned char i, dat = 0;
    for(i = 0; i < 8; i++)
    {
        DS1302_SCLK = 0;
        dat >>= 1;
        if(DS1302_IO) dat |= 0x80;
        DS1302_SCLK = 1;
    }
    return dat;
}

void DS1302_Write(unsigned char addr, unsigned char dat)
{
    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_RST = 1;
    DS1302_WriteByte(addr);
    DS1302_WriteByte(dat);
    DS1302_RST = 0;
}

unsigned char DS1302_Read(unsigned char addr)
{
    unsigned char dat;
    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_RST = 1;
    DS1302_WriteByte(addr);
    dat = DS1302_ReadByte();
    DS1302_RST = 0;
    return dat;
}

//DS1302突发读取：从0xBF开始连续读8个寄存器
void DS1302_ReadBurst(unsigned char *buf)
{
    unsigned char i;
    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_RST = 1;
    DS1302_WriteByte(0xBF);  // 突发读命令
    for(i = 0; i < 8; i++)
    {
        buf[i] = DS1302_ReadByte();
    }
    DS1302_RST = 0;
}

//====================星期与天数计算====================
//蔡勒公式：返回0~6，0为星期日
unsigned char GetWeekday(unsigned char year, unsigned char month, unsigned char day)
{
    unsigned int y = 2000 + year;
    unsigned char m = month;
    if(m <= 2)
    {
        m += 12;
        y -= 1;
    }
    return (day + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400 + 1) % 7;
}

//计算某月最大天数，支持闰年
unsigned char GetMaxDay(unsigned char month, unsigned char year)
{
    unsigned int full_year = 2000 + year;
    if(month == 2)
    {
        if((full_year % 4 == 0 && full_year % 100 != 0) || (full_year % 400 == 0))
            return 29;
        else
            return 28;
    }
    if(month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    return 31;
}

//====================DS1302时间读写====================
void DS1302_ReadTime(void)
{
    unsigned char buf1[8], buf2[8];
    unsigned char i, retry;

    for(retry = 0; retry < 5; retry++)
    {
        DS1302_ReadBurst(buf1);
        DS1302_ReadBurst(buf2);

        // 两次读取完全一致才认为数据有效，否则重试
        for(i = 0; i < 8; i++)
        {
            if(buf1[i] != buf2[i])
                break;
        }
        if(i == 8)  // 全部相等，跳出重试
            break;
    }

    // 突发读取寄存器顺序：秒、分、时、日、月、星期、年、写保护
    time_buf[0] = BCD_TO_DEC(buf1[0] & 0x7F); // 秒，去掉CH位
    time_buf[1] = BCD_TO_DEC(buf1[1]);        // 分
    time_buf[2] = BCD_TO_DEC(buf1[2]);        // 时
    time_buf[3] = BCD_TO_DEC(buf1[3]);        // 日
    time_buf[4] = BCD_TO_DEC(buf1[4]);        // 月
    time_buf[5] = BCD_TO_DEC(buf1[6]);        // 年（索引6是年，5是星期）
}

void DS1302_SaveTime(void)
{
    unsigned char sec_bcd;
    unsigned char wd;

    DS1302_Write(DS1302_WP, 0x00); //关闭写保护

    sec_bcd = DEC_TO_BCD(time_buf[0]);
    sec_bcd &= 0x7F;               //清除CH位，启动时钟
    DS1302_Write(DS1302_SEC_WR, sec_bcd);

    DS1302_Write(DS1302_MIN_WR,  DEC_TO_BCD(time_buf[1]));
    DS1302_Write(DS1302_HOUR_WR, DEC_TO_BCD(time_buf[2]));
    DS1302_Write(DS1302_DAY_WR,  DEC_TO_BCD(time_buf[3]));
    DS1302_Write(DS1302_MON_WR,  DEC_TO_BCD(time_buf[4]));

    wd = GetWeekday(time_buf[5], time_buf[4], time_buf[3]) + 1; //DS1302星期寄存器：1=周日，2=周一...
    DS1302_Write(DS1302_WEEK_WR, DEC_TO_BCD(wd));

    DS1302_Write(DS1302_YEAR_WR, DEC_TO_BCD(time_buf[5]));

    DS1302_Write(DS1302_WP, 0x80); //打开写保护
}

//上电检查DS1302是否启动，若CH=1则载入默认时间
void RTC_InitCheck(void)
{
    if(DS1302_Read(DS1302_SEC_RD) & 0x80) //时钟停止
    {
        time_buf[0] = DEFAULT_SEC;
        time_buf[1] = DEFAULT_MIN;
        time_buf[2] = DEFAULT_HOUR;
        time_buf[3] = DEFAULT_DAY;
        time_buf[4] = DEFAULT_MONTH;
        time_buf[5] = DEFAULT_YEAR;
        DS1302_SaveTime();
    }
    else
    {
        DS1302_ReadTime();
    }
}

//====================DS18B20温度传感器驱动====================
//初始化DS18B20，返回存在脉冲（0=存在，1=不存在）
bit DS18B20_Init(void)
{
    bit ack;
    DS18B20_DQ = 1;
    Delay10us(2);
    DS18B20_DQ = 0;
    Delay10us(48);      // 拉低480us以上
    DS18B20_DQ = 1;
    Delay10us(6);       // 等待15-60us
    ack = DS18B20_DQ;   // 读取存在脉冲
    Delay10us(20);      // 等待剩余时间
    return ack;
}

//写一个字节
void DS18B20_WriteByte(unsigned char dat)
{
    unsigned char i;
    for(i = 0; i < 8; i++)
    {
        DS18B20_DQ = 0;
        DS18B20_DQ = dat & 0x01;
        Delay10us(4);   // 写时序总时间约40us
        DS18B20_DQ = 1;
        dat >>= 1;
    }
}

//读一个字节
unsigned char DS18B20_ReadByte(void)
{
    unsigned char i, dat = 0;
    for(i = 0; i < 8; i++)
    {
        dat >>= 1;
        DS18B20_DQ = 0;
        Delay10us(1);   // 拉低1us
        DS18B20_DQ = 1;
        Delay10us(1);   // 释放，等待采样
        if(DS18B20_DQ)
            dat |= 0x80;
        Delay10us(4);   // 读时序剩余时间
    }
    return dat;
}

//启动温度转换
void DS18B20_StartConvert(void)
{
    DS18B20_Init();
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_CONVERT_T);
}

//读取温度原始值并计算放大10倍的温度
int DS18B20_ReadTemp(void)
{
    unsigned char temp_low, temp_high;
    int temp_raw, temp;

    DS18B20_Init();
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_READ_SCRATCH);

    temp_low = DS18B20_ReadByte();
    temp_high = DS18B20_ReadByte();

    temp_raw = (temp_high << 8) | temp_low;

    if(temp_raw & 0x8000) // 负温度
    {
        temp_raw = (~temp_raw) + 1;
        temp = -(temp_raw * 10 / 16);
    }
    else
    {
        temp = temp_raw * 10 / 16;
    }
    return temp;
}

//DS18B20状态机处理（在主循环中调用）
void DS18B20_Process(void)
{
    switch(ds18b20_state)
    {
        case 0: // 空闲状态，定时启动转换
            ds18b20_timer++;
            if(ds18b20_timer >= 100) // 约5秒（100*50ms）采集一次
            {
                ds18b20_timer = 0;
                DS18B20_StartConvert();
                ds18b20_state = 1;   // 进入转换中状态
                ds18b20_wait  = 15;  // 等待15次主循环（约750ms）
            }
            break;

        case 1: // 转换中，等待转换完成
            if(ds18b20_wait > 0)
                ds18b20_wait--;
            if(ds18b20_wait == 0)
                ds18b20_state = 2;   // 转换完成，准备读取
            break;

        case 2: // 读取温度
            temperature = DS18B20_ReadTemp();
            ds18b20_state = 0;       // 回到空闲
            break;
    }
}

//====================非阻塞按键扫描====================
//返回1~4表示对应按键按下一次，返回0表示无按键
unsigned char KeyScan(void)
{
    static unsigned char key_state = 0;
    unsigned char key_now = 0;

    if(KEY_SET == 0)       key_now = 1;
    else if(KEY_SHIFT == 0) key_now = 2;
    else if(KEY_INC == 0)   key_now = 3;
    else if(KEY_DEC == 0)   key_now = 4;

    if(key_now != 0)
    {
        if(key_state == 0)
        {
            key_state = 1;      //锁定，直到释放才能再次触发
            DelayMs(10);        //消抖
            return key_now;
        }
    }
    else
    {
        key_state = 0;          //按键已释放
    }
    return 0;
}

//====================按键处理与时间调整====================
void TimeAdjust(signed char dir)
{
    unsigned char maxd;

    switch(set_sel)
    {
        case 0: //秒
            time_buf[0] = (time_buf[0] + dir + 60) % 60;
            break;
        case 1: //分
            time_buf[1] = (time_buf[1] + dir + 60) % 60;
            break;
        case 2: //时
            time_buf[2] = (time_buf[2] + dir + 24) % 24;
            break;
        case 3: //日
            maxd = GetMaxDay(time_buf[4], time_buf[5]);
            if(dir == 1)
            {
                time_buf[3]++;
                if(time_buf[3] > maxd) time_buf[3] = 1;
            }
            else
            {
                if(time_buf[3] == 1) time_buf[3] = maxd;
                else time_buf[3]--;
            }
            break;
        case 4: //月
            if(dir == 1)
            {
                time_buf[4]++;
                if(time_buf[4] > 12) time_buf[4] = 1;
            }
            else
            {
                if(time_buf[4] == 1) time_buf[4] = 12;
                else time_buf[4]--;
            }
            //月改变后，修正日
            maxd = GetMaxDay(time_buf[4], time_buf[5]);
            if(time_buf[3] > maxd) time_buf[3] = maxd;
            break;
        case 5: //年
            if(dir == 1)
            {
                time_buf[5]++;
                if(time_buf[5] > 99) time_buf[5] = 0;
            }
            else
            {
                if(time_buf[5] == 0) time_buf[5] = 99;
                else time_buf[5]--;
            }
            //年改变后，修正日（影响闰年2月）
            maxd = GetMaxDay(time_buf[4], time_buf[5]);
            if(time_buf[3] > maxd) time_buf[3] = maxd;
            break;
    }
}

void KeyProcess(unsigned char key)
{
    if(key == 1) //K1 进入/退出设置
    {
        set_mode = !set_mode;
        if(set_mode == 0)
        {
            DS1302_SaveTime();   //退出设置时保存时间
        }
        else
        {
            set_sel = 0;         //进入设置默认选中秒
        }
    }

    if(set_mode == 1) //仅设置模式下K2/K3/K4有效
    {
        if(key == 2) //K2 切换选项：秒-分-时-日-月-年循环
        {
            set_sel++;
            if(set_sel >= 6) set_sel = 0;
        }
        else if(key == 3) //K3 加
        {
            TimeAdjust(1);
        }
        else if(key == 4) //K4 减
        {
            TimeAdjust(-1);
        }
    }
}

//====================LCD刷新显示====================
void ShowCalendar(void)
{
    unsigned char weekday;

    //第一行：20YY-MM-DD 星期
    LCD1602_SetPos(1, 0);
    LCD1602_WriteData('2');
    LCD1602_WriteData('0');
    LCD_ShowNum2(1, 2, time_buf[5]);
    LCD1602_SetPos(1, 4);
    LCD1602_WriteData('-');
    LCD_ShowNum2(1, 5, time_buf[4]);
    LCD1602_SetPos(1, 7);
    LCD1602_WriteData('-');
    LCD_ShowNum2(1, 8, time_buf[3]);

    //星期显示
    weekday = GetWeekday(time_buf[5], time_buf[4], time_buf[3]);
    LCD1602_SetPos(1, 10);
    LCD1602_WriteData(' ');
    LCD1602_SetPos(1, 11);
    switch(weekday)
    {
        case 0: LCD1602_WriteData('S'); LCD1602_WriteData('u'); LCD1602_WriteData('n'); break;
        case 1: LCD1602_WriteData('M'); LCD1602_WriteData('o'); LCD1602_WriteData('n'); break;
        case 2: LCD1602_WriteData('T'); LCD1602_WriteData('u'); LCD1602_WriteData('e'); break;
        case 3: LCD1602_WriteData('W'); LCD1602_WriteData('e'); LCD1602_WriteData('d'); break;
        case 4: LCD1602_WriteData('T'); LCD1602_WriteData('h'); LCD1602_WriteData('u'); break;
        case 5: LCD1602_WriteData('F'); LCD1602_WriteData('r'); LCD1602_WriteData('i'); break;
        case 6: LCD1602_WriteData('S'); LCD1602_WriteData('a'); LCD1602_WriteData('t'); break;
    }

    //第二行：HH:MM:SS
    LCD_ShowNum2(2, 0, time_buf[2]);
    LCD1602_SetPos(2, 2);
    LCD1602_WriteData(':');
    LCD_ShowNum2(2, 3, time_buf[1]);
    LCD1602_SetPos(2, 5);
    LCD1602_WriteData(':');
    LCD_ShowNum2(2, 6, time_buf[0]);

    //第二行后半部分：正常模式显示温度，设置模式显示SET
    if(set_mode == 0)
    {
        // 显示温度，格式：xx.xC（例如 25.5C）
        LCD1602_SetPos(2, 9);
        if(temperature < 0)
        {
            int abs_temp = -temperature;
            LCD1602_WriteData('-');
            if(abs_temp / 100 > 0)
                LCD1602_WriteData(abs_temp / 100 + '0'); // 百位（理论不会超过）
            else
                LCD1602_WriteData(' ');
            LCD1602_WriteData((abs_temp / 10) % 10 + '0');
            LCD1602_WriteData('.');
            LCD1602_WriteData(abs_temp % 10 + '0');
            LCD1602_WriteData('C');
        }
        else
        {
            if(temperature / 100 > 0)
                LCD1602_WriteData(temperature / 100 + '0'); // 百位
            else
                LCD1602_WriteData(' ');
            LCD1602_WriteData((temperature / 10) % 10 + '0');
            LCD1602_WriteData('.');
            LCD1602_WriteData(temperature % 10 + '0');
            LCD1602_WriteData('C');
        }
        LCD1602_WriteData(' '); // 清除残留字符
    }
    else
    {
        // 设置模式显示SET
        LCD1602_SetPos(2, 9);
        LCD1602_WriteData('S');
        LCD1602_WriteData('E');
        LCD1602_WriteData('T');
        LCD1602_WriteData(' ');
        LCD1602_WriteData(' ');
        LCD1602_WriteData(' ');
    }

    //设置模式光标闪烁
    if(set_mode == 1)
    {
        LCD1602_SetPos((set_sel <= 2) ? 2 : 1, pos_table[set_sel]);
        LCD1602_WriteCmd(0x0F);
    }
    else
    {
        LCD1602_WriteCmd(0x0C); //关闭光标
    }
}

//====================主函数====================
void main(void)
{
    unsigned char key_val;

    LCD1602_Init();
    RTC_InitCheck();   //上电检查DS1302时钟，必要时初始化

    while(1)
    {
        // 正常模式下读取DS1302时间，设置模式下保持用户调整值
        if(set_mode == 0)
        {
            DS1302_ReadTime();
        }

        // DS18B20温度采集状态机
        DS18B20_Process();

        ShowCalendar();          // 刷新显示
        key_val = KeyScan();     // 按键扫描（非阻塞）
        KeyProcess(key_val);     // 按键处理
        DelayMs(50);             // 主循环延时，控制刷新速度
    }
}