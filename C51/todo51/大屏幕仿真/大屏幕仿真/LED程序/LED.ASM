
ORG       0000H            ;程序执行开始地址
LJMP      START            ;跳至START执行
ORG       0003H            ;外中断0中断入口地址
RETI                       ;中断返回（不开中断）
ORG       000BH            ;定时器T0中断入口地址
LJMP      INTT0            ;进入定时器中断程序
ORG       0013H            ;外中断1中断入口地址
RETI                       ;中断返回（不开中断）
ORG       001BH            ;定时器T1中断入口地址
LJMP      INTT1            ;中断返回（不开中断）
ORG       0023H            ;串行口中断入口地址
RETI                       ;中断返回（不开中断）
ORG       002BH            ;定时器T2中断入口地址
RETI                       ;中断返回（不开中断）
ORG       0100H

START: MOV TMOD,#21H
       MOV TL1,#3CH
       MOV TH1,#0B0H
       MOV TH0,#0B1H
       MOV TL0,#0E0H
       ;MOV SP,#70H
       SETB EA
       SETB  ET0
       SETB  TR0
       ;SETB ET1

       ;SETB  TR1

START0: LCALL XSKJ           ;调显示控制
        ;LCALL TIMEXS
       SJMP START0
;进入定时器0中断程序
INTT0:  MOV TH0,#0B1H
        MOV TL0,#0E0H
        PUSH ACC
        PUSH PSW
        PUSH DPL
        PUSH DPH
        SETB RS0               ;选择工作寄存器1

        LCALL DSP            ;扫描显示

        ;时间计数器

        POP DPH
        POP DPL
        POP PSW
        POP ACC
        RETI
INTT1:  MOV TH1,#3CH
        MOV TL1,#0B0H
        PUSH ACC
        PUSH PSW
        PUSH DPL
        PUSH DPH
        SETB RS1

        DJNZ  6EH,INTT10
        ;LCALL GET1302
        MOV 6EH,#10
INTT10:
        CLR RS1
        POP DPH
        POP DPL
        POP PSW
        POP ACC
        RETI



;显示控制

XSKJ:   MOV R0,#0A0H         ;直接显示"欢迎"
        MOV DPTR,#TAB1
        LCALL ZJXS
        MOV R0,#0A2H
        MOV DPTR,#TAB2
        LCALL ZJXS
        ;LJMP $
XSKJ0:  MOV 60H,#2
        LCALL DELY    ;延时1S


        MOV R7,#10H              ;计数


XSKJ1:  LCALL ZBPZY               ;左半屏左移

        LCALL YBPYY               ;右半屏右移

        MOV 60H,#1
        LCALL DELY
        DJNZ R7,XSKJ1
        ;MOV 60H,#1
        ;LCALL DELY
        ;MOV 6EH,#10H
XSKJ2:  ;LCALL  ZPSY             ;整屏上移
        ;MOV 60H,#1
        ;LCALL DELY
        ;DJNZ 6EH,XSKJ2
         LCALL TIMEXS
        MOV R7,#0AH             ;左移显示 郑州加工业学院欢迎您
        MOV DPTR,#TAB6
        LCALL ZYXSDZ
        MOV R7,#10H
XSKJ3:    LCALL ZPSY

        MOV 60H,#1
        LCALL DELY
        DJNZ R7,XSKJ3

        RET
;=====================================================
TIMEXS: LCALL Get1302
        LCALL FLBCD


        LCALL FLBCD
        MOV SHUHC,41H
        LCALL SHUXJ
        MOV R0,#0A2H
        LCALL ZJXS8

        MOV SHUHC,40H

        LCALL SHUXJ
        MOV R0,#0A3H
        LCALL ZJXS8

        MOV SHUHC,43H
        LCALL SHUXJ
        MOV R0,#0A0H
        LCALL ZJXS8

        MOV SHUHC,42H

        LCALL SHUXJ
        MOV R0,#0A1H
        LCALL ZJXS8



        RET

;扫描显示
DSP:

        MOV R0,#0A0H
        MOV R1,#0FEH

DSP0:   MOV DPTR,#7FFFH
        MOV A,R1
        MOVX @DPTR,A
        MOV DPTR,#0BFFFH
        MOV A,#0FFH
        MOVX @DPTR,A

        MOV DPTR,#0DFFFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        MOV DPTR,#0EFFFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        MOV DPTR,#0F7FFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        MOV DPTR,#0FBFFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        LCALL DELAY

        MOV A,R1
        RL A
        MOV R1,A
        CJNE R0,#0C0H,DSP0

        MOV R1,#0FEH
