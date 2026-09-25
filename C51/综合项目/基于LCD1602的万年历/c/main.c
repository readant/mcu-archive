#include <reg51.h>
#define uchar unsigned char
#define uint unsigned int

/************************ 函数前置声明 ************************/
void delay_ms(uint t);
void lcd_write_cmd(uchar cmd);
void lcd_write_data(uchar dat);
void lcd_init();
void lcd_set_pos(uchar row, uchar col);
void lcd_show_str(uchar row, uchar col, uchar *str);
void lcd_clear();
void ds_write_byte(uchar dat);
uchar ds_read_byte();
void ds_write_reg(uchar addr, uchar dat);
uchar ds_read_reg(uchar addr);
void ds_init();
void ds_set_time(uchar *time);
void ds_read_time(uchar *time);
uchar bcd2dec(uchar bcd);
uchar dec2bcd(uchar dec);
void key_scan();
void show_time();

/************************ 最终正确引脚定义 ************************/
sbit LCD_RS = P3^7;  // RS(4脚) → P3.7(17脚)
sbit LCD_RW = P3^5;  // RW(5脚) → P3.5(15脚)
sbit LCD_E  = P3^6;  // E(6脚)  → P3.6(16脚)
#define LCD_DATA P2   // D0-D7 → P2.0-P2.7

// DS1302实时时钟
sbit DS_RST  = P3^0;
sbit DS_SCLK = P3^1;
sbit DS_IO   = P3^2;

// 按键
sbit KEY_SHOW = P3^3;  // 显示键
sbit KEY_HIDE = P3^4;  // 隐藏键

/************************ 全局变量 ************************/
// 时间缓冲区：秒 分 时 日 月 星期 年（BCD码格式）
uchar time_buf[7] = {0x00, 0x00, 0x12, 0x27, 0x05, 0x03, 0x26};
uchar display_flag = 1; // 显示标志：1=显示 0=隐藏
uchar set_mode = 0;     // 设置模式：0=正常显示 1=年 2=月 3=日 4=时 5=分 6=秒
uchar blink_flag = 0;   // 闪烁标志：0=显示 1=熄灭

// 【终极】每个数字位单独保存上一次显示的值
uchar last_year_ten = 0xFF;   // 年十位
uchar last_year_one = 0xFF;   // 年个位
uchar last_month_ten = 0xFF;  // 月十位
uchar last_month_one = 0xFF;  // 月个位
uchar last_day_ten = 0xFF;    // 日十位
uchar last_day_one = 0xFF;    // 日个位
uchar last_week = 0xFF;       // 星期
uchar last_hour_ten = 0xFF;   // 时十位
uchar last_hour_one = 0xFF;   // 时个位
uchar last_min_ten = 0xFF;    // 分十位
uchar last_min_one = 0xFF;    // 分个位
uchar last_sec_ten = 0xFF;    // 秒十位
uchar last_sec_one = 0xFF;    // 秒个位

// 英文星期缩写数组（DS1302: 1=周一 2=周二 ... 7=周日）
uchar code week_str[8][4] = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

/************************ 基础延时函数 ************************/
void delay_ms(uint t) {
    uint i,j;
    for(i=t; i>0; i--)
        for(j=110; j>0; j--);
}

/************************ LCD1602驱动函数 ************************/
void lcd_write_cmd(uchar cmd) {
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DATA = cmd;
    delay_ms(1);
    LCD_E = 1;
    delay_ms(1);
    LCD_E = 0;
}

void lcd_write_data(uchar dat) {
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DATA = dat;
    delay_ms(1);
    LCD_E = 1;
    delay_ms(1);
    LCD_E = 0;
}

void lcd_init() {
    delay_ms(20); // 上电稳定延时
    lcd_write_cmd(0x38); // 8位数据 2行显示 5x7点阵
    delay_ms(5);
    lcd_write_cmd(0x38);
    delay_ms(1);
    lcd_write_cmd(0x38);
    lcd_write_cmd(0x0c); // 显示开 光标关
    lcd_write_cmd(0x06); // 光标自动右移
    lcd_write_cmd(0x01); // 清屏
    delay_ms(2);
    
    // 固定字符只在初始化时写一次，永不重写
    lcd_show_str(0, 0, "20");
    lcd_show_str(0, 4, "-");
    lcd_show_str(0, 7, "-");
    lcd_show_str(1, 2, ":");
    lcd_show_str(1, 5, ":");
}

