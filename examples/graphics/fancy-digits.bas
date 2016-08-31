
' Shows fancy digits from images   2011-07-22
'
' The images were taken from here:
' https://sites.google.com/site/dannychouinard/Home/unix-linux-trinkets/fun-things/xdc-x11-digital-clock-using-image-digits

' You can scale everything with this scale factor.
' For TomTom device display this should be 1

scale=1
SIZEW ,320*scale,240*scale
rot=COLOR_RGB(1,0,0)
schwarz=COLOR_RGB(0,0,0)

@init
t$=digit0_bmp$
COLOR schwarz
PBOX 0,0,320*scale,200*scale
COLOR rot
TEXT 0,280*scale,"Due to a bug in earlier versions this demo only works with"
TEXT 0,300*scale,"X11-basic V.1.16 and later."
SHOWPAGE

DO
  @putstring(0,0,STR$(TIMER))
  @putstring(10*scale,50*scale,TIME$)
  @putstring(120*scale,100*scale,DATE$)
  SHOWPAGE
  PAUSE 0.1
LOOP
PAUSE 10
QUIT

PROCEDURE putstring(x,y,s$)
  LOCAL i,g,a$,t$
  FOR g=0 TO LEN(s$)-1
    i=PEEK(VARPTR(s$)+g)-ASC("0")

    IF i<0 OR i>9
      a$="t$=colon_bmp$"
    ELSE
      a$="t$=digit"+STR$(i MOD 10)+"_bmp$"
    ENDIF
    &a$
    PUT x,y,t$,scale
    IF i<0 OR i>9
      ADD x,10*scale
    ELSE
      ADD x,23*scale
    ENDIF
  NEXT g
  SHOWPAGE
RETURN

