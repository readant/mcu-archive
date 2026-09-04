/******************************************************************************
 * 100%按电路图：51单片机密码锁程序
 * 硬件对应：
 *  LCD1602: RS=P2.5, RW=P2.6, E=P2.7, D0~D7=P0.0~P0.7（外接上拉电阻）
 *  4x4键盘: 行线P1.0~P1.3（P0/P1/P2/P3），列线P1.4~P1.7（P4/P5/P6/P7）
 *  晶振: 12MHz
 ******************************************************************************/
#include <reg51.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

/* 状态定义 */
#define STATE_INPUT_PASS    0   // 等待输入密码
#define STATE_VERIFY_PASS   1   // 正在验证密码（按#后进入）
#define STATE_WELCOME       2   // 验证成功后的欢迎界面
#define STATE_OLD_PASS      3   // 修改密码：输入旧密码
#define STATE_NEW_PASS      4   // 修改密码：输入新密码
#define STATE_CONFIRM_PASS  5   // 修改密码：再次确认新密码

#define PASS_LEN            6   // 密码长度固定6位

/* LCD控制引脚
sbit RS = P2^5;
sbit RW = P2^6;
sbit E  = P2^7;

/* 全局变量 */
uchar password[PASS_LEN] = {'1', '1', '1', '1', '1', '1'}; // 默认密码
uchar input_buf[PASS_LEN];    // 输入缓冲区
uchar temp_buf[PASS_LEN];     // 新密码临时存储
uchar input_cnt = 0;          // 输入计数
uchar state     = STATE_INPUT_PASS;		 // 当前状态机

/* 函数声明 */
void delay_ms(uint t);
void Lcd_Write_Cmd(uchar cmd);
void Lcd_Write_Data(uchar dat);
void Lcd_Init(void);
void Lcd_Show_String(uchar x, uchar y, uchar *str);
uchar Key_Scan(void);
void Clear_Input(void);
bit  Verify_Password(uchar *a, uchar *b);
void Handle_Digit_Input(uchar *buf, uchar key);

/******************************************************************************
 * 延时函数（12MHz晶振，标准延时，与电路匹配）
 ******************************************************************************/
void delay_ms(uint t)
{
    uint i, j;
    for (i = t; i > 0; i--)				  // 外循环次
        for (j = 110; j > 0; j--);		  // 内层循环110，约1ms
}

/******************************************************************************
 * LCD写命令
 ******************************************************************************/
void Lcd_Write_Cmd(uchar cmd)
{
    RS = 0;      // 命令模式
    RW = 0;      // 写模式
    P0 = cmd;    // 数据放到P0口（LCD数据线）
    E  = 1;      // E高电平
    delay_ms(1); // 保持时间（至少450ns，1ms足够）
    E  = 0;      // E下降沿，LCD执行命令
}

/******************************************************************************
 * LCD写数据
 ******************************************************************************/
void Lcd_Write_Data(uchar dat)
{
    RS = 1;
    RW = 0;
    P0 = dat;
    E  = 1;
    delay_ms(1);
    E  = 0;
}

/******************************************************************************
 * LCD初始化（8位模式，2行显示）
 ******************************************************************************/
void Lcd_Init(void)
{
    delay_ms(15);
    Lcd_Write_Cmd(0x38); // 功能设置：8位接口，2行，5x7点阵
    Lcd_Write_Cmd(0x0C); // 开显示，无光标
    Lcd_Write_Cmd(0x06); // 地址自动+1，光标右移
    Lcd_Write_Cmd(0x01); // 清屏
    delay_ms(2);
}

/******************************************************************************
 * LCD显示字符串（x:列号0~15，y:行号0/1）
 ******************************************************************************/
void Lcd_Show_String(uchar x, uchar y, uchar *str)
{
    uchar addr = (y == 0) ? (0x00 + x) : (0x40 + x);
    Lcd_Write_Cmd(0x80 | addr);
    while (*str)
        Lcd_Write_Data(*str++);
}

/******************************************************************************
 * 【100%按电路图的键盘扫描】
 * 行线：P1.0~P1.3（输出4位），列线：P1.4~P1.7（输入4位）
 ******************************************************************************/