void lcd_set_pos(uchar row, uchar col) {
    if(row == 0) lcd_write_cmd(0x80 + col);
    else lcd_write_cmd(0x80 + 0x40 + col);
}

void lcd_show_str(uchar row, uchar col, uchar *str) {
    lcd_set_pos(row, col);
    while(*str) lcd_write_data(*str++);
}

void lcd_clear() {
    lcd_write_cmd(0x01);
    delay_ms(2);
    // 清屏后重新写入固定字符
    lcd_show_str(0, 0, "20");
    lcd_show_str(0, 4, "-");
    lcd_show_str(0, 7, "-");
    lcd_show_str(1, 2, ":");
    lcd_show_str(1, 5, ":");
    // 清屏后强制刷新所有数字位
    last_year_ten = 0xFF;
    last_year_one = 0xFF;
    last_month_ten = 0xFF;
    last_month_one = 0xFF;
    last_day_ten = 0xFF;
    last_day_one = 0xFF;
    last_week = 0xFF;
    last_hour_ten = 0xFF;
    last_hour_one = 0xFF;
    last_min_ten = 0xFF;
    last_min_one = 0xFF;
    last_sec_ten = 0xFF;
    last_sec_one = 0xFF;
}

/************************ DS1302驱动函数 ************************/
void ds_write_byte(uchar dat) {
    uchar i;
    for(i=0; i<8; i++) {
        DS_SCLK = 0;
        DS_IO = (dat & 0x01) ? 1 : 0;
        dat >>= 1;
        DS_SCLK = 1;
    }
}

uchar ds_read_byte() {
    uchar i, dat=0;
    DS_IO = 1; // 释放总线
    for(i=0; i<8; i++) {
        dat >>= 1;
        DS_SCLK = 0;
        if(DS_IO) dat |= 0x80;
        DS_SCLK = 1;
    }
    return dat;
}

void ds_write_reg(uchar addr, uchar dat) {
    DS_RST = 0;
    DS_SCLK = 0;
    DS_RST = 1;
    ds_write_byte(addr);
    ds_write_byte(dat);
    DS_RST = 0;
}

uchar ds_read_reg(uchar addr) {
    uchar dat;
    DS_RST = 0;
    DS_SCLK = 0;
    DS_RST = 1;
    ds_write_byte(addr);
    dat = ds_read_byte();
    DS_RST = 0;
    return dat;
}

void ds_init() {
    ds_write_reg(0x8E, 0x00); // 关闭写保护
}

void ds_set_time(uchar *time) {
    uchar i, addr=0x80;
    ds_write_reg(0x8E, 0x00); // 关闭写保护
    for(i=0; i<7; i++) {
        ds_write_reg(addr, time[i]);
        addr += 2;
    }
    ds_write_reg(0x8E, 0x80); // 开启写保护
}

void ds_read_time(uchar *time) {
    uchar i, addr=0x81;
    for(i=0; i<7; i++) {
        time[i] = ds_read_reg(addr);
        addr += 2;
    }
}

// BCD码转十进制
uchar bcd2dec(uchar bcd) {
    return (bcd>>4)*10 + (bcd&0x0F);
}

// 十进制转BCD码
uchar dec2bcd(uchar dec) {
    return ((dec/10)<<4) | (dec%10);
}

