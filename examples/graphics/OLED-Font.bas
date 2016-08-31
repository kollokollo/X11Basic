' OLED-CLOCK.bas (c) Markus Hoffmann 2011
'
'

' font5x7.fnt 1280 Bytes. (compressed: 1013 Bytes, 79%)
font5x7$=""
font5x7$=font5x7$+"MBND6N\D=H3G&@%&-'D-%O=_U8:Y/Z>G''C0I4TR7UFF+/A-cVCPD_3P3YK^$S;G"
font5x7$=font5x7$+"%*G[VU1XZc>H;'+Z3NF+a]1A-K9bMW$*bOA+,5/:E,T*.L4]25->2O@D*2_(O*(="
font5x7$=font5x7$+"-EaK<(@>4@^`?M74+\%++<MQN'ZD23[LGRb*POHT51^^THB;35%T9<>0Zc1VW;HY"
font5x7$=font5x7$+"1@;'K1D(AC,(B`A,&X_%JLVb+JLA2I0Y*5/MDbCJ[;Cb\cXY,SOOU7[9`>6-T=c5"
font5x7$=font5x7$+"WJ'2U[5c?NX7XN9Q6$&+\`Y@BJ)>W<$AAZ/V6G=H<G@b+=)W*S`<3%,$Y,'N^3OL"
font5x7$=font5x7$+"USIaOWZM:F[6(3FZ*==U%H=NN'E$\SE=MH03@`aB*'XVQ37./-._3R+^@/94ZD2c"
font5x7$=font5x7$+"D-84aG+)P`G/)&T&NR`V;42b<.3:A\VbBV:(A2^\YI@7NK6+MWFJ=W.MX_UWZT)*"
font5x7$=font5x7$+">Q2)<A:M&05(V?%<Z?Y(L*HcNK,Q`E+[[OZN8X0J&[.M2RL7S`B$SMcP0cRQ`^)X"
font5x7$=font5x7$+">?I/9$347]2=/5%A)EKFKN(=2Q`AC;=J,X/]O3b`*bOU(B48W`AMCW+)4_MNJ389"
font5x7$=font5x7$+"c*-O9JDPKP61/.6?((^LCF`MZIE':3Z+2+/D7?&bCD`(O*UHc5,6[07<G'TLc4B3"
font5x7$=font5x7$+"9Xc/$W?H31LG5IA1+_FAJJ9L+'Y6BW*VSNQY)E_@%AO9?/9N+`,WU>)<LF&.+&0'"
font5x7$=font5x7$+"]Fc:&`DB;c&FEE`G@&=39<J<%P;&&Q8G=<C7&NVT2O>\<cPQ/7)N%@APQ6a%:,Uc"
font5x7$=font5x7$+"0aEL%G?+cS6F`>U1=;>?CI2b0MX%^^'KJY'BDF.VA`8?U:$-_^$W4-9L;('O\+0'"
font5x7$=font5x7$+"U@[X_(UbL+I\WQ;H2'5`:',TRN8]<'-K_-I-PI)VL->.-FHH..D[LXTG///4C)7+"
font5x7$=font5x7$+"WQB&@U0UWQ51K_)-KYZL1V$.)QE\PEbcE]c._>5K9KNY&9P?<4?*<4+'')c3X*\?"
font5x7$=font5x7$+"27]4]0T_30*\$.<L45Tc8C6ZS;aObUS=F@38$3*Gb9Q8'P]4X>%9>Ic*B/V5'J(R"
font5x7$=font5x7$+">Aa-W\=ZaT1a3QX,SDNa9F$7,EaVVW:]=ZaUD[a0,BZJ.KG-N<@ODX)FO<QB=H(A"
font5x7$=font5x7$+"TL^0,-`bR)\>cKSI\CD@VFcW<+.]>U4?8VD'D:A(:_^`b(E3OV<5O*@PW((7L2Ta"
font5x7$=font5x7$+";25?-_P2-5\1);G5)UN;K\FRR$FPO/b[;>WW@.4HO(O?ZMI41-T;2^RTMaG'50KI"
font5x7$=font5x7$+"Q'9??,@\Pc$?$2B1K-:.F9Z7R4H<7M)F(,;L90)(DR;G1;'6W\.':TCMVOL\:Q^A"
font5x7$=font5x7$+"/X0\cNY0a)6:*3+X991`<*OFa@39cKUO4BYc7RK$,=1ScT1G?4**AD;M6^I$DL,2"
font5x7$=font5x7$+"BA\JDD,$"
font5x7_fnt$=UNCOMPRESS$(INLINE$(font5x7$))
font$=font5x7_fnt$
CLEARW
@putstring(10,50,"OLED-Font by MH 2011")
@putstring2(10,250,"This is a smaller variant",2)
' color get_color(30000,30000,30000)
' box 90,90,250,130
lauf$=REPLACE$(system$("cat /proc/cpuinfo"),CHR$(10)," | ")

