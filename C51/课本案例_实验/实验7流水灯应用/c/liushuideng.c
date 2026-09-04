#include<reg51.h>
#define uint unsigned int

void delay(uint k)
{
	uint i,j;
	for(i = 7;i<k;i++)
		for(j = 0;j<124;j++);

}
main()
{
	while(1)
	{
		P1 = 0xfe;delay(500);
		P1 = 0xfd;delay(500);
		P1 = 0xfb;delay(500);
		P1 = 0xf7;delay(500);
		P1 = 0xef;delay(500);
		P1 = 0xdf;delay(500);
		P1 = 0xbf;delay(500);
		P1 = 0x7f;delay(500);
	}
}