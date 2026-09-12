/*------------------------------------------------
  功能：8x8点阵滚动显示
  硬件：P0口接行线，P1口接列线
  说明：字符从右向左滚动
------------------------------------------------*/
#include <reg51.h>

#define ROW_PORT P0
#define COL_PORT P1

unsigned char code Message[] = {
    0x00, 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x00,  // A
    0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00,  // B
    0x00, 0x3C, 0x66, 0x60, 0x60, 0x66, 0x3C, 0x00   // C
};

#define MSG_LEN 24

void Delay(unsigned int t)
{
    while (--t);
}

void ScrollDisplay(void)
{
    unsigned char i, j;
    unsigned char offset = 0;

    for (offset = 0; offset < MSG_LEN; offset++)
    {
        for (j = 0; j < 50; j++)  // 每个位置显示一段时间
        {
            for (i = 0; i < 8; i++)
            {
                ROW_PORT = 0x01 << i;
                if (offset + i < MSG_LEN)
                    COL_PORT = ~Message[offset + i];
                else
                    COL_PORT = 0xFF;
                Delay(200);
                COL_PORT = 0xFF;
            }
        }
    }
}

void main(void)
{
    while (1)
    {
        ScrollDisplay();
    }
}
