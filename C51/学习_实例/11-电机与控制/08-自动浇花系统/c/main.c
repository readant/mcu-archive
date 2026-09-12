/*------------------------------------------------
  功能：自动浇花系统
  硬件：土壤湿度传感器接ADC0832，水泵接P1.0
  说明：土壤干燥时水泵自动浇水
------------------------------------------------*/
#include <reg51.h>

sbit CS = P1^3;
sbit CLK = P1^2;
sbit DIO = P1^1;
sbit PUMP = P1^0;
sbit LED = P1^5;

#define DRY_THRESHOLD 150   // 干燥阈值（低于此值浇水）

void Delay(unsigned int t)
{
    while (--t);
}

unsigned char Read_ADC0832(unsigned char channel)
{
    unsigned char i, dat = 0;

    CS = 0;
    CLK = 0; DIO = 1;
    CLK = 1; CLK = 0;
    DIO = channel;
    CLK = 1; CLK = 0;
    DIO = 1;
    CLK = 1; CLK = 0;

    for (i = 0; i < 8; i++)
    {
        CLK = 1; CLK = 0;
        dat <<= 1;
        if (DIO) dat |= 0x01;
    }

    CS = 1;
    return dat;
}

void main(void)
{
    unsigned char soil_val;

    PUMP = 0;  // 水泵关闭
    LED = 1;   // LED熄灭

    while (1)
    {
        soil_val = Read_ADC0832(0);  // 读取土壤湿度

        if (soil_val < DRY_THRESHOLD)  // 土壤干燥
        {
            PUMP = 1;   // 开启水泵
            LED = 0;    // 点亮指示灯
        }
        else  // 土壤湿润
        {
            PUMP = 0;   // 关闭水泵
            LED = 1;    // 熄灭指示灯
        }

        Delay(10000);
    }
}