uchar Key_Scan(void)
{
    uchar row, col;

    // 1. 检测是否有按键按下（先写行线全低，确定初始电平）
    P1 = 0x0F; // 低4位行线全输出低，高4位列线全上拉输入
    if ((P1 & 0xF0) != 0xF0) // 列线出现低电平说明有按键按下
    {
        delay_ms(10); // 消抖延时
        if ((P1 & 0xF0) != 0xF0) // 再次确认按键按下
        {
            // 2. 逐行扫描，逐位列检测
            for (row = 0; row < 4; row++)
            {
                // 仅拉低当前行，其他行保持高电平，列线保持上拉
                P1 = 0xF0 | ~(1 << row);
                if ((P1 & 0xF0) != 0xF0) // 当前行有按键按下
                {
                    // 3. 读取列值（注意行线/列线命名完全对应）
                    col = (P1 & 0xF0) >> 4;
                    switch (col)
                    {
                        case 0xE: col = 0; break; // 1110 = 第0列（P4）
                        case 0xD: col = 1; break; // 1101 = 第1列（P5）
                        case 0xB: col = 2; break; // 1011 = 第2列（P6）
                        case 0x7: col = 3; break; // 0111 = 第3列（P7）
                        default: return 0xFF;
                    }

                    // 4. 等待按键释放，防止重复触发
                    while ((P1 & 0xF0) != 0xF0);
                    delay_ms(10); // 释放消抖

                    // 5. 按键映射（与电路图的布局完全一致）
                    switch (row * 4 + col)
                    {
                        case 0:  return '1'; case 1:  return '2'; case 2:  return '3'; case 3:  return 'A';
                        case 4:  return '4'; case 5:  return '5'; case 6:  return '6'; case 7:  return 'B';
                        case 8:  return '7'; case 9:  return '8'; case 10: return '9'; case 11: return 'C';
                        case 12: return '*'; case 13: return '0'; case 14: return '#'; case 15: return 'D';
                        default: return 0xFF;
                    }
                }
            }
        }
    }
    return 0xFF; // 无按键按下
}

/******************************************************************************
 * 清空输入缓冲区
 ******************************************************************************/
void Clear_Input(void)
{
    uchar i;
    for (i = 0; i < PASS_LEN; i++)
        input_buf[i] = 0;
    input_cnt = 0;
}

/******************************************************************************
 * 密码验证（返回1表示两个密码一致）
 ******************************************************************************/
