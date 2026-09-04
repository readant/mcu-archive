#include <reg51.h>

// ================= 硬件引脚定义 =================
sbit LED_ALLOW = P3^6;      // 抢答允许指示灯 (P3.6)
sbit KEY_START = P3^0;      // 主持人开始按键 (P3.0)
sbit PIN_INT0 = P3^2;       // 中断检测引脚 (P3.2)

// ================= 常量定义 =================
#define ALLOW_ON  0         // 允许灯点亮状态 (低电平亮)
#define ALLOW_OFF 1         // 允许灯熄灭状态
#define ANSWER_LED_ON 0     // 抢答灯点亮状态 (低电平亮)

// ================= 共阳数码管段码表 (0~8) =================
// 对应 P0 口输出，abcdefg 对应 P0.0~P0.6
unsigned char code SEG_TABLE[9] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80  // 8
};

// ================= 全局变量 =================
bit g_bStartFlag = 0;       // 抢答开始标志
bit g_bAnswerLock = 0;      // 抢答锁存标志
unsigned char g_cWinner = 0;// 获胜者编号

// ================= 延时函数 =================
void DelayMs(unsigned int ms)
{
    unsigned int i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

// ================= INT0 中断服务函数 =================
// 响应任意一路抢答按键
void EX0_ISR(void) interrupt 0
{
    unsigned char i;
    
    // 只有在“开始”后且“未锁存”时才响应
    if(g_bStartFlag && !g_bAnswerLock)	 //抢答开始标志、抢答锁存标志
    {
        g_bAnswerLock = 1;      // 置位锁存标志，防止重复触发
        LED_ALLOW= ALLOW_OFF;   // 熄灭抢答允许灯
        
        // 扫描 P2 口找到获胜的按键			
        for(i = 0; i < 8; i++)
        {											
            // 检测对应的按键是否被按下 (低电平)
            if((P2 & (1 << i)) == 0) 
            {
                g_cWinner = i + 1;     // 记录编号 (1-8)
                P1 = ~(1 << i);     // 点亮对应指示灯 (低电平亮)
                P0 = SEG_TABLE[g_cWinner]; // 显示编号
                break;              // 退出循环
            }
        }
        
       EX0 = 0; // 关闭外部中断，防止抖动或松手造成重复触发
    }
}

// ================= 系统初始化 =================
void SystemInit(void)
{
    // 端口初始状态
    P0 = 0xFF;  // 数码管熄灭
    P1 = 0xFF;  // 指示灯熄灭
    LED_ALLOW= ALLOW_OFF; // 允许灯熄灭
    P2 = 0xFF;  // 按键口全部置高 (使能内部上拉)
    
    // 中断初始化
    IT0 = 1;    // INT0 下降沿触发
    EX0 = 1;    // 使能 INT0 中断
    EA  = 1;    // 开启总中断
}

// ================= 主函数 =================
void main(void)
{
    SystemInit(); // 初始化

    while(1)
    {
        // 扫描主持人开始按键 (P3.0)
        if(KEY_START == 0)				//检测主持人是否按下按键
        {
            DelayMs(20); // 消抖
            if(KEY_START == 0)
            {
                // 重置抢答系统
                g_bStartFlag = 1;			//抢答开始标志
                g_bAnswerLock = 0;			// 抢答锁存标志  
                g_cWinner = 0;				// 获胜者编号	   
                
                P1 = 0xFF;    // 清空指示灯
                P0 = 0xFF;    // 清空数码管	 				  
                LED_ALLOW = ALLOW_ON; // 点亮允许指示灯
                
                EX0 = 1;      // 重新开启外部中断，等待抢答
                while(KEY_START == 0); // 等待按键松开
            }
        }
    }
}