/**********************************************
功能：32x16LED汉字字幕程序
编译：ICC AVR
作者：刘伟
**********************************************/
#include <32x16led.h>
void main (void)
{	
	while (1)
	{
		led_initial();
		print_char("TEST LED 0123456789 abc");
		print_char("我爱金鱼");
		delay(50);
	//	led_initial();
		print_char("自己编的练习程序,");
		print_char("显示效果与延时参数和电脑速度有很大关系.");
		print_char("我的电脑太差运行起来老是一顿一顿的^_^");
		print_char("相信换台好点的电脑会有更好的效果! 刘伟");							
		delay(50);

	}
}