DSP1:   MOV DPTR,#7FFFH
        MOV A,#0FFH
        MOVX @DPTR,A
        MOV DPTR,#0BFFFH
        MOV A,R1
        MOVX @DPTR,A

        MOV DPTR,#0DFFFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        MOV DPTR,#0EFFFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        MOV DPTR,#0F7FFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        MOV DPTR,#0FBFFH
        MOV A,@R0
        MOVX @DPTR,A
        INC R0

        LCALL DELAY

        MOV A,R1
        RL A
        MOV R1,A
        CJNE R0,#0E0H,DSP1
        RET


;送数右缓冲
;调用前先给R3,DPTR赋值
XSYHC:  MOV R1,#70H
XSYHC0: MOV A,R3
        MOVC A,@A+DPTR
        MOV @R1,A
        INC R1
        INC R3
        INC R3
        CJNE R1,#80H,XSYHC0
        RET


;送数左缓冲
;调用前先给R3,DPTR赋值
XSZHC:  MOV R1,#80H
XSZHC0: MOV A,R3
        MOVC A,@A+DPTR
        MOV @R1,A
        INC R1
        INC R3
        INC R3
        CJNE R1,#90H,XSZHC0
        RET


;直接显示
;调用前先给R0,DPTR赋值
ZJXS:  MOV R3,#00H

ZJXS0: MOV A,R3
       MOVC A,@A+DPTR
       MOV @R0,A
       INC R0
       INC R3
       MOV A,R3
       MOVC A,@A+DPTR
       MOV @R0,A
       INC R0
       INC R0
       INC R0
       INC R3
       CJNE R3,#20H,ZJXS0
       RET


;整屏左移
ZPZY:    MOV R0,#0DFH
         MOV R1,#7FH
         MOV R2,#04H

ZPZY1:   CLR C
         MOV A,@R1
         RLC A
         MOV @R1,A

ZPZY0:   MOV A,@R0
         RLC A
         MOV @R0,A
         DEC R0
         DJNZ R2,ZPZY0
         DEC R1
         MOV R2,#04H
         CJNE R0,#9FH,ZPZY1
         RET


;整屏右移
ZPYY:    MOV R0,#0A0H
         MOV R1,#80H
         MOV R2,#04H

ZPYY1:   CLR C
         MOV A,@R1
         RRC A
         MOV @R1,A

ZPYY0:   MOV A,@R0
         RRC A
         MOV @R0,A
         INC R0
         DJNZ R2,ZPYY0
         INC R1
         MOV R2,#04H
         CJNE R0,#0E0H,ZPYY1
         RET


;整屏上移
ZPSY:    MOV R0,#0A0H
         MOV R1,#0A4H

ZPSY0:   MOV A,@R1
         MOV @R0,A
         INC R0
         INC R1
         CJNE R0,#0E0H,ZPSY0
         RET


;整屏下移
ZPXY:    MOV R0,#0DFH
         MOV R1,#0DBH

ZPXY0:   MOV A,@R1
         MOV @R0,A
         INC R0
         INC R1
         CJNE R0,#9FH,ZPXY0
         RET

;半屏上移
;调用前先给R0,R1赋值
BPSY:    MOV R3,#10H
         MOV A,@R1
         MOV @R0,A
         INC R0
         INC R1
         MOV A,@R1
         MOV @R0,A
         INC R0
         INC R1
         INC R0
         INC R1
         INC R0
         INC R1
         DJNZ R3,BPSY
         RET

;半屏下移
;调用前先给R0,R1赋值
BPXY:    MOV R3,#10H
         MOV A,@R1
         MOV @R0,A
         DEC R0
         DEC R1
         MOV A,@R1
         MOV @R0,A
         DEC R0
         DEC R1
         DEC R0
         DEC R1
         DEC R0
         DEC R1
         DJNZ R3,BPXY
         RET

;半半屏上移
;调用前先给R0,R1赋值
BBPSY:   MOV R3,#10H
         MOV A,@R1
         MOV @R0,A
         INC R0
         INC R1
         INC R0
         INC R1
         INC R0
         INC R1
         INC R0
         INC R1
         DJNZ R3,BBPSY
         RET

;半半屏下移
;调用前先给R0,R1赋值
BBPXY:   MOV R3,#10H
         MOV A,@R1
         MOV @R0,A
         DEC R0
         DEC R1
         DEC R0
         DEC R1
         DEC R0
         DEC R1
         DEC R0
         DEC R1
         DJNZ R3,BBPXY
         RET

