#include<reg51.h>
#define uint unsigned int
#define uchar unsigned char

void delay(uint);
uchar code table[] = 
	{	    0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e	};

void main()
{
	uchar m = 0;
	while(1)
	{
		if(m == 16) m = 0;
		P1 = table[m ++ ];
		delay(1000);
	}
}

void delay(uint k)
{	
	unsigned char a,b;
	for(;k>0;k--)
		for(b = 142;b>0;b--)
			for(a=2;a>0;a--);
}