PROCEDURE init
  ' output of inline.bas for X11-Basic 04.03.2011
  ' digit0.bmp 1334 Bytes. (compressed: 1118 Bytes, 83%)
  digit0$=""
  digit0$=digit0$+"JL:4QH:ZY7bQA^)5PJ-B'Dc-MTXTOC[Ac[7J<1LN78H?ORP3K]+`N\RSZ;38H7C&"
  digit0$=digit0$+"@UW7F5FVS^TL3GcUSU)JW7H=224`_bbM&A6D9bKP?%8-U&*17NT)XD+FB$V+/X51"
  digit0$=digit0$+"$X(H>A+(PI863.D1'2]OQ2?E6PT5(/;P&K/-&.Q$\[=3c%4M(GC^:\4S1.cT;]Z<"
  digit0$=digit0$+"&&PEM<NGAUF_+R56$PZ%.(5$CA_)=9<8K>H(K^CHMVR)-&ZZ$<UB@WA`Q;c]C2^O"
  digit0$=digit0$+"5\4S*:]<S`LLY^'MDG_'REIa$9]S_b>@:UE,\C;YR5'>0]CGGEB^R0])ZT(U8AYX"
  digit0$=digit0$+",K>,^&Y9`T`HV6$49b&][^/S7/MQ['K92>WHJ_[0/=&>bK(TVF\/]Y+E+3bE\PF9"
  digit0$=digit0$+"I?*3A)S7]QM18WVXO6,QK(565,@TSc;`J\(KXI$PK99G-:4_?\)7?7[IGV1X+\H+"
  digit0$=digit0$+"b`ZZ7ZF7FP/cIE><GQEa_V67CRA]`&T:8Z@9?CF2.%,WcbPN(Y<\*35K%-caZIDO"
  digit0$=digit0$+"-'SLG/U4+(b%'<X\[3$X+EICXbC<$*._UabM+1GC92'@Z<]:bU?G_`PN'2K4=,2."
  digit0$=digit0$+">KVZaU@KQH/2Y)Ka:W2.*7XU^IXD//M76F<'R9S8XQG0G>OA11a-2DK`,U@:%55K"
  digit0$=digit0$+"HJ(4.TP?E3L$<T_-AUV3Z+S87;GMB2KcD_Sc]`cCb-[?D5'6&Cb+$S;H)FaO4^Z@"
  digit0$=digit0$+"D@B[6:;Z9)0+.:7L?*W'N8;X*N08.HUJ6<,MS%`\Z8OAW+@60OY-JZ4/@+8N4V=+"
  digit0$=digit0$+"(b55$'J0\,*:7Q_W'`/bH0:&)/Y<[C1OL/F.^*<(BQ5J<(4V8c%N\?$<[C4)AM&&"
  digit0$=digit0$+"$b/L?0:W=NT.G?AWTb:^V5:I-@:JNAE<Z2-a@K;(4$Y?;E&V5:Q[4D2IY@]U=UG:"
  digit0$=digit0$+"XV9HN?0[A^KYD9&b>^Z/HUCO'\5WQSOR-RZ=,bPT8@:A@<;;L<%MBA@[JJI]5*3c"
  digit0$=digit0$+"=I9;YA`IBa:`YJC(TMZ*SQ:+%LAAC-'*NH+05N6)32)R?FHV`QaF?A4bT]/`'aH&"
  digit0$=digit0$+"3=ZEJ?S:[LH_HOA;5JOC:.E?+YU&A*3U@R8H[[1-N9'$K*5G;9$3I^@['6VR;*`K"
  digit0$=digit0$+"(&)6QSK]4&O)9Z&O[VIOU.<65J,7<a,_BZE:RPR=IH^GIMG;V;/TKG,OG&=0S`^Q"
  digit0$=digit0$+"b(\.MJZ3b41%aT;TB%>a1_]/-BD@7@*DAA<D6Gb,OQ20660WMF%?P37YY6A:4\V*"
  digit0$=digit0$+"E`K%ZF9ZW6HKSC?Z/\:<T'))RS?DES(62=9MON8;*E_P$/K<2P14$Ja'IY+LcTX*"
  digit0$=digit0$+",\$Yac2[_-&?&(%\ZPa4]1BD?]2L'UDWLH6>_:^K]V8DM=[)LET=WFBa?D](&SZ7"
  digit0$=digit0$+"RPN6\W..E1DMC)cYN$Pc'8%HaEH`;(N,5NZF6XD(M9?H[E;>2EUP9V%5\6KO;4H4"
  digit0$=digit0$+">Y093GH;(HD<O9W'YC:)M@@b(H\,F'F0L-HX2:QV/b=c%c8+UGb_;@(S%/L&*Y^8"
  digit0$=digit0$+"W8:X]P1.?UO09OVB($4$"
  digit0_bmp$=UNCOMPRESS$(INLINE$(digit0$))
  ' digit1.bmp 1198 Bytes. (compressed: 983 Bytes, 82%)
  digit1$=""
  digit1$=digit1$+"M.R4[-Z\`09R:+KWX&ZI3.QCU1c\99+V(bNS,F(C*Kc09)*H]_D/;LDA;\PER.G="
  digit1$=digit1$+"P@9W%M*c-R34XY:@B;X/DR^+XFZ+ZDHJ2@P$6AaB.M)4<UF&c)-G==N^S>L`BDII"
  digit1$=digit1$+":GBL%DEB=aPAa,c-1X,5%B/I&KU'@MQ&K'KH^X%ZTIa^2.2%<.EJI<]*1H@0XR'M"
  digit1$=digit1$+"_^U6+Y1@GG+?<N0QD3A^FbK'KcZ+--JC/A):]57)PPLZ+O%_X41DYYUV@/\):_A0"
  digit1$=digit1$+"@QN`>`T%aNEBPH%-$B+H+QO$CE53J1aMHRU7%B<S`$a65>G:+K.H5&^4M(?@X>P2"
  digit1$=digit1$+"L@PRLY/CFcc,cNA:==TF4<G;GB^NXS&HM$AA9*;$D2Xab2)XZbR1*,1(WWC_FM>L"
  digit1$=digit1$+"<)bQD+aR</55$.U@B:]OJ(P60,%CKBGC>3Q;5:4$aEYPQ?>3,TZCUX/>_A2-[=Z0"
  digit1$=digit1$+"'6=,ScQK'X[^M'CP0<FRS)aE>YGP^D62&\%NTE$JNB*4I+ZWcCUZ/Z;:(J[EW7X<"
  digit1$=digit1$+"$9Kc3_,^P5UA^LIM,.-25*HKCUXa(;.[UG'W6\*`-<`%D44-4<=\b&^=J\NRB.G>"
  digit1$=digit1$+"VN.+6\ZKE4&^;F5([:H<842@KcOE%XH*P_MJa[L)V4Ma/61PL0XN]F@])2H'XCA("
  digit1$=digit1$+"&JR.>,*'+Q8IY<N`X$IP8c][3:8I)A@);W&D9Y\a2a=K/3$)-;>?MPHP4RYQ75`("
  digit1$=digit1$+"[RX8R3Q;*K)A=6]6<R3GX]Z5VA+GW2T9G?*%:=GARUA5HXN[GI&22%%XL=AC'FU?"
  digit1$=digit1$+"$aJ-/ZRXH&N13X,E.X$;@TK9=F='V.?VU3*\Q_0?+,'N-HM),0+&`aDNWZ%'@9&]"
  digit1$=digit1$+"_EIKQCRX&JS$+?&-Q7YPQ]TC_A>Tb7,2A<H($La-7FD5EC,aTYcH]6SP8<cQX?^V"
  digit1$=digit1$+">-%),:)7,T/M2@^Q,55(BHD_R6/E:6G*`22=7+*.T%3cE;Xb?7$(2$U@+5+W?>15"
  digit1$=digit1$+"N3>*Cc^V6O2bBQ/41`A.X/X;?*2]2\=HD:Y0F+J4Z*31=$*B3,cLI0'**c6c@/%1"
  digit1$=digit1$+"/-B6E@EV-U387*%S=EXQCK8?16;B58,YaW(%PJU5:9/K&2-X-S5D,FU052>3Z5IT"
  digit1$=digit1$+"6EU76A9_RC[_(FF:_<T0I<Lb3@B[%Q5P0LK^6NSUFCE$?5.TVRX969-PbA7J5]:;"
  digit1$=digit1$+"U.TJC*+CB_,5,NVP4?L=^;Z2<LGbT7a1<]UKLH-0@H:.TN(-6'<K;U@@?Y/WLS2&"
  digit1$=digit1$+"M%A,=1\;FI>c)*,6@&7bD_9GTET3Uc;I&15.)ZR?S=_'VQB=4'XILJB<CD5FMEHB"
  digit1$=digit1$+"`cHR].44&YOc2\)_^I\T`D1HVTQ94$D$"
  digit1_bmp$=UNCOMPRESS$(INLINE$(digit1$))
  ' digit2.bmp 1366 Bytes. (compressed: 1133 Bytes, 82%)
  digit2$=""
  digit2$=digit2$+"P?>4%;%]WIE.@0MTXS\P,Y9ESE`$\bcYCG&ZARXQA9K=WIEO(WQ:D*<4A%0\$M03"
  digit2$=digit2$+"-\5Z?+ZHMZ$^*.aC8*\_Y<B.R@.0D^A]6*ZC$+UWP8*^?*WVOO53Vb(3?'=.4E\-"
  digit2$=digit2$+"\9GDa@7V(T(ZXF8\F*$83LXP:&bV+S'[L'F5I0a@D=$VL\Z.EXIW7@9XA`,8;0PO"
  digit2$=digit2$+"G:+T7a5$R?a9Z1F/S,BJU&I`;<`BR^T,4D1b'.G%F-T;.GJD^UKO]0P(WIa\8X1U"
  digit2$=digit2$+"0DZ&]LH\46/4QXW,=`_P7IH7RGE8'`02Z5<a(`\7bUV[B'&\E_?=P%U)3[;<G8cQ"
  digit2$=digit2$+"H9`/W=XX<_Qb<D0\3V*PNJ&7%V?9F&b:DXUYQ`CQ0XU*0(M;cA0/\cYNZ%@);&`H"
  digit2$=digit2$+"_;BK2M*7N9=XT6:B(03Y[2]'2@BD(\U]EP/.>@b2;HV\>;NbH`0a,4BcOaAAX0'Q"
  digit2$=digit2$+"A\b]:KA60@bC`aCaTPWC_K5+4P0Z+-0W^&PS+?b1F0Z<Z8VO01L:--1Y>;`B?J[0"
  digit2$=digit2$+"RUWGND5`Zc+WBN2T1<O7K=J88]\DN+/NIR=\D0?7MKF].PMEX<AZb):$K`_LW9IL"
  digit2$=digit2$+"aMbS7=B0a*a-1G<];&B%a6Oc`CO`V=J\]U9LV'*[6U`\`3T^O?Za+<'J(N0B0>N&"
  digit2$=digit2$+"CTJ_]WZ1[NE:)10PFVVJR14_8CAF9FIN(WP,)RC1'7%^@H8b7XQ\KQJ29%66:<0I"
  digit2$=digit2$+"=U1)1;]T>:NVT_2I9L5OV?61c=6B=/_WYH'/PD<8&L4P6E\8A[&<Q&]GBGJGX91S"
  digit2$=digit2$+"=+_S$bM)W[2G]UcL)F*C2@T.W.RD''VPXCWTT[FaA;_*aK0<$?\c_2SUa/@YC/;]"
  digit2$=digit2$+"K%T[Y-ORNBW0`EJ`H;O:Z=9D^*^T5EH3^`3W6>6J)_);Q%SHDCCcBAPaN)7%\4NH"
  digit2$=digit2$+"8c>\H*DXG+]^c,;9B;&'BbF9?/R`^69[8T(^0DB'2BIT%'/,+EbR;^0)Q(^QNMRP"
  digit2$=digit2$+"L?*?E7?VQ*EA_N4ZRY;\]RHV9_F(V,I=C9?\_H7?[KP@ZBS0<):K28BH&:^\6).="
  digit2$=digit2$+"HUNM0L*`W;-'D<:DH*_+]H.<+DB;E04WC0*$X'@M&H07*P`b15?7LFJM+/PZ0,%V"
  digit2$=digit2$+"MKN;U&AEH6T1&%>_;672PIGRMR[([>O1;:HMP(b+b8+6:G_7Aa((E\[K:'_[2=C1"
  digit2$=digit2$+"VS$($Jc75B2c*E>$9:/J%2Q]:2_2W-8ZLFT]C'8-cUGDI1H4^:O1c<@:;c^.`O]S"
  digit2$=digit2$+"a*:JEE<ZIK.B$D,9`JS^IVNRXYR_PE.NJ[0ZF`@7SEH]XHY7RV]TS;H]?FN=H(??"
  digit2$=digit2$+"X959SISW.1J5F@S$,<+NQEL.``)NI4SNZb'%6=JcDc>Z0X\2R\N;F;V)[';8M;ca"
  digit2$=digit2$+"G`+0?9?4DQW$W$b]1-7;\M]bH\1:T>aR-$[$6VU'-Z8]IV=)Y*a9A8IY[(<;b%EI"
  digit2$=digit2$+"GG(]&*;bM&*=M;UJ?%2<X*JT(=U?CE-.Lc1?MU(X$_8)c/N>VaH]1AC+JW_,SJ6,"
  digit2$=digit2$+"R>=5N;&ba=^UJE/<'?[J=V6],\(HTW_2-$(H$$,$"
  digit2_bmp$=UNCOMPRESS$(INLINE$(digit2$))
  ' digit3.bmp 1358 Bytes. (compressed: 1133 Bytes, 83%)
  digit3$=""
  digit3$=digit3$+"*8>49:UZ*;HB^B`$N@&*>1?$ORV&`%TD(T==`257?`,:P:X_8/E$_Y6bGWVL2/bS"
  digit3$=digit3$+"J38T,86GC0W.@.bEJ&0](MZ?C+1VRI^\P9N`(,^AW6*T@LKCTSb=6T*W099^<W7>"
  digit3$=digit3$+"/5&1-?^OF-\I>=K.XV\JSI>/UTMGD15\74<0M@:OCU612<(%`64B7cN01;HU[R&R"
  digit3$=digit3$+"RX4*.CZQLO;?O5O0_ZaH?N=IC((4.A&@9D);S=I@-),cBF7E$,.33O6HZN3L=9(="
  digit3$=digit3$+"6V6,(?;N(Hc_&ZW/:4_S6@:G%&*B3a;C?WAFQXJBKZ70T_KZ,a;4cF$R`<\`)=Z-"
  digit3$=digit3$+"M3'@%HN4S2%,_9NDb>%O;S.R6EX1.B+_>>\BJ/MH8a@*T>F&WT6RWE(ZP;H\E'9P"
  digit3$=digit3$+"NaM8;[=4?8a+P:2a.QRAMW>2XU'PTVF8@)4\5IMUF_UL1&Z<&'\;IcK*VZ^18-3X"
  digit3$=digit3$+"%Z>`63\,U=**ZJ`/QR/?+K^S,6AV6H\8]/\K42T=CB4JZSO\]&LKDbCVaC9%JQ'D"
  digit3$=digit3$+"[F6;.TQ7cb++b\&;T<3':Y\A.3MK^[[C\UW\:cCZ[%KKZ)G)D=:3'6E9J7'^7)X]"
  digit3$=digit3$+"+c>QL;J\7EJaA(&2R<,_/.H1@[PX$=^]VcQZ'MS&BUUHUSTG%cb;U<0&%R&WKDIB"
  digit3$=digit3$+"T?8$*XXR+V3C:>ZG\D9-aT<IS.D0K$`\^2ICZ2JP2&D<TU2RAH9[(\RBKN@B=POQ"
  digit3$=digit3$+",%/C]]]1:cO@]*M+Ob?A9W-[bMa0cOE\F[Mb(2=B^`Z7TG53%Q0TNA8=>%R@F31'"
  digit3$=digit3$+"/IcN5Q4^O4Naa1`2X.G3X56*(aP<B+G<AG_0^bOII7JATTDYDR$c_XJI*\<%8SZc"
  digit3$=digit3$+"-F4(;5C<AMM[,S@\`Y\6/4OO/(bPX6H7HVAIZ.A_]@Q]&RT3M$;3\RKB-J<?FVJ/"
  digit3$=digit3$+"ZR`I8OHJUSH?E`2SH,QG*;=T<-(A;2;CV.`Z*DP.N,NE=IY3(,7G=A>Rc(/JZ=FH"
  digit3$=digit3$+"0YL[888B:*9&K](8\aAGTT*9Y9`=22c.;7-)c,&T.`U+*)VN?a+]*OE[]T(=@6F-"
  digit3$=digit3$+"bR4\TR3W,cEYYNUS]-/TE@3/-Z7@=:`\>UB<9+bc,0_ED).:EB&X.R/=AOBMb@&a"
  digit3$=digit3$+"S:P4)N5+60ST3<2b\9ZbT?,$IJN;ESQ<&F&H:O/9a6G8&&bU3*I%^@OUKT*/.QZU"
  digit3$=digit3$+"F*VUYOJ)9@LV,;cF]VTI(`ZZ]Uc`YH.-:8FPKM@2C+WEB+JJ8;/TBc?.V3_,*>II"
  digit3$=digit3$+"Q,GcAS46V%ZKGW(9YK@P*+I$:*<%T<K0aPJTKb[B?0?OW@0?\>82.[\X$Mc0a)[>"
  digit3$=digit3$+"bHQ@JDK4T')KD$U2PQW55/)H47W<XcN9IRP_CBG9Y=;(U2$LQ6_*4^&U3P>HIPYb"
  digit3$=digit3$+"05MQS_&Y.XB&W4@3HS9FB8[L16<$D>V@?Zc[c`aF,4b?',Z?+TQGAH/HP=D<]1%D"
  digit3$=digit3$+"1MPD/[5;O^<&\C(bJ6GPDPINKT/%[14FB=4M+RI2^Qb:B*^85<YSYHV<(.OJ:GT="
  digit3$=digit3$+"1>G_UI^92BK`@Q]%T+ac_O:$7F%%<@4+:>P,VDP$"
  digit3_bmp$=UNCOMPRESS$(INLINE$(digit3$))
  ' digit4.bmp 1422 Bytes. (compressed: 1205 Bytes, 84%)
  digit4$=""
  digit4$=digit4$+"&9>45;\=R8c*,'-,P>B-6M0)6:Z^E054W&O23L>(5:<P$`c?S>S3Z<E.DX>acF3O"
  digit4$=digit4$+"SEY/2\U/BYS41S]^KOO8M?E.T@H&'_ZN``A**I>LWOQFRY=7.P?7RLEU6W\?3TDW"
  digit4$=digit4$+"YAUcR]XJU.8O\a&<RH)][>X_@a]%bbI>ZB/7C(HQKQB(_+^;$\:VP>,G@3:aT=c6"
  digit4$=digit4$+"^@.1H+$9a\6ZNT$AIX=.9V_3B-UMKR0PVD/3K3][L]H3^/a;K(DG53cJ;3<70BXX"
  digit4$=digit4$+"RW0;.[(@6;)W'T9b_;AVZ.3)aFDT;^V,U]3G02/@4^&*N<RE6_%PX`D>T:a5Z_%_"
  digit4$=digit4$+"3X;a?KRB95\E%Q1A\'K`YRO3Q3W`@2c0Y56[_`+YQU?R[(7c+,]]\&Q,R=&>a@ab"
  digit4$=digit4$+"\^Q[8[cH.7)a+3SM%E$.+@^Fb^GDYI0WW94E^^0N:E[A^L@ZBE.'RJWFK4_`O/[c"
  digit4$=digit4$+"5[74RP)M]1LVX&A7T5O@[/G`_:$\A_73C\QZ._9UJQ22@??H/94R'TRAX:3KWLYQ"
  digit4$=digit4$+"/bM55c*0F/)\$G2>V8/*W0@,c%;=:@*R51Gb^9'`,_T2,8[.>%bIM91b6Q5=XaZ*"
  digit4$=digit4$+"FT\D+bD$UU>5:O]R$H2?N,)_?2G3PXQ<I^E[I_=5*8S6E;1$`D9PIHWL.MWJKA%3"
  digit4$=digit4$+"cOCX-P`<?FPaENbJ53O7.[K)CM@.HJ^*E,P1VaIH)^Zb_N45DJ47[31U5].K`,5="
  digit4$=digit4$+";_881H:N16;_=F9LR\JPb%0MA]A\Y4FE5+I6F'RK7JKCJ;`/@AG6^>$5Z6M\S-]["
  digit4$=digit4$+"41D^KB-YY>P58KQDc+;8NRc'TE\VYPC@,0a99BZb'*$8YMMB;cT:_;:*JB/_B_1W"
  digit4$=digit4$+"ET6JA81=+V?+;_P:Q:)2FLML.[FRa+VYSO@1E/4[8W4TbZS=VJB<BE<-EaF6K/O2"
  digit4$=digit4$+"(ULP4_I?<<(*3F.@US&I=Z\5M`O6>&c6PK(+Z_^_/SA(cXT5(L&.PQ<E2aY;>F$V"
  digit4$=digit4$+"ITH`DW.R%E?;Lb9P**a.$cC`<ZC'b[IW]?JK/)E]QU`C).2>BY,BXK1KLV@D.bFH"
  digit4$=digit4$+"16\3^/[?TK(Yb\[64>LIBWO=4^`K,((aVY)/:^'[>>JO%Rb/[D(F_$M,P%S8<0@9"
  digit4$=digit4$+"22TO@W)+'(I;C)*UU2[T0M,CW^%OL$;C6:0'CL]Zb97^`?RQRP$(+C&GFWPV&VH<"
  digit4$=digit4$+">=(:35DL(09:G$NP642A7GUbG.4?JW-I-7[XR5S+4G1b.,0_.LD'$M$MB%=%$'EX"
  digit4$=digit4$+"[4'CZ1JGP6)TD>_8aC61aaF+J&9.D./U$.=E5]/Q?W7B.F$Y>$,E(02]R(F@<0?V"
  digit4$=digit4$+"H(YXN&SSFJZ^*.S=C?ALWFKC5A1;D)%X([(P37>7\G7)M'$4<a'^\N%8\R*C>F+:"
  digit4$=digit4$+"@RU>XH.Z[cI%<7/=)_0<,$H;\P>`AJ$4T93'40;ML,Ma-[Z3BUZ3Z%WV1SNOa52Y"
  digit4$=digit4$+"\2&'1]1Z4Q^WW(,>=<bTUc7SQ><XI)3Y9*,]V@7V1JDC8A:O,ET\>4&%U'=C3RB<"
  digit4$=digit4$+"+J4Q5D'2L(]K-HcLLTS$L_.]1?/5I-IbN\c31QD&GIUR]&<2=_ZT1([S4B(72&F0"
  digit4$=digit4$+"TSEU;V37`PEY>6AYU9[@_;BCO4/JG*2c%b['=`^J]SPbOW6XMN\$D1P1CTP=%,JO"
  digit4$=digit4$+"5`[G&40$"
  digit4_bmp$=UNCOMPRESS$(INLINE$(digit4$))
  ' digit5.bmp 1386 Bytes. (compressed: 1144 Bytes, 82%)
  digit5$=""
  digit5$=digit5$+"?F*4846K8:B&XVW`a)-I4P87K_Za*?CE_X+OLO=J>C[,0'bUcbA3S_)Q%RK4R2;S"
  digit5$=digit5$+"R;01<*'L41(\QBJ=IF:XO(.1]W6UH\1R<`/'$.>\\]^5+8[,T_5N;Y\&b4*^(-%D"
  digit5$=digit5$+"4$aF/[8AN,CWHKFK6OW]EBJ0R)C0<OcLHRX[@7TTH=@0G%*RW9YWH6GJ@4%cDT0<"
  digit5$=digit5$+"(`0F_(YV8C*3%^XR-*VAC/:<a35MK]/UMa?R(C,\,$ZD_I]ZN<O(RX_*Y%>J=9Q0"
  digit5$=digit5$+"a&0%:69<bX:Qa.;(-+OMaB(a63&[%]Y5YO+,'cY,W+<K5GS&$Z%,'BJM2='K,cA@"
  digit5$=digit5$+"IO?6Y0;[R0$])3`>6B^D`DQLN04D\2U2IGN)MXWQ&&,Yb<bI1$JFII%4?R0W`GGZ"
  digit5$=digit5$+"W:X$WRaSZKFTHC3O]0]FHbB-D@%JD8c'/GYW)8.&20HQA'/JVa2D(07+%DbK+;C-"
  digit5$=digit5$+"VZbaO$3_K_HB.SDbN1\%.W'9M^TUT\PC>ZC?Tc\=%-LO]PGF2B;K[=6KU.QD*I1A"
  digit5$=digit5$+"+7?4;0G)JL%1(;C`F@>H+NE?90SO/&MU/+cX_HH,\+W5CFT_:`E8:4B6)A=:M.JQ"
  digit5$=digit5$+"\VR+5N8G\M1Ga=3BB3G$FW=L7:-IB8T%`,YBWJ==%$1?\JZ$6H2)S*%OM7GM*UHH"
  digit5$=digit5$+"V*TbBSO3CJbRSJ5B6.M<M7=^BBB-*.(ODb4&4IWF,0O8a.299)$3_FE$;H]?'?W5"
  digit5$=digit5$+"'/BWHSC7LR@`^3c477<(V580YZ]47a*&QXVY_b:bV$3N\<[WW(;6@M$0E:7RO0&@"
  digit5$=digit5$+"4?Y&%@bZ6U_U;>F*\L5[R8/\b(QGDcaNPH(?<\4]]P9F>/C4:\L+Va1D`0NBABcZ"
  digit5$=digit5$+"424O1B]-?HGQcM;:%@WU*bYPaOE&`)a\V>4C1<$GM<F+@UY<F_JXCK:KF8RP2?Q."
  digit5$=digit5$+"OLWT,W)-(&JZ>;\:`%8FLX_+H5CcW93'0@R0L@V0&CaNT1A'I<$NDZW/5=Q$@DDD"
  digit5$=digit5$+"&:XI/%]B@NM6GFN?.aA8+,UD&Q>@PcBAY7S_`^J)OUG6cV-$$ED@809,Y4+]DH+1"
  digit5$=digit5$+"T;W/5SMbV/*@MB<=_<]W0V3ML,M&X/QK.@U)48,>[@/2RI/GM%-SKUa:;[M:VGTT"
  digit5$=digit5$+"<Ta/\1BD/R)S5UX]6>OLIX:_?OZbZZ^ac'\;]OY%.J9+c5NWKH*bG700=-V$QHc:"
  digit5$=digit5$+":b,.BMJ<(B2CJ8$FXEKO0ZM2H$9N4,,-.AYJ&K3='8M<a0QZ]Zb'3RH&0-B%../G"
  digit5$=digit5$+"+KL;C-;`b_XHZ%POZ@R(KEGQQ4F859UT^Yb>CQ8`SWHY4E>_\+`M&0NLYHG,[9L/"
  digit5$=digit5$+"/c[&a?-8,X$PX35/T1T%PV59JY'Yc$3)R8O@Jca&+M%0+QN1)9R-Ga26^B=,3?LL"
  digit5$=digit5$+"$:4V,K,K;0Z\GGLZ5;*SI<*SN;]&[;_7@B11)@DG&OLEANYX;LB42(NIXTZ`DW`a"
  digit5$=digit5$+"K.MS8cQPNI$%QO5OR`S=&AA1[VH=]%>\NC7JC+3TO[ZCD,`8ScbM+293cF\+\'ZL"
  digit5$=digit5$+"9L+cB4FN'54Z]42I`BC:)U(3^WQ1=:H>>R[5X`1ARF:L1VN:KL&9,4$$"
  digit5_bmp$=UNCOMPRESS$(INLINE$(digit5$))
  ' digit6.bmp 1326 Bytes. (compressed: 1079 Bytes, 81%)
  digit6$=""
  digit6$=digit6$+"BGJ47Na2J>,5;K1;I<+98%Q_(;<_[5bBY.%^KP;:cNY'UW&ZE9E+9N9`$I9,5;:1"
  digit6$=digit6$+"YH=5E^7J'J&Y;R_U^`0*='$GM?@=Z@L?a\=BRR;`I`V-KL'>R]1-BY0?L5]O(E7."
  digit6$=digit6$+"$Q`B9FNbBJ/YQ%IOR37Q%4B-VKaIb-6Y%'K.c\^P'5JZ(7=9E8SF/)bb$;XVC8>+"
  digit6$=digit6$+"=EOSR=S&/H+*7>>DZ&CUE*R*DX8&IQb);;T'A]N*7/=NW;';E*5bY829A9C.Q,E9"
  digit6$=digit6$+"),4TD/3cIV,?B'EQcLaAH@JZIQZF&X&+$%-/+_VRFG:N=)[:B&/6)7,8P['C,7]%"
  digit6$=digit6$+"O\HM]+PP10BEF_:SN8AQ80J@(?)(J16E>0),c5Ic&&8UYTV?UN-ZLR4$Q,2%K;A8"
  digit6$=digit6$+"''-0B4(:6?146B)Pb?P))E$_CB$T:YLFU@)]]bZ2;14D8FJ\L>^B3>29RNS86RTR"
  digit6$=digit6$+"2]_^N@ZCO>$-_D?.(X4%).SJ57G7E@b`VFZ+@Q2_Z[W_ITL\H-%4R*T5.Q2MI%.E"
  digit6$=digit6$+"`Ab:+/UPG\b:<]^C)I-QS3[0:(@M$*E&CV[219a_*@6.\D2bH0%+ON:CL%%JXZ(0"
  digit6$=digit6$+"R:cQB*BNJ@/]F1K%O]-<KTab,+4E%;DM,Y;cE.@,Y7@ZS%3DBF/@@1(/,+_/Yc(5"
  digit6$=digit6$+"UEUFcTc>1E9_E7?`5Q1?%UF0+U@00;>7`F'acL'L@,`Fb*Mc1O:$S>,]520PBX(Y"
  digit6$=digit6$+"_BSY/DH:VV9WRL(SC:bOORD?K*L62YFR,<b*.T)D1'T)[],<L$/BPT4@J\(aTDRQ"
  digit6$=digit6$+"&I2GPR1C3:]?``;T;*GBbHNE74&_*XFRR)$1b=;Z<@>O(OQ5XZ;%,YANO<`*V)[a"
  digit6$=digit6$+"Q^5[393L30a*Z.$5.G$M%.G0bGBHN4CI^]36Z.[I/_(L*&GN9:&,9$R(BK.UU]4H"
  digit6$=digit6$+"?%)M%,=&U81?aCRc-K,4U*J*^NOZ^0ZV?SCMW^/DAIA\(/E`.6=';Q[a`5-DCFY,"
  digit6$=digit6$+"JE^cFI4U0Z$GSLCOEG7bE^D[bKC<(?5;MK@T^X?'U%=%]cD`32`4'(CDC\aP+VZF"
  digit6$=digit6$+"-SC]L)Nc;%Z8a3,;V@P10C;TBRC5P@A0V\4S7K2-Kb3:XXDRIBR^Y9Z'I(%B:QbO"
  digit6$=digit6$+"VUI^4@-(*)6>6:0:TZ98T<HKa^^cVMT_LD01VU/$PZaHT[Z/]MMPFODU>+MZQN1E"
  digit6$=digit6$+"%Ya=)8=D+1M%1:R_93]+,JRG?^H6<K?5E&cb7)7S0D2(?R+JJ1)/HF9C_0R0T>5>"
  digit6$=digit6$+"$>&A(.>]$8a@2b(F4cP_$8Z(BX11cUaCa%<)J2T/?c1BZ*aJb,S\H.>TQ10S`FS\"
  digit6$=digit6$+"2Z*$XAC`V0>I]F3FGGOS\8D]NASXZc-(R3I'a`_S@O;86[]:R=7b,X%Y<F@E:@A<"
  digit6$=digit6$+"b-bEa&B:J:bFY.B$<]&FJ]K3Y1N<QTFOI44(NXPAH/./I,=TROCKFGH^5BI^J;]O"
  digit6$=digit6$+"@.;6((07V<4WbQ*TQ*K%/&0(Y1.@-T,$"
  digit6_bmp$=UNCOMPRESS$(INLINE$(digit6$))
  ' digit7.bmp 1326 Bytes. (compressed: 1085 Bytes, 81%)
  digit7$=""
  digit7$=digit7$+"`Q:4UKcI?6ZW5`U$QG23V18YJVFb8?P_&ARCTWK]Qbc>=X[\Z\,C0]M6TWI2E6%Q"
  digit7$=digit7$+"744>@';CXN-O&2$)P'0>=;ON$`QHX,JJ(_D$(8IL=U:_.F%^6.0XXENVA$JG7bC'"
  digit7$=digit7$+">Xa7(*.Cb-Y&La7414C*`KIP:@?;/BR*5MB&128+3EN`:$W2O;^JAG,2F'MOG7I-"
  digit7$=digit7$+"O_ACK\:$+0X+<6K895F>8^bE*$JL.J>DX_\A2H+=+MBab.\_9QXI9>BJRKZ??+.M"
  digit7$=digit7$+"JT=5O5VQ.b5`,%6N_BD1BAV48\L,bB-4$W4IITE<Fc@^3S/EUDT0K?0)R\>5<P7+"
  digit7$=digit7$+"?<)3,V=:Ta;Pa))3S<E&H,''Z:+4,4>UJ<?B4J'/A5[[P..SK$?197U/V;`F.H&["
  digit7$=digit7$+"&O`UH3L9Q5)B?$A/A05L(0SF.b<3LI7Z>YKNR:5+`;bL\(4L/H7):?F[@Q.LN-RQ"
  digit7$=digit7$+"Mb;6W\AB-OL5?-9++-_9&U*-S3S;H.I_c6>I$8CJU'Y*,+@$>7'ZM0H2b3FaN1.A"
  digit7$=digit7$+"88TO]?)KDcbQ)-GH+S\'15%YX&QS`498/)U*7PN@A41MTVAbG^J[F,BYVMU-;*;5"
  digit7$=digit7$+"PWYM'YFQX.]SMPG`V,8&WL+;M0:aPE<,6AYQ0BJZ[IJTTO(aK44U>(\TAa]CIT1V"
  digit7$=digit7$+"WHD@a9LL>NE_YP7_/)3X0^S+N`'>W^'UK`6M_;;/J8Z*80)AJX$;K5(B<QaV`:3@"
  digit7$=digit7$+">_^7Jc$a40S4K;Z'F;A_P^$E0WR[Xc:1@SUKC0Y8[30((b2OD\WY:Y:P&*YZ2>WR"
  digit7$=digit7$+"B2XJZ)UJAL6)bF97P\$DM?QDUR.+8WQ$6J.1N6bcU(WF`X/8J0J*)R5%=[9=2M%_"
  digit7$=digit7$+"VFA<<=(UaF)>04L[/602UMD<I5R..`0&;$c+YYX$B3\[9$SOXIR`UH2ENV?a];G-"
  digit7$=digit7$+"+cV^<^,*01WHET^SD)W9FB/<I''F]?Z$MK8E:8HI^4P-P*1:*b]P.^'YGaB@A';W"
  digit7$=digit7$+"S'-,IZO8DX`&*LVC\]:X-`Q[c)IP%.K-$cJa:):[_c--P7KSV8MISUO?M9Z&FAbS"
  digit7$=digit7$+"B2GI_8Fb:?U1^0<*TE<=0<X'^</69\bD[C%$I;bMBZ7:JE_O4c$^&MQ2ETO+IMIH"
  digit7$=digit7$+"U>,:82(aV3[>)8T<2<ZJ8>0?cF:)>Z=C;94H0YCbKUW%`(O3W$.%<S\Wc$UA9^^@"
  digit7$=digit7$+"DB??7.(T15ING=b6;b0&5'_;,%']aA_C1X.C&'L(/?P=P^b3DXJM?3DX-RF8`BJE"
  digit7$=digit7$+"b*3EJT3'=QV\JT/[/C<[_O-T0',[>'?[MC8?ZP)[;631C(]]%,_.+Q1[NX.2W6['"
  digit7$=digit7$+"O@MRXIS.9).']N^8;U_.N&U'Z[,)3_05L-I.1G)U=4Y]R(VW<BbO7]Ca%-+4L;RG"
  digit7$=digit7$+"4>YPK30_aTE_*DZ[RcF8OTWT`)E*(S?b;0+5(&Y&6c0F9_]Z?R.8+*';'3C,2H<L"
  digit7$=digit7$+"I<IZSWAP9c9_W64/:?DH6V.5?Ib6-80LBHa%$)0$"
  digit7_bmp$=UNCOMPRESS$(INLINE$(digit7$))
  ' digit8.bmp 1398 Bytes. (compressed: 1178 Bytes, 84%)
  digit8$=""
  digit8$=digit8$+"*8>4Y5G3(/D:a0(R$aL3O(XE\:%ZAbU^TaG1PY1U1c:@c<XBN\VOQ._6IN/%^.UO"
  digit8$=digit8$+"^J6+'7-5\E(:1.\E.,.WP52BJV=-S=1\I]P)Y:1=a,U@`^'+ZXFL@203-ZSa3U`*"
  digit8$=digit8$+"&?FKH:L58V*`NT0-QINSDUHQKY=2(Y):a,43Q\?`.^'&XP(]>-EAN4&<ab*L^(^?"
  digit8$=digit8$+"Z2''O$MG]),96^/VDc3CQ'%+W@Z1L?HI&&FQ)4[GTA=(W[*Gc;%GBHGF>,)(H*cQ"
  digit8$=digit8$+"%%>?JR'7A@=D[/JZUS+bEB9_JY\,a\5GCFW7_a)*OA?S'U2/-%R9IP_9Q69'`(ET"
  digit8$=digit8$+"bSFa%7&bF?DA9cGJXVXSWL)F]?$9&_2G;P:@3Sa9c8^.4_$_9^=59'=BWL_;K\OG"
  digit8$=digit8$+"*^YM:6RJ>bKHT.I3O?J&U:PNaAEJE(Oc1MSAJFCJM6$M5ba=AbR3V=)*?_S24R$)"
  digit8$=digit8$+"Q;\X@NH`3(E@2;8)W2G.YNS$JAP'1LaO'*QH4V7,(\:5I5C)WE-N02YXJR9=(%R5"
  digit8$=digit8$+"EH72FFL2IU%$3EIQ?@^CW>:[b-IS@2N27LQ1B)V97W3SFTcXQ8(F*K7,Z4A-S)Sc"
  digit8$=digit8$+"_O=OaG\MNcK$c$c<EW=B'GT(D%V>01,U@b>*U,AbR@T0N?(/T[.OS?^80E`Z*9H\"
  digit8$=digit8$+"RCY.V@(XE5b^S>L_$/B\;6KW-SN.8(%,;N`:5V&'I^4_^Z4HN0LQ@VV6+\$;_[)F"
  digit8$=digit8$+"C+L3W.;8:90[b?LUHIA8P2J/85`>^+C8.NHP^S\N``OK_C_Va5$W9V$ODCb13Q-^"
  digit8$=digit8$+"7P+<c3P_9Q%Z<bGJ/U'/9*F=M50Y;T:&OSbcDa<<aK;('7AE$4B(R56`EUX]B=?Z"
  digit8$=digit8$+"Z?DRH'>GR;%%K`<aCK3(VAK%%E8Q(E0)+-$62c*4/I/M+U(A<(SUCAEDa`Fa6;/0"
  digit8$=digit8$+"RQ)T\*3>TM1I:1$1>=<>0N.4?RF>9^/E%N;J$<[0U;,>-'Q]FETR=1=.WN^JK1H6"
  digit8$=digit8$+"?3\YL*@.7'PQB,$F2YA*//AIL5K?HOKT[K&>0BQK[G.N:DY^6Y[\\HFM[M9)MUE_"
  digit8$=digit8$+"7VbEQ%.)2^\TA06FJLW%%\B.1O]<&c@_+94L44)*]WF=>UHI:K-,X:2D:JM-FZB\"
  digit8$=digit8$+"-A2Z.V=<.c_`DKK-D2`K,UZbcJ@%U^a=JPSHF[;++1R-L=JU&/b1)J_a&Wc(N)0P"
  digit8$=digit8$+"(P[]BK>05=`<A`)]L7Y;R;5A;[03FS@UE-5I8'JRW%C[+I-HN0YB(-9UKY?\U6:'"
  digit8$=digit8$+"]5UI9.(X/*Z8`A*Ia$FX.0:[`.8cLP%NS7bOGDPCcbU1[OF0V>U796\E'V-[M_A,"
  digit8$=digit8$+"PaT*-TP5;?_=<C4F+$GBPQH&Q[<VYM^B'aYBFJ'?C7Q*)WAV8D3>)^5U&3(\WcU("
  digit8$=digit8$+"WI<99]3ZHBFb5_U$>cKN%V1YEQ$2AD1HG3.XRM6%>*BG[^6%()'*O->W$`]NY9]B"
  digit8$=digit8$+"(JHZ'%DZ_S<%56,U0'*GbKZR(/3cZ]\O89]&XW9aM'$9^UWJA:@D\GCX+7_8?OD="
  digit8$=digit8$+"C&M>2b'%E9`>@/M+Z4/@93)2U\/,O4>Q?:>Q($,N.<BH[>Y$@cc/EU`>]T9CK)Hb"
  digit8$=digit8$+"G^$T4%FD*[%7JH\$R6;Jc.:\bO<7<M$S$D4$"
  digit8_bmp$=UNCOMPRESS$(INLINE$(digit8$))
  ' digit9.bmp 1306 Bytes. (compressed: 1105 Bytes, 84%)
  digit9$=""
  digit9$=digit9$+"YS*4CHbZY4ODa_PY.P:.4L2b=Ua5(];DWUX`F:`S>cU1YO/)9W%&c4ZG-L0R5E/L"
  digit9$=digit9$+"QTHRG:)a0,;Y3KX&:.IMJWG/7]aPPDOTV>1L<a=AZ4%PGY4JBO/Z.(?LBSLV^_ML"
  digit9$=digit9$+"aB8';1N=YN3`13HXCH2)26,JYYO1VT<)Q-:2I4W&S3U$TKDY'Q8)7QKWN`/SYB^3"
  digit9$=digit9$+"@F;1WWE)_TNa842J4.-PA<(._Z\UM96)P%L0B6\=M8=V)`?/GFG7/F[BXQPD\AX("
  digit9$=digit9$+";KcARX;VEJA117J3Q[Q9&IP3$LMDY%:a_&RS2L+Z7XD8;a4)PWDVYNU&@?H1K$F["
  digit9$=digit9$+"IT[%L60]\`UPPJLK]V`A$7`Q.KD2LWO:N0YcO;c1H)HD%@_.*%PO,W`O-./6\G-A"
  digit9$=digit9$+":VcX(DGIGOL8PJ>,+-W7AS60JD=:\QH<BR\5Q;1=Z.8@`<O]Qb(MYPKG[8->5CP5"
  digit9$=digit9$+"aZQB[7(>[EBbR>3P0$/]1a&C4]>^%E%C_=E5.AA@4O@H/3/MbX]R@:,2;;:([DF>"
  digit9$=digit9$+"`Pb;=E('IN29D2&)DZ1BLXEAPK-2P-)SNHXNW-)_+.$+`GC2_>SU]PBZ0K<]E8'W"
  digit9$=digit9$+"=+(aDBXT^[O-0IJ\M3\&V:C*Z1L>I':Y14[VM$X6`WN``4M`P1M(\7>aaI+]9^7<"
  digit9$=digit9$+"]9`a$TV5ZHH-Y&='5D@45b[cW)%IS6+Ua34=(D<;TS(1Q%:<Y+'(J)\,C'_X<F>5"
  digit9$=digit9$+"\%>(\:;V5VTBR8`QZ'-R'J7MVK1'JX?%T'^&QUKa.QIHU.L_XAD6&:$TQ[LbJW'c"
  digit9$=digit9$+"T]45^-GT^FW43A_`;3X,U<JDFHL;I@.'<$K8U)%\`P_1,0TK@A6O?`;N2W]4:JZ;"
  digit9$=digit9$+"*C%,X2\4\KTAXQ3;RJX@Ab[E_`['G_`9c47VVG^:\BOGV^B*QQT6NTERLZ+BT>L&"
  digit9$=digit9$+"A;6R`QJ`*%X<A).JPc9S^_E<M,?>=;GDbIJ9%KY<@$7)3YT*-PDZ38KEB9,;+ALR"
  digit9$=digit9$+"TK>AU-8NIZZ.(CV/>$]L+?*4c/K0**XaV9MHP8GR9X>\*,%6([K?Q0_M1B)<]RXY"
  digit9$=digit9$+"FZ(J>,/8<.R'cZB;%C$>[@NT^,ZF46XD3L^-3>O`N=MG032\T2>(V-I,GAIIV$RE"
  digit9$=digit9$+"L><E*CMDCFJ1:Y[XSc5_[>UN<G?H\E;X3-6B%>2NL@(V:S&`^C+46Db'QXc;SLba"
  digit9$=digit9$+"SD=.Z42<8/>Tc,9Y74G?34(U8V+G/3)6B??%\QCCRJH),SbF%=IZaZ+b2$b0>M^%"
  digit9$=digit9$+"H-]KPJLVX`WN&HCL/7A/O`TZ8K2ZXJM$.Q*_N/YcX(:\6\8+E;a-'=EZ,U$+KW76"
  digit9$=digit9$+"_'D20OK_36JM:10aDU7+6R[I?]3>UI?>S?1*1NRKTN)\Q8:TZONCN1L))H>&>$]6"
  digit9$=digit9$+"`M0V(4I[LC,=H/8YYO*,_ZR1%>Z'a?(8@-aW^Z?MYEPJF<b=E4H*?$3DL`$93Y2H"
  digit9$=digit9$+"Y:E0V?])GI'(L)5bCX\ELRM%>0_/VTN'RS<Q8,(SA?8'9,=('-3>N2OVJ3&-.0'1"
  digit9$=digit9$+"($$$"
  digit9_bmp$=UNCOMPRESS$(INLINE$(digit9$))

  ' colon.bmp 1114 Bytes. (compressed: 839 Bytes, 75%)
  colon$=""
  colon$=colon$+"D)bDU11BU)%34US3J`RCXN=W/W$2$[E[2VSC+Q:.0@1UK>`>*1E*^L=6+/CcC^KH"
  colon$=colon$+"3/+WAaAD@:/Q8-[SW2Y]\RM+:H852HbFL9bDV]VFTZ5.7Y^`+_A)T?5[F&4b3H4M"
  colon$=colon$+"PXIWDN5:,)5c%['c25aLX*(HaG[+_*\]/R)O]C8ROVIT<9)%'5(Mb=cSG^L+aD=F"
  colon$=colon$+"<JbI1:US73UC`&,?>]=;J]K2(L:J&&O'\VJ=44a'5BbNV&-_@;(/DR)W1HFL:=?L"
  colon$=colon$+"X./7&<]Z=+;-;WLQ@?B-&67FN@WG4*1S2@K%L->>G]CX_AY<X?ZR??,[\T?8G+T2"
  colon$=colon$+"NYc,=C)11XG,*N;c&,U3cUS/?a3;,3-PKc<4CHH^]7Q=<M+LAPG\R1bK'*@QE[[O"
  colon$=colon$+"A:AIHa/IBV[_3G\O/WJL61M'0Xa<3N9'AB=00%S9*4YBV@&Q1?KMO1D;5L:BP=%K"
  colon$=colon$+"28V9P=L+0-Q=H0:,Y.=47J/&a7@H3%<RR09ML..2FVaMI95G=$McERDZ=Vc1Ec,b"
  colon$=colon$+"/R^V(VWVb:8D_<FR^Y23B8U3U+5HSZ>54=G:EX1?D9Y4c0CA@^DWI1\4B/OF,*3$"
  colon$=colon$+"K.F^G+E<9c,2=-=,,BMDT/8T\]DC?EW%N2@V.Wb2H758:N;G2=VF+QU-O<EQYY7U"
  colon$=colon$+"XR]141H]_M?'+`T(+V:X;6^R<WF8G@X9\JZFWJ0S`+Y,c*T`I;?VMC+QZa]+U)bc"
  colon$=colon$+"%(McDbA<%[UJbKaP/E6V*GSY7EZF>HW.J;9I;6ZF\FI/KPK%_+Z22=_QB1+R_-3P"
  colon$=colon$+"1+)5)&BT\=M(KLDP7T)AObc@LQ?A>;*V2D*P^[W[<-K\.9CHaX^TGOS4^+?<L-;T"
  colon$=colon$+"7D>@:`HWbK]JL8V'JL+@5<cEW:7T9JS9&KG0-%)`)I34Z>0-R-?CZ'+_0D=2):FZ"
  colon$=colon$+"*90TK^`PD.>327R`+(XZ;P'%cS+9HF453U'>b'`.YCH?V?'_,%9'<4<Ga;L-2VJZ"
  colon$=colon$+"7Pc&*7X_Z$)E*Q+O,AYIRD)YJ)M2=@+4_](U8@W@-5S/,)2OXF9O1280%6HV[^CM"
  colon$=colon$+"(0&N]K7;bG31J4'\$a4IHZTW>J<%?%<TDE2QR=+Ga*@$?\V;Ea9_FRZ^M.=XQ)6'"
  colon$=colon$+"YE)B6%)cX'K:/R;D(Z'-?`H*@a6HPV$$"
  colon_bmp$=UNCOMPRESS$(INLINE$(colon$))
RETURN