;半屏左移
;调用前先给R0,R1赋值
BPZY:    MOV R3,#08H
         MOV R2,#04H

BPZY1:   CLR C
         MOV A,@R1
         RLC A
         MOV @R1,A

BPZY0:   MOV A,@R0
         RLC A
         MOV @R0,A
         DEC R0
         DJNZ R2,BPZY0
         DEC R1
         MOV R2,#04H
         DJNZ R3,BPZY1
         RET


;半屏右移
;调用前先给R0,R1赋值
BPYY:    MOV R3,#08H
         MOV R2,#04H

BPYY1:   CLR C
         MOV A,@R1
         RRC A
         MOV @R1,A

BPYY0:   MOV A,@R0
         RRC A
         MOV @R0,A
         INC R0
         DJNZ R2,BPYY0
         INC R1
         MOV R2,#04H
         DJNZ R3,BPYY1
         RET


;送数下缓冲一
;调用前先给R0,R2,DPTR 赋值
XSSXHCY:  MOV A,R2
         MOVC A,@A+DPTR
         MOV @R0,A
         INC R0
         INC R2
         MOVC A,@A+DPTR
         MOV @R0,A
         RET

;送数下缓冲二
;调用前先给R1,R3,DPTR 赋值
XSSXHCE:  MOV A,R3
         MOVC A,@A+DPTR
         MOV @R1,A
         INC R1
         INC R3
         MOVC A,@A+DPTR
         MOV @R1,A
         RET


;左半屏左移

ZBPZY:  MOV R0,#0DDH
        MOV R1,#8FH

ZBPZY0: MOV A,@R1
        CLR C
        RLC A
        MOV @R1,A
        MOV A,@R0
        RLC A
        MOV @R0,A
        DEC R0
        MOV A,@R0
        RLC A
        MOV @R0,A
        DEC R0
        DEC R0
        DEC R0
        DEC R1
        CJNE R1,#7FH,ZBPZY0
        RET

;左半屏右移

ZBPYY:  MOV R0,#0A0H
        MOV R1,#80H

ZBPYY0: MOV A,@R1
        CLR C
        RRC A
        MOV @R1,A
        MOV A,@R0
        RRC A
        MOV @R0,A
        INC R0
        MOV A,@R0
        RRC A
        MOV @R0,A
        INC R0
        INC R0
        INC R0
        INC R1
        CJNE R1,#90H,ZBPYY0
        RET

;右半屏左移

YBPZY:  MOV R0,#0DFH
        MOV R1,#7FH

YBPZY0: MOV A,@R1
        CLR C
        RLC A
        MOV @R1,A
        MOV A,@R0
        RLC A
        MOV @R0,A
        DEC R0
        MOV A,@R0
        RLC A
        MOV @R0,A
        DEC R0
        DEC R0
        DEC R0
        DEC R1
        CJNE R1,#6FH,YBPZY0
        RET

;右半屏右移

YBPYY:  MOV R0,#0A2H
        MOV R1,#70H

YBPYY0: MOV A,@R1
        CLR C
        RRC A
        MOV @R1,A
        MOV A,@R0
        RRC A
        MOV @R0,A
        INC R0
        MOV A,@R0
        RRC A
        MOV @R0,A
        INC R0
        INC R0
        INC R0
        INC R1
        CJNE R1,#80H,YBPYY0
        RET

;左移显示多字
;调用前先给DPTR,R7赋值
ZYXSDZ: MOV R6,#08H
        MOV R3,#00H
        LCALL XSYHC

ZYXSDZ0:LCALL ZPZY
        MOV 60H,#1
        LCALL DELY
        DJNZ R6,ZYXSDZ0
        MOV R6,#08H
        MOV R3,#01H
        LCALL XSYHC

ZYXSDZ1:LCALL ZPZY
        MOV 60H,#1
        LCALL DELY
        DJNZ R6,ZYXSDZ1
        MOV A,DPL
        ADD A,#20H
        MOV DPL,A
        MOV A,DPH
        ADDC A,#00H
        MOV DPH,A
        DJNZ R7,ZYXSDZ
        RET

;取反
CF:     MOV R0,#0A0H
CF1:    MOV A,@R0
        CPL A
        MOV @R0,A
        INC R0
        CJNE R0,#0E0H,CF1
        RET

