' This version of space.bas works only with X11-Basic V.1.16 and later
'
a%=6
DIM pkx(200+1),pky(200+1),r%(200+1),v%(200+1)
ARRAYFILL pkx(),320
ARRAYFILL pky(),200
FOR i%=0 TO 200
  r%(i%)=RANDOM(1000)
  v%(i%)=RANDOM(20)+3
NEXT i%
weiss=COLOR_RGB(1,1,1)
t=TIMER
zc=0

' output of inline.bas for X11-Basic 04.04.2011
' /home/hoffmann/ttt/bas/p01277.bmp 4218 Bytes. (compressed: 1306 Bytes, 30%)
p01277$=""
p01277$=p01277$+"2/S4S&HVJ425aXS\:C>aN<VC-%\'YJ$AXY\U,Q7FWcR8:cDV.B%H.4=]XQU+;+N_"
p01277$=p01277$+"%Z)c;b$Z13-GA$C/\U^RI7@=ICa<PMMF3BWC[U3C[`I1GUHaO%+J/K=YCb1U:+BQ"
p01277$=p01277$+"2a5K=W4C<)HFSS>A3(,B@(5\EQ6Ua_IA-\D^)9B_412YE(=cAI'UZ5G\=Gc01<[4"
p01277$=p01277$+"ASW8GB@aUH3GG*2GYJ+AFIa(W0?@:.0V6-/X67=&JQ(;_A*C-+OUS0Q>])EHW.Bb"
p01277$=p01277$+"N=UM/)a0`=YP;VQ:]0UG^<8U,H>U?I:(@@]CRDR9M/3P/ALG,/H[/N90&ZBNQ[V6"
p01277$=p01277$+"@?B=>a4M\D59:cHC0_*J`J2Ya3-43H12SIC1TMb],&NB1*cV5J=YU+-`\KL47JU&"
p01277$=p01277$+"c[Z8WH[[Y0U$6]b@=24P/Z+Q0Y+XDPN=DSNN+$9,6*43?^6_?]*1KG3$0I^-BV.G"
p01277$=p01277$+"//V17(I`P^2Z1\L4:^N64DR@EN1VMN&XY-c/@&QI:;^aBP,4Q.OT/MTEUX/=DJ`?"
p01277$=p01277$+"*QNYF^a/IEN,R;PS&KZH^\=7?0%+aTK+D?7_;_JO(,Q(>DDbJcQ@GN3VJH.PQKGc"
p01277$=p01277$+"83,[C(%CUX)/Q+H4N<3UHJ[,M^9N&SUZ_WV(U'ND\W%LYK11I[77c^XLRJRZJ8=-"
p01277$=p01277$+"W4YB8YP9I:_\.>.DL&I8FZYO_/O_:/AW%`QMIS@3[VCLAJNT'Z8)c3EOT/_^,&4J"
p01277$=p01277$+"F-PG;Ba9Hc1SOQX6F6DD0TOIET%PcR<6a7BXMY;_DcZ<9PXKAZ;<&]*U/@c7$J&*"
p01277$=p01277$+"\/**]*^=9+=G%N3>]>>T/5_E,a+L//W,FM2O?bZ8^8JA*$+S?RWV$DT-AM=BFXU8"
p01277$=p01277$+"0=25?/%8b%4b?$3PJC13D)[HU8BE>3C(8X?X^FbQS*9\'4,0<>\XG[F6c0I[K%=1"
p01277$=p01277$+"1cA'D-'/LWU^/O4)c/5E`M<N\aKF+7I<4^/S+Xc8Y(I^)Eb<W\R&9>29N6/8_aKL"
p01277$=p01277$+"L<cI^1D9]&>U<P*1PC'C7S@B`WFF+$NFG]<_:UEQG'?K0YCKc:1IVKEHI[b%F<(Z"
p01277$=p01277$+"0K158F>>G;;7I%?>%Z_42Y]9C`SS[I\*1P^S$_`\&SWY^$FJB63H@MW(9>Sb,P*Q"
p01277$=p01277$+"Y@:a``8N5\*2'UTS5)`3Z-b6$N8VYT<4UBF^b^]aF^PC28<LH6bA8^E[cHc4NO%]"
p01277$=p01277$+"[YO^(U+P67,.[K>PJ1'4WYbQ_(R4UF*7\/<.DOOLHI*AYG8&Y[K6(=G'D3(VK.$@"
p01277$=p01277$+"<<$^_7;c-.8<4&PE5KM=@@,DS`G<1a]b/?bK@G98`INP1KT/R7@A3WSQJ]TH8_:2"
p01277$=p01277$+";B._MT^5H+P+2>ZUL3C0Z;SDC<I:.EXY1.C8DT@ULS1L:9F/b1=K,\*<&5?6ZQ.B"
p01277$=p01277$+"DGD4NJ=*)SET]SA:/3D1+Q'TT40KRWN??-Y,-`EcX:ZQ7&BY$J$92GS5O9/]R<2M"
p01277$=p01277$+"AaXMSHR[<J*YE4TQ/$M_\Y9=I?aON>UHV3M]?0D5,%6/UALV)6I+C=`.]R@JBDMW"
p01277$=p01277$+"8/W&[H(<]&>``Ob/@X\3CVK,/W9R?G-[-43:>>bc)b%S+43.K<5*?+X*S'1'9D(C"
p01277$=p01277$+"P+K\b31I049DO4T&:_<*+\O*L0_OLCNQIHU`]^\'Z&1SH7_4B;\`P_85MY`NLTA'"
p01277$=p01277$+"1.cP4[O8RGXND^$J\-M'&;M3[UQ;[/AKD_cSM_FZR8-+/`Yb?,](Y.I-40Zca%OS"
p01277$=p01277$+"V0>X*c:c.-;X,T2^M4('VUPF?%P=$[>:ZF:M8Y<=7bE4$**^CU/[@`b[>J3WF'/\"
p01277$=p01277$+"%'%Z.LN>`D0@,$$$"
p01277_bmp$=UNCOMPRESS$(INLINE$(p01277$))

COLOR GET_COLOR(65535,65535,20000)
CLEARW
TEXT 200,30," S P A C E      von Markus Hoffmann "
c%=0
DO
  EXIT IF MOUSEK
  COLOR schwarz
  PBOX 0,0,640,400
  COLOR weiss
  FOR c%=0 TO a%-1
    z0=360*r%(c%)/1000
    v1=v%(c%)*(SQR((pkx(c%)-320)^2+(pky(c%)-200)^2)+0.1)*0.01
    ADD pkx(c%),SIN(rad(z0))*v1
    ADD pky(c%),COS(rad(z0))*v1
    PUT pkx(c%),pky(c%),p01277_bmp$,v%(c%)*0.1
    IF pkx(c%)>639 OR pkx(c%)<0 OR pky(c%)>399 OR pky(c%)<0
      r%(c%)=RANDOM(1000)
      pkx(c%)=320
      pky(c%)=200
      z0=2*PI*r%(c%)/1000
      z1=SIN(z0)
      z2=COS(z0)
      v1=RANDOM(100)
      ADD pkx(c%),z1*v1
      ADD pky(c%),z2*v1
      v%(c%)=RANDOM(5)+3
    ENDIF
  NEXT c%
  SHOWPAGE
  PAUSE 0.05
  IF zc=40
    PRINT AT(1,1);TIMER-t
    IF TIMER-t>3
      DEC a%
      PRINT "-"'a%
    ELSE
      INC a%
      PRINT "+"'a%
    ENDIF
    t=TIMER
    CLR zc
  ENDIF
  INC zc
LOOP
QUIT
