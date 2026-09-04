#include <reg51.h>

main()
{
	TMOD = 0x20;
	SM0 = 0;
	SM1 = 1;
	REN = 1;
	PCON = 0;
	TH1 = 0xFD;
	TL1 = 0xFD;

	TR1 = 1;
	P1 = SBUF;while(! RI);RI = 0;
	SBUF = P1;while(! TI);TI = 0;
}