;清缓冲
;调用前先给 R1,R4赋值
CHC:    MOV A,@R1
        CLR A
        MOV @R1,A
        INC R1
        DJNZ R4,CHC
        RET

;置缓冲
;调用前先给 R1,R4赋值
ZHC:    MOV A,@R1
        CLR A
        CPL A
        MOV @R1,A
        INC R1
        DJNZ R4,ZHC
        RET

;或显示
;调用前先给 DPTR,R0赋值
HXS:   MOV R3,#00H
HXS0:  MOV A,R3
       MOVC A,@A+DPTR
       ORL A,@R0
       MOV @R0,A
       INC R0
       INC R3
       MOVC A,@A+DPTR
       ORL A,@R0
       MOV @R0,A
       INC R0
       INC R0
       INC R0
       INC R3
       CJNE R3,#20H,HXS0
       RET

;延时（100MS）
;调用前先给 60H赋值
DELY:  MOV 61H,#30
DELY0: MOV 62H,#250
       DJNZ 62H,$
       DJNZ 61H,DELY0
       DJNZ 60H,DELY
       RET
;延时（1MS）
DELAY:      MOV 63H,#2
DELAY0:     MOV 64H,#250
            DJNZ 64H,$
            DJNZ 63H,DELAY0
            RET
;======================================================================
;8*16直接显示
;调用前先给R0,DPTR赋值
        SHUHC EQU 50H      ;数缓冲
SHUXJ:  MOV A,SHUHC
        CJNE A,#0,SHUXJ1
        MOV DPTR,#TABSHU0
        LJMP SHUXJOUT
SHUXJ1: CJNE A,#1,SHUXJ2
        MOV DPTR,#TABSHU1
        LJMP SHUXJOUT
SHUXJ2: CJNE A,#2,SHUXJ3
        MOV DPTR,#TABSHU2
        LJMP SHUXJOUT
SHUXJ3: CJNE A,#3,SHUXJ4
        MOV DPTR,#TABSHU3
        LJMP SHUXJOUT
SHUXJ4: CJNE A,#4,SHUXJ5
        MOV DPTR,#TABSHU4
        LJMP SHUXJOUT
SHUXJ5: CJNE A,#5,SHUXJ6
        MOV DPTR,#TABSHU5
        LJMP SHUXJOUT
SHUXJ6: CJNE A,#6,SHUXJ7
        MOV DPTR,#TABSHU6
        LJMP SHUXJOUT
SHUXJ7: CJNE A,#7,SHUXJ8
        MOV DPTR,#TABSHU7
        LJMP SHUXJOUT
SHUXJ8: CJNE A,#8,SHUXJ9
        MOV DPTR,#TABSHU8
        LJMP SHUXJOUT
SHUXJ9: CJNE A,#9,SHUXJ10
        MOV DPTR,#TABSHU9
        LJMP SHUXJOUT
SHUXJ10: MOV DPTR,#TABSHU10

SHUXJOUT:

RET
ZJXS8:  MOV R3,#00H

ZJXS80: MOV A,R3
       MOVC A,@A+DPTR
       MOV @R0,A
       INC R0
       INC R0
       INC R0
       INC R0
       INC R3
       CJNE R3,#10H,ZJXS80
       RET
; 将时间变成分离BCD码40H-------
FLBCD: MOV R0,#SECOND
       MOV R1,#40H
       MOV R7,#7
FLBCD1:  MOV A,@R0

       ANL A,#0FH
       MOV @R1,A
       MOV A,@R0
       SWAP A
       ANL A,#0FH
       INC R1
       MOV @R1,A
       INC R0
       INC R1
       DJNZ R7,FLBCD1
       RET



;*********************************************************************/
SECOND EQU 30H
T_CLK Bit P1.4;实时时钟时钟线引脚
T_IO  Bit P1.5;实时时钟数据线引脚
T_RST Bit P1.6;实时时钟复位线引脚

;**********************************************************
;子程序名：Set1302
;功    能：设置DS1302 初始时间,并启动计时。
;说    明：
;调    用：RTInputByte;入口参数：初始时间在:Second,Minute,Hour,Day,Month,Week.YearL(地址连续)
;出口参数：无
;影响资源：A B R0 R1 R4 R7

;**********************************************************
Set1302:
          CLR    T_RST
          CLR    T_CLK
          SETB   T_RST
          MOV    B,     #8EH        ;控制寄存器
          LCALL  RTInputByte
          MOV    B,     #00H        ;写操作前WP=0
          LCALL  RTInputByte
          SETB   T_CLK
          CLR    T_RST
          MOV    R0,    #Second
          MOV    R7,    #7        ;秒 分 时 日 月 星期 年
          MOV    R1,    #80H      ;秒写地址
