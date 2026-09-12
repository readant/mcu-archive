#include <reg51.h>
//重新定义数据类型，简化书写
#define uchar unsigned char
#define uint unsigned int

/*
共阳极数码管段码表，储存在程序存储器（ROM）中，节省RAM
共阳极数码管：公共端接高电平，段引脚低电平时点亮。
字节的位对应关系（假设P1.0接a段，P1.1接b段,...,P1.7接dp段）
例如0x89 = 1000 1001（二进制）：
bit7(dp)=1（灭），bit6(g)=0（亮），bit5(f)=0（亮），bit4(e)=0（亮），
bit3(d)=1（灭），bit2(c)=0（亮），bit1(b)=0（亮），bit0(a)=1（灭）
点亮b,c,e,f,g段，组成"H"字形。其他段码类似。
*/
// 字符：H    E    L    L    O    C    5    1
uchar code seg[] = {0x89, 0x86, 0xC7, 0xC7, 0xC0, 0xC6, 0x92, 0xF9};

// 延时函数（约1秒，基于12MHz晶振）（四层循环嵌套来实现）
void delay()
{
	uchar h, i, j, k;                     //每层循环变量
	for (h = 5; h > 0; h--)              //外层循环--5次
		for (i = 4; i > 0; i--)          //中层循环--4次
			for (j = 116; j > 0; j--)    //内层循环--116次
				for (k = 214; k > 0; k--);//最内层循环214次，空语句
}

void main()
{
	uchar i;         //循环变量，用于索引段码表
	while (1)        //死循环
	{
		//依次取出段码表中的8个值，每个值对应一个字符
		for (i = 0; i < 8; i++)
		{
			P1 = seg[i];   // 输出段码到P1口，直接驱动数码管各段
			delay();       // 等待1秒
		}
	}
}

