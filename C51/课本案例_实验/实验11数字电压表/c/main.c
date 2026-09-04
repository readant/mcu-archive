#include<reg51.h>
#define uint unsigned int
#define uchar unsigned char

uchar code table[]
= {0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0xee,0x3e,0x9c,
0x7a,0x9e,0x8e};

sbit START= P3^0;    //A/D转换启动信号输入端
sbit EOC= P3^1;     //转换结束信号输出引脚。开始转换时为低电平，转换结束时为高电平
sbit OE= P3^2;     //输出允许控制端，用以打开三态数据输出锁存器
sbit dot= P1^0;    //数码管的小数点控制位

/****************延时子程序****************/
void delay(uint m)
{
    while(m--);
}

void main()
{
    uint temp;
    START= 0;
    OE= 0;
    START= 1;        //启动A/D转换
    START= 0;
	while(1)
    {
        if(EOC== 1)        //查询0808转换结束信号
        {
            OE= 1;       //这时D0~D7输出转换后的数据，CPU可以进行读取数据
            temp= P0;    //读取数据
            temp= temp* 1.0/255* 500; //将获得的数值转换成模拟电压对应的电压值
            OE= 0;       //D0~D7引脚呈高阻状态
            P2= 0xfe;    //选中数码管的个位
            P1= table[temp% 10]; //显示temp的个位数值
            delay(500);  //延时数码管
            P2= 0xfd;
            P1= table[temp/10% 10]; //显示temp的十位数值
            delay(500);
            P2= 0xfb;
            P1= table[temp/100% 10]; //显示temp的百位数值
            dot= 1;      //小数点显示
            delay(500);
            START= 1;    //启动下一次A/D转换
            START= 0;
        }
    }
}