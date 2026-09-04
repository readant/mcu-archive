/*------------------------------------------------
  功能：24C02存储用户设置
  硬件：24C02接I2C，按键切换设置
  说明：用户设置的亮度、音量等参数保存到24C02
------------------------------------------------*/
#include <reg51.h>

sbit SCL = P2^0;
sbit SDA = P2^1;
sbit KEY = P2^2;
sbit LED = P1^0;

#define ADDR_WRITE 0xA0
#define ADDR_READ  0xA1
#define SET_ADDR   0x00  // 设置存储地址

void Delay(unsigned int t) { while (--t); }
void DelayUs(unsigned char us) { while (us--); }

void I2C_Start(void) { SDA=1; SCL=1; DelayUs(5); SDA=0; DelayUs(5); SCL=0; }
void I2C_Stop(void) { SDA=0; SCL=1; DelayUs(5); SDA=1; DelayUs(5); }

void I2C_SendByte(unsigned char dat)
{
    unsigned char i;
    for (i=0; i<8; i++) { SDA=(dat&0x80)>>7; dat<<=1; SCL=1; DelayUs(5); SCL=0; DelayUs(5); }
    SDA=1; SCL=1; DelayUs(5); SCL=0;
}

unsigned char I2C_ReadByte(void)
{
    unsigned char i, dat=0;
    SDA=1;
    for (i=0; i<8; i++) { SCL=1; DelayUs(5); dat<<=1; if(SDA) dat|=1; SCL=0; DelayUs(5); }
    return dat;
}

void Write24C02(unsigned char addr, unsigned char dat)
{
    I2C_Start(); I2C_SendByte(ADDR_WRITE); I2C_SendByte(addr); I2C_SendByte(dat); I2C_Stop();
    Delay(100);
}

unsigned char Read24C02(unsigned char addr)
{
    unsigned char dat;
    I2C_Start(); I2C_SendByte(ADDR_WRITE); I2C_SendByte(addr); I2C_Stop();
    I2C_Start(); I2C_SendByte(ADDR_READ); dat=I2C_ReadByte(); I2C_Stop();
    return dat;
}

void main(void)
{
    unsigned char brightness;

    // 读取上次保存的亮度值
    brightness = Read24C02(SET_ADDR);
    if (brightness > 10) brightness = 5;  // 初始化检查

    LED = 0;  // 点亮LED

    while (1)
    {
        if (KEY == 0)
        {
            Delay(20);
            if (KEY == 0)
            {
                brightness++;
                if (brightness > 10) brightness = 1;

                // 保存到24C02
                Write24C02(SET_ADDR, brightness);

                while (!KEY);
            }
        }
    }
}