CLR count
DO
  d$=time$
  FOR g=0 TO LEN(d$)-1
    @putchar(100+23*g,100,peek(varptr(d$)+g))
  NEXT g
  'print mid$(lauf$,count,60)
  @putstring2(10,270,mid$(lauf$,count,60),2)
  INC count
  IF count>len(lauf$)
    CLR count
  ENDIF
  PAUSE 0.2
LOOP
~inp(-2)
QUIT

PROCEDURE putstring(x,y,s$)
  LOCAL i
  FOR i=0 TO LEN(s$)-1
    @putchar(x+23*i,y,peek(varptr(s$)+i))
  NEXT i
RETURN
PROCEDURE putstring2(x,y,s$,s)
  LOCAL i
  FOR i=0 TO LEN(s$)-1
    @putchar2(x+6*i*s,y,peek(varptr(s$)+i),s)
  NEXT i
RETURN

PROCEDURE putchar(x,y,c)
  LOCAL a,a1,i,j
  bg=GET_COLOR(0,0,0)
  off=GET_COLOR(10000,15000,15000)
  shine=GET_COLOR(15000,30000,30000)
  fg=GET_COLOR(0,60000,65535)
  a=VARPTR(font$)+5*c
  COLOR bg
  PBOX x-1,y-1,x+4*5+1,y+4*7+1
  FOR j=0 TO 4
    a1=PEEK(a+j)
    FOR i=0 TO 6
      IF btst(a1,i)
        COLOR shine
        BOX x+j*4-1,y+i*4-1,x+j*4+3,y+i*4+3
        COLOR fg
      ELSE
        COLOR off
      ENDIF
      PBOX x+j*4,y+i*4,x+j*4+2,y+i*4+2
    NEXT i
  NEXT j
  VSYNC
RETURN
PROCEDURE putchar2(x,y,c,s)
  LOCAL a,a1,i,j
  bg=GET_COLOR(0,0,0)
  off=GET_COLOR(10000,15000,15000)
  shine=GET_COLOR(15000,30000,30000)
  fg=GET_COLOR(0,60000,65535)
  a=VARPTR(font$)+5*c
  COLOR bg
  PBOX x-1,y-1,x+5*s+1,y+7*s+1
  FOR j=0 TO 4
    a1=PEEK(a+j)
    FOR i=0 TO 6
      IF btst(a1,i)
        IF s>2
          COLOR shine
          CIRCLE x+j*s+2,y+i*s+2,s
        ENDIF
        COLOR fg
      ELSE
        COLOR off
      ENDIF
      PCIRCLE x+j*s+2,y+i*s+2,s-1
    NEXT i
  NEXT j

RETURN
PROCEDURE makefont
  font$=SPACE$(5*256)
  i=0
  OPEN "I",#1,"src/5x7.c"
  WHILE not eof(#1)
    LINEINPUT #1,t$
    t$=REPLACE$(t$,","," ")
    t$=TRIM$(t$)
    IF left$(t$)="0"
      POKE varptr(font$)+5*i,VAL(WORD$(t$,1))
      POKE varptr(font$)+5*i+1,VAL(WORD$(t$,2))
      POKE varptr(font$)+5*i+2,VAL(WORD$(t$,3))
      POKE varptr(font$)+5*i+3,VAL(WORD$(t$,4))
      POKE varptr(font$)+5*i+4,VAL(WORD$(t$,5))
      PRINT i,t$
      INC i
      EXIT if i=256
    ENDIF
  WEND
  ' memdump varptr(font$),len(font$)
  BSAVE "5x7.fnt",VARPTR(font$),LEN(font$)
RETURN
