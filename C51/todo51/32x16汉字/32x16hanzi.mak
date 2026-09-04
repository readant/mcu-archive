CC = iccavr
CFLAGS =  -ID:\ICC\include\ -IE:\32x16ºº×Ö -e -DATMEGA  -l -g -Mavr_enhanced 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LD:\ICC\lib\ -g -ucrtatmega.o -bfunc_lit:0x54.0x4000 -dram_end:0x45f -bdata:0x60.0x45f -dhwstk_size:16 -beeprom:1.512 -fihx_coff -S2
FILES = main.o 

32x16hanzi:	$(FILES)
	$(CC) -o 32x16hanzi $(LFLAGS) @32x16hanzi.lk   -lcatmega
main.o: E:/32x16ºº×Ö/32x16led.h D:/ICC/include/iom16v.h D:/ICC/include/macros.h E:/32x16ºº×Ö/font.h
main.o:	E:\32x16ºº×Ö\main.c
	$(CC) -c $(CFLAGS) E:\32x16ºº×Ö\main.c
