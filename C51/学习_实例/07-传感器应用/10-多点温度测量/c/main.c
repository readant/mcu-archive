/*
 * 多点温度测量（多个DS18B20）
 * 功能：在同一总线上挂载多个DS18B20，分别读取温度
 * 硬件：P3.7接多个DS18B20的数据线（并联）
 * 说明：每个DS18B20有唯一的64位ROM编码
 *       通过ROM搜索算法可以找到总线上所有设备
 *       通过ROM匹配命令可以与指定设备通信
 */
#include <reg51.h>

sbit DS18B20_DQ = P3^7;

#define MAX_DEVICES 3  // 最多挂载3个设备

/* 存储每个设备的ROM编码（实际需要ROM搜索算法获取） */
unsigned char rom_codes[MAX_DEVICES][8] = {
    {0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
    {0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
    {0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
};

void delay_us(unsigned char us)
{
    while (us--);
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

bit ds18b20_reset(void)
{
    bit presence;
    DS18B20_DQ = 1;
    DS18B20_DQ = 0;
    delay_us(480);
    DS18B20_DQ = 1;
    delay_us(60);
    presence = DS18B20_DQ;
    delay_us(420);
    return presence;
}

void ds18b20_write_byte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DS18B20_DQ = dat & 0x01;
        delay_us(2);
        DS18B20_DQ = 1;
        dat >>= 1;
        delay_us(60);
    }
}

unsigned char ds18b20_read_byte(void)
{
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        DS18B20_DQ = 0;
        delay_us(2);
        DS18B20_DQ = 1;
        delay_us(8);
        dat >>= 1;
        if (DS18B20_DQ) dat |= 0x80;
        delay_us(50);
    }
    return dat;
}

/*
 * ROM匹配命令 - 与指定设备通信
 */
void ds18b20_match_rom(unsigned char *rom)
{
    unsigned char i;
    ds18b20_write_byte(0x55);  // 匹配ROM命令
    for (i = 0; i < 8; i++)
        ds18b20_write_byte(rom[i]);
}

/*
 * 跳过ROM - 对所有设备广播
 */
void ds18b20_skip_rom(void)
{
    ds18b20_write_byte(0xCC);
}

/*
 * 读取指定设备的温度
 */
float ds18b20_read_temp(unsigned char *rom)
{
    unsigned char temp_l, temp_h;
    int temp_raw;
    float temperature;

    ds18b20_reset();
    ds18b20_match_rom(rom);  // 选择指定设备
    ds18b20_write_byte(0x44); // 启动转换

    delay_ms(750);

    ds18b20_reset();
    ds18b20_match_rom(rom);
    ds18b20_write_byte(0xBE); // 读暂存器
    temp_l = ds18b20_read_byte();
    temp_h = ds18b20_read_byte();

    temp_raw = (temp_h << 8) | temp_l;
    temperature = temp_raw * 0.0625;

    return temperature;
}

void main(void)
{
    float temps[MAX_DEVICES];
    unsigned char i;

    while (1)
    {
        for (i = 0; i < MAX_DEVICES; i++)
        {
            temps[i] = ds18b20_read_temp(rom_codes[i]);
        }
        delay_ms(1000);
    }
}
