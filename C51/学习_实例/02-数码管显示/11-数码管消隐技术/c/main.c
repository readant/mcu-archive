/*
 * 数码管消隐技术
 * 功能：动态扫描显示中消除"鬼影"（残影）问题
 * 硬件：P0口接段选, P2口接位选, 4位共阴数码管
 * 说明：动态扫描切换时，如果不先消隐再切换位选，
 *       上一位的数据会短暂显示在下一位上，形成"鬼影"
 *       解决方法：段选先全灭→切换位选→再送新段选
 */
#include <reg51.h>

unsigned char code SEG_TABLE[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,  // 0-4
    0x6D, 0x7D, 0x07, 0x7F, 0x6F   // 5-9
};

unsigned char disp_buf[4] = {1, 2, 3, 4};  // 显示缓冲区

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/*
 * 显示4位数码管（带消隐）
 */
void display_scan(void)
{
    unsigned char i;
    unsigned char位选[] = {0xFE, 0xFD, 0xFB, 0xF7};

    for (i = 0; i < 4; i++)
    {
        P0 = 0x00;         // 1. 先消隐（段选全灭）
        P2 = 0xFF;         //    位选全灭
        P0 = SEG_TABLE[disp_buf[i]];  // 2. 送新段选
        P2 = 位选[i];      // 3. 切换位选
        delay_ms(2);       // 4. 短暂显示
    }
}

void main(void)
{
    unsigned int cnt = 0;

    while (1)
    {
        display_scan();
        cnt++;
        if (cnt >= 5000)
        {
            cnt = 0;
            disp_buf[3]++;
            if (disp_buf[3] >= 10) { disp_buf[3] = 0; disp_buf[2]++; }
            if (disp_buf[2] >= 10) { disp_buf[2] = 0; disp_buf[1]++; }
            if (disp_buf[1] >= 10) { disp_buf[1] = 0; disp_buf[0]++; }
            if (disp_buf[0] >= 10) disp_buf[0] = 0;
        }
    }
}
