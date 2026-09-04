#include<reg51.h>

//延时程序
void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i=0; i<ms; i++)
		for(j=0; j<110; j++);
}

void main(void)
{
	P0 = 0x00;  // 所有LED全灭
	P2 = 0xFF;
	while(1)    // 死循环
	{
		P0 = P2;        
		
		delay_ms(20);   // 消抖
	}
}