/************************ 按键扫描函数 ************************/
void key_scan() {
    uint cnt; // 变量定义移到函数开头，符合C89标准
    
    // 显示键：短按显示，长按3秒进入设置模式
    if(KEY_SHOW == 0) {
        delay_ms(20);
        if(KEY_SHOW == 0) {
            display_flag = 1;
            cnt = 0;
            while(KEY_SHOW == 0) {
                delay_ms(10);
                cnt++;
                if(cnt > 300) { // 长按3秒
                    set_mode++;
                    if(set_mode > 6) {
                        set_mode = 0;
                        ds_set_time(time_buf); // 退出设置时保存时间
                        // 退出设置后强制刷新所有数字位
                        last_year_ten = 0xFF;
                        last_year_one = 0xFF;
                        last_month_ten = 0xFF;
                        last_month_one = 0xFF;
                        last_day_ten = 0xFF;
                        last_day_one = 0xFF;
                        last_week = 0xFF;
                        last_hour_ten = 0xFF;
                        last_hour_one = 0xFF;
                        last_min_ten = 0xFF;
                        last_min_one = 0xFF;
                        last_sec_ten = 0xFF;
                        last_sec_one = 0xFF;
                    }
                    break;
                }
            }
            while(KEY_SHOW == 0);
        }
    }

    // 隐藏键：短按隐藏，设置模式下为加1键
    if(KEY_HIDE == 0) {
        delay_ms(20);
        if(KEY_HIDE == 0) {
            if(set_mode == 0) {
                // 正常模式：隐藏显示
                display_flag = 0;
                lcd_clear();
            } else {
                // 设置模式：当前项加1
                uchar temp = bcd2dec(time_buf[set_mode-1]);
                temp++;
                
                // 时间合法性检查
                switch(set_mode) {
                    case 1: if(temp>99) temp=0; break; // 年：00-99
                    case 2: if(temp>12) temp=1; break; // 月：1-12
                    case 3: if(temp>31) temp=1; break; // 日：1-31
                    case 4: if(temp>23) temp=0; break; // 时：0-23
                    case 5: if(temp>59) temp=0; break; // 分：0-59
                    case 6: if(temp>59) temp=0; break; // 秒：0-59
                }
                
                time_buf[set_mode-1] = dec2bcd(temp);
            }
            while(KEY_HIDE == 0);
        }
    }
}