bit Verify_Password(uchar *a, uchar *b)
{
    uchar i;
    for (i = 0; i < PASS_LEN; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

/******************************************************************************
 * 处理数字输入，显示*号并存储
 ******************************************************************************/
void Handle_Digit_Input(uchar *buf, uchar key)
{
    if (input_cnt < PASS_LEN)
    {
        if (input_cnt == 0)
            Lcd_Write_Cmd(0x80 | 0x40); // 光标定位到第二行开头
        buf[input_cnt] = key;
        Lcd_Write_Data('*');
        input_cnt++;
    }
}

/******************************************************************************
 * 主程序（状态机）
 ******************************************************************************/
void main(void)
{
    uchar key;
    bit   pass_ok;

    // 硬件初始化
    P1 = 0xFF; // P1口全部拉高，准双向口内部上拉
    Lcd_Init();

    // 开机提示
    Lcd_Show_String(0, 0, "Enter Password!");

    // 主循环
    while (1)
    {
        key = Key_Scan();

        // 全局*键处理（取消/进入下一步）
        if (key == '*')
        {
            switch (state)
            {
                case STATE_INPUT_PASS:
                    Lcd_Write_Cmd(0x01);
                    Lcd_Show_String(0, 0, "Enter Password!");
                    Clear_Input();
                    break;
                case STATE_WELCOME:
                    Lcd_Write_Cmd(0x01);
                    Lcd_Show_String(0, 0, "Enter Old Pass!");
                    Clear_Input();
                    state = STATE_OLD_PASS;
                    break;
                case STATE_OLD_PASS:
                case STATE_NEW_PASS:
                case STATE_CONFIRM_PASS:
                    Lcd_Write_Cmd(0x01);
                    Lcd_Show_String(0, 0, "Welcome!");
                    Clear_Input();
                    state = STATE_WELCOME;
                    break;
                default: break;
            }
            continue;
        }

        // 状态处理逻辑
        switch (state)
        {
            // 状态0：密码输入
            case STATE_INPUT_PASS:
                if (key >= '0' && key <= '9')
                    Handle_Digit_Input(input_buf, key);
                else if (key == '#' && input_cnt == PASS_LEN)
                    state = STATE_VERIFY_PASS;
                break;

            // 状态1：验证密码
            case STATE_VERIFY_PASS:
                pass_ok = Verify_Password(input_buf, password);
                Lcd_Write_Cmd(0x01);
                if (pass_ok)
                    Lcd_Show_String(0, 0, "Welcome!");
                else
                {
                    Lcd_Show_String(0, 0, "Wrong Pass!");
                    delay_ms(2000);
                    Lcd_Write_Cmd(0x01);
                    Lcd_Show_String(0, 0, "Enter Password!");
                }
                Clear_Input();
                state = pass_ok ? STATE_WELCOME : STATE_INPUT_PASS;
                break;

            // 状态2：欢迎界面
            case STATE_WELCOME:
                break;

            // 状态3：输入旧密码（修改密码流程）
            case STATE_OLD_PASS:
                if (key >= '0' && key <= '9')
                    Handle_Digit_Input(input_buf, key);
                else if (key == '#' && input_cnt == PASS_LEN)
                {
                    pass_ok = Verify_Password(input_buf, password);
                    Lcd_Write_Cmd(0x01);
                    if (pass_ok)
                    {
                        Lcd_Show_String(0, 0, "New Password!");
                        Clear_Input();
                        state = STATE_NEW_PASS;
                    }
                    else
                    {
                        Lcd_Show_String(0, 0, "Wrong Old Pass!");
                        delay_ms(2000);
                        Lcd_Write_Cmd(0x01);
                        Lcd_Show_String(0, 0, "Welcome!");
                        Clear_Input();
                        state = STATE_WELCOME;
                    }
                }
                break;

            // 状态4：输入新密码
            case STATE_NEW_PASS:
                if (key >= '0' && key <= '9')
                    Handle_Digit_Input(temp_buf, key);
                else if (key == '#' && input_cnt == PASS_LEN)
                {
                    Lcd_Write_Cmd(0x01);
                    Lcd_Show_String(0, 0, "Confirm New");
                    Clear_Input();
                    state = STATE_CONFIRM_PASS;
                }
                break;

            // 状态5：确认新密码
            case STATE_CONFIRM_PASS:
                if (key >= '0' && key <= '9')
                    Handle_Digit_Input(input_buf, key);
                else if (key == '#' && input_cnt == PASS_LEN)
                {
                    pass_ok = Verify_Password(input_buf, temp_buf);
                    Lcd_Write_Cmd(0x01);
                    if (pass_ok)
                    {
                        uchar i;
                        for (i = 0; i < PASS_LEN; i++)
                            password[i] = temp_buf[i];
                        Lcd_Show_String(0, 0, "Changed!");
                        delay_ms(2000);
                        Lcd_Write_Cmd(0x01);
                        Lcd_Show_String(0, 0, "Welcome!");
                        state = STATE_WELCOME;
                    }
                    else
                    {
                        uchar i;
                        for (i = 0; i < PASS_LEN; i++) temp_buf[i] = 0;
                        Lcd_Show_String(0, 0, "Mismatch!");
                        delay_ms(2000);
                        Lcd_Write_Cmd(0x01);
                        Lcd_Show_String(0, 0, "New Password!");
                        state = STATE_NEW_PASS;
                    }
                    Clear_Input();
                }
                break;

            default:
                Lcd_Write_Cmd(0x01);
                Lcd_Show_String(0, 0, "Enter Password!");
                Clear_Input();
                state = STATE_INPUT_PASS;
                break;
        }
    }
}