S13021:
          CLR    T_RST
          CLR    T_CLK
          SETB   T_RST
          MOV    B,     R1        ;写秒 分 时 日 月 星期 年 地址
          LCALL  RTInputByte
          MOV    A,     @R0       ;写秒数据
          MOV    B,     A
          LCALL  RTInputByte
          INC    R0
          INC    R1
          INC    R1
          SETB   T_CLK
          CLR    T_RST
          DJNZ   R7,    S13021
          CLR    T_RST
          CLR    T_CLK
          SETB   T_RST
          MOV    B,     #8EH        ;控制寄存器
          LCALL  RTInputByte
          MOV    B,     #80H        ;控制,WP=1,写保护
          LCALL  RTInputByte
          SETB   T_CLK
          CLR    T_RST
          RET
          ;**********************************************************
          ;子程序名：Get1302;功    能：从DS1302 读时间
          ;说    明：
          ;调    用：RTInputByte,RTOutputByte
          ;入口参数：时间保存在:Second,Minute,Hour,Day,Month,Week.YearL
          ;出口参数：无
          ;影响资源：A B R0 R1 R4 R7

          ;**********************************************************
Get1302:
          MOV    R0,    #Second;
          MOV    R7,    #7
          MOV    R1,    #81H       ;秒地址
G13021:
          CLR    T_RST
          CLR    T_CLK
          SETB   T_RST
          MOV    B,     R1        ;秒 分 时 日 月 星期 年 地址
          LCALL  RTInputByte
          LCALL  RTOutputByte
          MOV    @R0,   A         ;秒
          INC    R0
          INC    R1
          INC    R1
          SETB   T_CLK
          CLR    T_RST
          DJNZ   R7,    G13021
          RET
          ;**********************************************************
          ;功    能：写1302一字节  （内部子程序）
          ;**********************************************************
RTInputByte:
            MOV    R4,    #8
Inbit1:
          MOV    A,     B
          RRC    A
          MOV    B,     A
          MOV    T_IO,  C
          SETB   T_CLK
          CLR    T_CLK
          DJNZ   R4,   Inbit1
          RET
          ;**********************************************************
          ;功    能：读1302一字节  （内部子程序）
          ;**********************************************************
RTOutputByte:
          MOV    R4,    #8
Outbit1:  MOV    C,     T_IO
          RRC    A
          SETB   T_CLK
          CLR    T_CLK
          DJNZ   R4,    Outbit1
          RET
          ;///////////////////////////////////////////////////////////
TABSHU0:
;--  文字:  0  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,018H,024H,042H,042H,042H,042H,042H,042H,042H,024H,018H,000H,000H,
TABSHU1:
;--  文字:  1  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,010H,070H,010H,010H,010H,010H,010H,010H,010H,010H,07CH,000H,000H,
TABSHU2:
;--  文字:  2  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,03CH,042H,042H,042H,004H,004H,008H,010H,020H,042H,07EH,000H,000H,
TABSHU3:
;--  文字:  3  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,03CH,042H,042H,004H,018H,004H,002H,002H,042H,044H,038H,000H,000H,
TABSHU4:
;--  文字:  4  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,004H,00CH,014H,024H,024H,044H,044H,07EH,004H,004H,01EH,000H,000H,
TABSHU5:
;--  文字:  5  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,07EH,040H,040H,040H,058H,064H,002H,002H,042H,044H,038H,000H,000H,
TABSHU6:
;--  文字:  6  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,01CH,024H,040H,040H,058H,064H,042H,042H,042H,024H,018H,000H,000H,
TABSHU7:
;--  文字:  7  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,07EH,044H,044H,008H,008H,010H,010H,010H,010H,010H,010H,000H,000H,
TABSHU8:
;--  文字:  8  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,03CH,042H,042H,042H,024H,018H,024H,042H,042H,042H,03CH,000H,000H,
TABSHU9:
;--  文字:  9  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,018H,024H,042H,042H,042H,026H,01AH,002H,002H,024H,038H,000H,000H,
TABSHU10:
;--  文字:  E  --
;--  宋体12;  此字体下对应的点阵为：宽x高=8x16   --
DB  000H,000H,000H,0FCH,042H,048H,048H,078H,048H,048H,040H,042H,042H,0FCH,000H,000H,