/************************ 单数字位刷新显示函数 ************************/
void show_time() {
    uchar ten, one;
    
    // 正常模式：只有对应数字位变化时才刷新
    if(set_mode == 0) {
        // 年
        ten = (time_buf[6]>>4) + '0';
        one = (time_buf[6]&0x0F) + '0';
        if(ten != last_year_ten) {
            last_year_ten = ten;
            lcd_set_pos(0, 2);
            lcd_write_data(ten);
        }
        if(one != last_year_one) {
            last_year_one = one;
            lcd_set_pos(0, 3);
            lcd_write_data(one);
        }
        
        // 月
        ten = (time_buf[4]>>4) + '0';
        one = (time_buf[4]&0x0F) + '0';
        if(ten != last_month_ten) {
            last_month_ten = ten;
            lcd_set_pos(0, 5);
            lcd_write_data(ten);
        }
        if(one != last_month_one) {
            last_month_one = one;
            lcd_set_pos(0, 6);
            lcd_write_data(one);
        }
        
        // 日
        ten = (time_buf[3]>>4) + '0';
        one = (time_buf[3]&0x0F) + '0';
        if(ten != last_day_ten) {
            last_day_ten = ten;
            lcd_set_pos(0, 8);
            lcd_write_data(ten);
        }
        if(one != last_day_one) {
            last_day_one = one;
            lcd_set_pos(0, 9);
            lcd_write_data(one);
        }
        
        // 星期
        if(time_buf[5] != last_week) {
            last_week = time_buf[5];
            lcd_show_str(0, 13, week_str[time_buf[5] & 0x07]);
        }
        
        // 时
        ten = (time_buf[2]>>4) + '0';
        one = (time_buf[2]&0x0F) + '0';
        if(ten != last_hour_ten) {
            last_hour_ten = ten;
            lcd_set_pos(1, 0);
            lcd_write_data(ten);
        }
        if(one != last_hour_one) {
            last_hour_one = one;
            lcd_set_pos(1, 1);
            lcd_write_data(one);
        }
        
        // 分
        ten = (time_buf[1]>>4) + '0';
        one = (time_buf[1]&0x0F) + '0';
        if(ten != last_min_ten) {
            last_min_ten = ten;
            lcd_set_pos(1, 3);
            lcd_write_data(ten);
        }
        if(one != last_min_one) {
            last_min_one = one;
            lcd_set_pos(1, 4);
            lcd_write_data(one);
        }
        
        // 秒
        ten = (time_buf[0]>>4) + '0';
        one = (time_buf[0]&0x0F) + '0';
        if(ten != last_sec_ten) {
            last_sec_ten = ten;
            lcd_set_pos(1, 6);
            lcd_write_data(ten);
        }
        if(one != last_sec_one) {
            last_sec_one = one;
            lcd_set_pos(1, 7);
            lcd_write_data(one);
        }
    }
    // 设置模式：当前选中项闪烁，其他项保持静态
    else {
        // 年
        if(set_mode == 1 && blink_flag) {
            lcd_show_str(0, 2, "  ");
        } else {
            ten = (time_buf[6]>>4) + '0';
            one = (time_buf[6]&0x0F) + '0';
            if(ten != last_year_ten || set_mode == 1) {
                last_year_ten = ten;
                lcd_set_pos(0, 2);
                lcd_write_data(ten);
            }
            if(one != last_year_one || set_mode == 1) {
                last_year_one = one;
                lcd_set_pos(0, 3);
                lcd_write_data(one);
            }
        }
        
        // 月
        if(set_mode == 2 && blink_flag) {
            lcd_show_str(0, 5, "  ");
        } else {
            ten = (time_buf[4]>>4) + '0';
            one = (time_buf[4]&0x0F) + '0';
            if(ten != last_month_ten || set_mode == 2) {
                last_month_ten = ten;
                lcd_set_pos(0, 5);
                lcd_write_data(ten);
            }
            if(one != last_month_one || set_mode == 2) {
                last_month_one = one;
                lcd_set_pos(0, 6);
                lcd_write_data(one);
            }
        }
        
        // 日
        if(set_mode == 3 && blink_flag) {
            lcd_show_str(0, 8, "  ");
        } else {
            ten = (time_buf[3]>>4) + '0';
            one = (time_buf[3]&0x0F) + '0';
            if(ten != last_day_ten || set_mode == 3) {
                last_day_ten = ten;
                lcd_set_pos(0, 8);
                lcd_write_data(ten);
            }
            if(one != last_day_one || set_mode == 3) {
                last_day_one = one;
                lcd_set_pos(0, 9);
                lcd_write_data(one);
            }
        }
        
        // 星期（设置模式下不闪烁，保持静态）
        if(time_buf[5] != last_week) {
            last_week = time_buf[5];
            lcd_show_str(0, 13, week_str[time_buf[5] & 0x07]);
        }
        
        // 时
        if(set_mode == 4 && blink_flag) {
            lcd_show_str(1, 0, "  ");
        } else {
            ten = (time_buf[2]>>4) + '0';
            one = (time_buf[2]&0x0F) + '0';
            if(ten != last_hour_ten || set_mode == 4) {
                last_hour_ten = ten;
                lcd_set_pos(1, 0);
                lcd_write_data(ten);
            }
            if(one != last_hour_one || set_mode == 4) {
                last_hour_one = one;
                lcd_set_pos(1, 1);
                lcd_write_data(one);
            }
        }
        
        // 分
        if(set_mode == 5 && blink_flag) {
            lcd_show_str(1, 3, "  ");
        } else {
            ten = (time_buf[1]>>4) + '0';
            one = (time_buf[1]&0x0F) + '0';
            if(ten != last_min_ten || set_mode == 5) {
                last_min_ten = ten;
                lcd_set_pos(1, 3);
                lcd_write_data(ten);
            }
            if(one != last_min_one || set_mode == 5) {
                last_min_one = one;
                lcd_set_pos(1, 4);
                lcd_write_data(one);
            }
        }
        
        // 秒
        if(set_mode == 6 && blink_flag) {
            lcd_show_str(1, 6, "  ");
        } else {
            ten = (time_buf[0]>>4) + '0';
            one = (time_buf[0]&0x0F) + '0';
            if(ten != last_sec_ten || set_mode == 6) {
                last_sec_ten = ten;
                lcd_set_pos(1, 6);
                lcd_write_data(ten);
            }
            if(one != last_sec_one || set_mode == 6) {
                last_sec_one = one;
                lcd_set_pos(1, 7);
                lcd_write_data(one);
            }
        }
    }
}

/************************ 主函数 ************************/
void main() {
    uint blink_cnt = 0;
    
    lcd_init();
    ds_init();
    
    // 首次使用取消下面注释设置初始时间，运行一次后重新注释！
    // ds_set_time(time_buf);
    
    while(1) {
        if(set_mode == 0) {
            ds_read_time(time_buf); // 正常模式下读取DS1302时间
        }
        
        key_scan(); // 扫描按键
        
        // 闪烁计时（500ms闪烁一次，仅用于设置模式）
        blink_cnt++;
        if(blink_cnt >= 50) { // 主循环10ms一次，50次=500ms
            blink_cnt = 0;
            blink_flag = ~blink_flag;
        }
        
        if(display_flag) {
            show_time(); // 显示时间（单数字位刷新）
        }
        
        delay_ms(10); // 主循环10ms执行一次
    }
}