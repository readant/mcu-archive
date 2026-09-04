NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             lit   0054   0E3C =   3644. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0054  _ASC_MSK
       0054  __lit_start
       0654  _gb_16
       0E90  __lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   0E90   00A8 =    168. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0E90  __idata_start
       0F38  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   0F38   0492 =   1170. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       079C  __text_start
       079C  __start
       07BC  _exit
       07BD  _led_initial
       07EF  _print_char
       08FC  _move_to_buffer
       0939  _scan_led
       0960  _delay
       0970  _main
       098A  pop_gset2
       098D  pop_gset5
       0990  push_gset1
       0993  pop_gset1
       0994  pop
       09A7  push_gset2
       09AB  push_gset3
       09AF  push_gset4
       09B3  push_gset5
       09B7  push_lset
       09CE  pop_lset
       09E5  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0060   00A8 =    168. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0060  __data_start
       0108  __data_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             bss   0108   0041 =     65. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0108  _display_buffer
       0108  __bss_start
       0148  _dir_f
       0149  __bss_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0024 =     36. bytes (abs,ovr,rom)

Files Linked      [ module(s) ]

D:\ICC\lib\crtatmega.o	[ crtatmega.s ]
main.o	[ main.c ]
<library>	[ gpop2.s, gpop5.s, gpush.s, gpush2.s, gpush3.s, gpush4.s, gpush5.s, lpush.s ]

User Global Definitions

ram_end = 0x45f
hwstk_size = 0x10

User Base Address Definitions

func_lit = 0x54
data = 0x60
eeprom:1.512