TAB1:   DB  00H,80H,00H,80H,0FCH,80H,04H,0FCH
        DB  45H,04H,46H,48H,28H,40H,28H,40H
        DB  10H,40H,28H,40H,24H,0A0H,44H,0A0H
        DB  81H,10H,01H,08H,02H,0EH,0CH,04H

TAB2:   DB  00H,00H,41H,84H,26H,7EH,14H,44H
        DB  04H,44H,04H,44H,0F4H,44H,14H,0C4H
        DB  15H,44H,16H,54H,14H,48H,10H,40H
        DB  10H,40H,28H,46H,47H,0FCH,00H,00H

TAB3:   DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH
        DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH
        DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH
        DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH

TAB4:   DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH
        DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH
        DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH
        DB  0FFH,0FFH,0FFH,0FFH,0FFH, 0FFH,0FFH,0FFH

TAB5:   DB  10H,00H,11H,0FCH,10H,04H,10H,08H
        DB  0FCH,10H,24H,20H,24H,24H,27H,0FEH
        DB  24H,20H,44H,20H,28H,20H,10H,20H
        DB  28H,20H,44H,20H,84H,0A0H,00H,40H

TAB6:   DB  41H,00H,22H,7CH,14H,44H,7FH,44H             ;郑
        DB    08H,48H,08H,48H,09H,50H,0FFH,0C8H
        DB    08H,44H,08H,42H,08H,42H,14H,62H
        DB    12H,54H,22H,48H,40H,40H,00H,40H

TAB7:   DB   10H,04H,10H,84H,10H,84H,10H,84H            ;州
        DB   54H,0A4H,52H,94H,52H,94H,90H,84H
        DB   10H,84H,10H,84H,10H,84H,10H,84H
        DB   20H,84H,20H,84H,40H,04H,00H,04H

TAB8:   DB   10H,00H,11H,0FCH,14H,08H,0FEH,10H          ;轻
        DB   20H,38H,28H,0CCH,4BH,02H,7CH,00H
        DB   08H,08H,09H,0FCH,0EH,20H,0F8H,20H
        DB   48H,20H,08H,24H,0BH,0FEH,08H,00H


TAB9:   DB   00H,00H,00H,08H,7FH,0FCH,01H,00H            ;  工
        DB   01H,00H,01H,00H,01H,00H,01H,00H
        DB   01H,00H,01H,00H,01H,00H,01H,00H
        DB   01H,04H,0FFH,0FEH,00H,00H,00H,00H

TAB10:   DB   04H,40H,04H,40H,04H,40H,04H,40H             ;业
        DB   44H,44H,24H,44H,24H,48H,14H,48H
        DB   14H,50H,14H,50H,14H,60H,04H,40H
        DB   04H,40H,04H,44H,0FFH,0FEH,00H,00H

TAB11:   DB   22H,08H,11H,08H,11H,10H,00H,20H             ;学
        DB   7FH,0FEH,40H,02H,80H,04H,1FH,0E0H
        DB   00H,40H,01H,84H,0FFH,0FEH,01H,00H
        DB   01H,00H,01H,00H,05H,00H,02H,00H


TAB12:   DB   00H,80H,78H,40H,4FH,0FEH,54H,02H            ;院
        DB   58H,14H,63H,0F8H,50H,00H,48H,08H
        DB   4FH,0FCH,48H,0A0H,68H,0A0H,50H,0A0H
        DB   41H,22H,41H,22H,42H,1EH,4CH,00H

TAB13:   DB   00H,80H,00H,80H,0FCH,80H,04H,0FCH           ;欢
        DB   45H,04H,46H,48H,28H,40H,28H,40H
        DB   10H,40H,28H,40H,24H,0A0H,44H,0A0H
        DB   81H,10H,01H,08H,02H,0EH,0CH,04H

TAB14:   DB   00H,00H,41H,84H,26H,7EH,14H,44H            ; 迎
        DB   04H,44H,04H,44H,0F4H,44H,14H,0C4H
        DB   15H,44H,16H,54H,14H,48H,10H,40H
        DB   10H,40H,28H,46H,47H,0FCH,00H,00H

TAB15:   DB   09H,00H,09H,00H,13H,0FCH,12H,04H            ;您
        DB   34H,48H,59H,40H,91H,50H,12H,4CH
        DB   14H,44H,11H,40H,10H,80H,02H,00H
        DB   51H,84H,50H,92H,90H,12H,0FH,0F0H


 END
