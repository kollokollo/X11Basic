' Test of the PUT command. Should work with X11-basic V.1.6 and later
' (c) Markus Hoffmann 2011
'

schwarz=get_color(0,0,0)
gelb=get_color(65535,65535,0)
' output of inline.bas for X11-Basic 04.04.2011
' /home/hoffmann/ttt/bas/p00225.bmp 4218 Bytes. (compressed: 2416 Bytes, 57%)
p00225$=""
p00225$=p00225$+"5`GL;T^6JT35J5SM:B95UbGcS9+XO9cQHBQ3L.F?K\&0YNa1W3]W018S2,M(MG_^"
p00225$=p00225$+"9SR,7QS/AP%U,McO?OZT/YPc2G?H5aL,$K`&;;7[L`QR][Z*2J7Q@9WC(L'cJT_<"
p00225$=p00225$+"R&E+``H+XN6U>@\MV\-GV2cF?7+ZV]78bUE8/U&@>8>@NaUFI%XB`H'[J>[3?&['"
p00225$=p00225$+"DRK2HB/8HD=$:X.EB%(];MTR_P;19Y5F.[T7'EN/8;0Q:a?&Y;b^^0-7=%:LRE)I"
p00225$=p00225$+"VZ]0E12`-?Y;bba'@P3JA=+XO\DSKA6^$]E)EN_:a8ZJ><&bTIRIHKA]T%Y^GKE("
p00225$=p00225$+"<J\DLbXY7Z:01A0B:(>GTKVO[/7R)<U+_,+NR:M(@):H:/GW%)]@&&-&],&_D1[c"
p00225$=p00225$+"';:HV@B=<20?P-JMK<[D%E.>G.26R@R31%1./=Z42]-L'Q'QV.]GH-$R8<$<R*KU"
p00225$=p00225$+"4+$Q)2)Y2>-SHEA6\Z7&_>?F5D$XI\QUHV$$`5:F/V@;/KY_O5H71:b@0G0U;AQ_"
p00225$=p00225$+"S+FA56BDR5:::WP@Z9CZDBG3H07&(K?+T&ZU5O.(0,4c+^8$@Y9SO+B%]S.V`C1I"
p00225$=p00225$+":BQPI[$2>(:470Sb1/S^XC33B$5IUO916&LT]6YWX%G$@NW1K=3:-=,ZL0K6,UJ)"
p00225$=p00225$+"I8_IL'X`RV('(=/CEERc)N75OZC8)KDS4;,7V)Q8Z)FOO7>LA&J,:@'KBP5_.=L,"
p00225$=p00225$+"WE]PMF)?8,/((AH\RO(LR9BLCANULM^\NI(US[ON9O_]A_0C\9GC]c%O]IC`OMb:"
p00225$=p00225$+"64(3&OR^+`?(bPW7(9<37,3;;OFY;?\/?7.(U=b&E<?)WK_KI7XcQ+^S:X;cZ0/:"
p00225$=p00225$+"CDSXWS0bY$^D60+BT$2,$\P@YB-Ec-PO\Qac>OG0'*E,H<2_Y_3N/[,Q3GE&)VB,"
p00225$=p00225$+"F-&QBR&S/4.;R=5,XIP'^&$7c6RTE]1H06_,cD?39]LTcHR%\K2P,E5H:bQ:.37S"
p00225$=p00225$+",*_;2IGH4HYG*[DX[&DL6VJQbQC@@^MYVFOR4L4'@KZ+G,$4P);`/M`S\-QRLX&F"
p00225$=p00225$+"*/$`PR>WQU56'\MII3UJ7<)$__&9b`E:/b1_ZY[3:4:Xa?)C$P@PY(\.S\'[cAG;"
p00225$=p00225$+"W\Y]DA^(8cbV*%TTaL*CJ^3W:B0b;1X_0*X5]KF[A-$;F70YRb);L9aLYV\)TX+$"
p00225$=p00225$+">76`&0HYOWcV(YY^8EIL]]c3-4X]$N)D(XB8*TV'7D-'O`/V>VUV\[RQ>@L:U-7`"
p00225$=p00225$+"SDQV$JOHL)LWUMJLED+_T3Y7_.U`N(%YLI3HBTa5.a3;V(6K[>U(5HLFG_O[W^K;"
p00225$=p00225$+"'=_B?*)%.M5QI0AW$L7ZW@E%9H8'7F,56CX_]M%=+TX6NWUcYIX`L*`.A_?$W2>Y"
p00225$=p00225$+"6<U?@Vc&)51T%+BK%(6]6E@7]c/<:2FYc.J1904U_bG@-b.-90QLEBF4R;%]ZX(_"
p00225$=p00225$+"BCVH'K(P?NIP)DDEMb])70YSCA.64YRD94.JY0?GLH8Q9C`R*[Y6AE3XP-\@F7W3"
p00225$=p00225$+"1I=/=)V%XJb88B]-NEF*3,7FOQ/.[(5R^>9(NHV9&28]&GCV`%R-0:-NW\+:9ZX5"
p00225$=p00225$+"OHMP82NV(C%(C<_.8;J@I,XO*aZV)9O:K3K+$0.PT9&&AT0@'8$cBAH7YTM4L[P0"
p00225$=p00225$+"YG`)7N2WOb<_2Y;c67.J7S(JQHWJR<WZ=5A88XL^\E2W1:^3\YQ%.*0KD1G,D.J."
p00225$=p00225$+"AGZJ9?(=E5^J5,E:Zb/]UFU3b<MbTH35C7M<*'G>LUJR^O,2%^&b4_8S597T;G%5"
p00225$=p00225$+"O$A<CD8?6?&37>AH\ZFF0KI372-L.'[O)[?9Q2G`PXc^`Z>:E+*F$;$]^bL>+Z5V"
p00225$=p00225$+"R$<K^2E6=CWCZ'?2J,7>[=3=G\?K46\Q?=)5\13-5+:N*4O`-AZ]MTY,$D6&9EF)"
p00225$=p00225$+"9U'%0JD7.?YN/\Q4`\WTb20<O1Q,3H/7<.]T/8]10%)>:XP9&U>VTH)8\`F2JTB\"
p00225$=p00225$+",cN1HTNcJ))$9F*7USV]>33363=,_O.C3@@0b&W-I2a>`UC$STX,M6@35[23bL`7"
p00225$=p00225$+"QZ/WJ&TJ\<*:?;7Y(2-)_B^=5@NPQAX)Pa-1`N_X03<9;GD\T@6J^cCJbaQ;^*a)"
p00225$=p00225$+")4S7,[*RSaW3F`;\YM0D8LY[9\_ML$;POES)_R--A6<IO1)&9Z?]'9&E^[M@[KL<"
p00225$=p00225$+"abP1\M8/;a+\+'P_1+L2OADP[0HX)*7A_F-8GP2ZS7<(8)HbL8SX6CQ,\^L&.L]W"
p00225$=p00225$+"J5[P+X2,EZ^`YL>T:+SUZW:LM7L(]')bX<A0$+NL4L.bcB(2-9UaS^Q\&POD;M_M"
p00225$=p00225$+")S)8TE6<O$89@ZVG%;7UHabP*T.N$:Y+R>W5_cGCA4_>Z-DS),-K@?&AQ$1^.T+U"
p00225$=p00225$+"=*,,Z1?=c&E$'Q@`:'+X\A[3`MK\b]2$(:bCaI-Q\5WN;X$I$I)4ISX\>_-@bBc^"
p00225$=p00225$+"0=[<a(R`1\`KN4D$]=3U69O-?N1NQD'Z:C&c1_VV1$2J%OM2E1GDG3?@ZT).=+LK"
p00225$=p00225$+"]%)$+9JO'?(aP-;LM=Yc:(V62C3$(0b/TC?+>Y8*C]&=BBXU?@X/PSU[33$N^JFa"
p00225$=p00225$+".[N0AH.c4K]GC3]DX%<<V3:Pa3,-^S8UJV+-A`5GV_TR0]X,T@KK?c&@4VPKR'U["
p00225$=p00225$+"<2Q^(3%%]2?>IC?&b_Q$O@SPbU5,.'3IAMA+=:H8W;Y.$^O9F&3=03S&Q1YHV`,="
p00225$=p00225$+"E$(*+4-^aX&b1ZVYQ$Q_3V+M&R.LC8O^XTc)NbbEb]^&F:C0P:[&BU.JII;4=L4;"
p00225$=p00225$+"EZ'S`-'B;;9[3T`ZK,KD<75cc<'*%J2YY4O'bOSW(].SW^\8[bOKJ`QU8S*;9D%U"
p00225$=p00225$+"RVW6L(/ZC\<@MB$?[9[A@JY;`K(C31BU*9&FNcII(%TY=`/F<J%:2Z^.P?5LJZU<"
p00225$=p00225$+"D5GFLK.')83@K>b[C(U;RYPZ*`4`PH3Ba*Z=[Z2@'=JV7_F5N]^aUWEX?P8KDc.b"
p00225$=p00225$+"HLMVFcBV_T/Y0\9S5Y`9*(0M2<]F%'GW(,HUL&*8XP48$JPQJ@<_S8Ma131]VMM;"
p00225$=p00225$+"Q7%Y9`E.=ZX%b9]&+ONb3A4S1])E`C`%7CVcVQ_4c9T*SDZ&YA1HF'_]*c4%YRS@"
p00225$=p00225$+"`'3/:K/,0JB<+.A*\-4O'>>bY[7K4-N)</*0X:6P^GMNJ8NaPJ\+8+B*K2QI,$A^"
p00225$=p00225$+"1H\4b0PRH]U*A6[U`CQ\5/XBT8SD:aMO04?3OI(Z)5]c5A(A9W,A8EFVEL*b[_^["
p00225$=p00225$+">`8I,*aPbB$_QWYb*MPMJL*$@P*O'][U_(MMU8N9IW1E/PbVX$bVY\;)>PDC6P,Z"
p00225$=p00225$+"O?&cZP<D$8Y`Y<B&BV8(%$$$"
p00225_bmp$=UNCOMPRESS$(INLINE$(p00225$))
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
' output of inline.bas for X11-Basic 04.04.2011
' /home/hoffmann/ttt/bas/p01469.bmp 65674 Bytes. (compressed: 10920 Bytes, 16%)
p01469$=""
p01469$=p01469$+"%WCU'\S^0E:U4U,YJ5@13c=0a[>Z+TO'(cbc=HaK6]N8+KGZUGC*%-K46IIB:_/="
p01469$=p01469$+"UG+a4GA,M<NEJKFQC-0I<HW2FY='U\=Z664/P/@.B<.2R1b'V6XD;VWQ@/-'4Q^S"
p01469$=p01469$+"ZKU7'I9/F$4T@;D^CS9a9D1[>20P\S;UN@]c:U,/KR1K`-O_(\K?Y3/-)K@@@P=["
p01469$=p01469$+"/H^4P=`17RMH_\/1`XA0(9MU]7(N&C7&F9Z&WD(Y]D\&3,,Y)0XT5SO:K4`;;]%8"
p01469$=p01469$+"`EK`JAZT<a*X5'O'V7WXXLGX$T5[F1?VDZ?49ATY0>S-*bN9EZ0UZMQR/%7A`a]3"
p01469$=p01469$+"(EN@$%;TU1>AH<MN@)A$1L7>%P$1HULZC??7F-R\L:`c$I=_%,(]*J?58^M1I0]A"
p01469$=p01469$+"TZ]9ZV,RcB_8`baL2>(BDVQC.+L$S/2$&X^7<$'c2L(UU)I%2=ELMK>^C/*cVFL$"
p01469$=p01469$+"X</6V/&E8EU3%\E^XN*:S`PW^>BcUN(S*12V>*-'O&JQI.`N&S(C'PQ$<]KcZc=^"
p01469$=p01469$+":P5$]`YU304b<6*5]9J%;)J6TNaYEV+VS;9KB/V*CJU_M_bC<V\2PU^3TKSaRZIP"
p01469$=p01469$+"T[>DZ\@F58I(1,aZJ?I1]%TYKcNP.%H>D%F6PE*JT*V+c2%4<_5F/WIJ$$1>G<KK"
p01469$=p01469$+"B*S+]A0G=[_)Hb_4T46b1]A6??GZS9%:<:>4D>Q`]D,Y,X'AR8<IbO7Xc7%'+'.R"
p01469$=p01469$+"Z6E=a:J'TZ/_9JHA,3AM%R*-,Z$.@_2]N.Q7W_;?2A76)c^DS0B`,KJ6,+ERH@W^"
p01469$=p01469$+"0AI^N&<0()5$^W8$-^SaV`[RY:+,BAXK%S67-cWcO9O,*>4%).Q%7RV+=&RS61/A"
p01469$=p01469$+"ULc,O<T[-1EM(/O/[KV3Ka>D8*'/MA37@+SG]Fb]O>$0O),EcRX<B8?&<-`b&aK?"
p01469$=p01469$+"]U<bF8>.>CC*Kc;L%L6[(FOOMFPbI`U?D6R4c4LWDK9FNNQb.'G9,=[9TY*RSJ1L"
p01469$=p01469$+"G;?-933HM)R1;9DP8KA4Ob$cGH5^@_I;V@7[%?I&F%EH*2I3X-@,]DS:TQHZK3$;"
p01469$=p01469$+"@PHYQZN+\,5LE=PD`9QNQ)'9Q*YX9>?3[;5<6L-/\R=>^(48CD*)aX`DR92J0$?2"
p01469$=p01469$+"0Q)E9O*:*%-?DaXc0U)*a=45,NKA=b)/7,Z?]F?Y;a3>B(%_](Z/G71I3;$9L$Tb"
p01469$=p01469$+"/CUFO.F%,K^ZIREZ*0V?F3HD(6^0?ZFTLQVO/5N8I&^baH?;VYL4,<^4,T/;bT$6"
p01469$=p01469$+"0?3Z]GR5B=,^;N(/(I@N5K(2>&R:VA;b,0Ia+1[a':CP$;MI\U+,Y_=?K@9/HD\_"
p01469$=p01469$+"6UR2TD/7++7^SAJ`1V+5==+$CB>GDZW&[*cSSZ\BSUa0DGR<1Q=+?XL=_9>,(9EP"
p01469$=p01469$+"Q]Y/7ZWBY/&SG<B_`IE:<8=.U@7<VR2P7IXRBEP.UC/'7W*3O_3].2QKZIVV8M'>"
p01469$=p01469$+"b/-?T11C3:4W60@]Pa\+,T9:<6OVKJUHb(4cX.9)T0KJ*=PZ90?*)6FGQDE$05AW"
p01469$=p01469$+":?[J([a(\YS2C*+CP1>4)_?)V,4'J&KKb\W;%`;T&.1'5Q-W1DY41VHB=GM4UDL`"
p01469$=p01469$+"U?b?Z:U$9*%@I/2_\C^+A%*;O'-\W*-04G[5$T%M4@LWA_(%2$U`X$Ta1DUC3'Q3"
p01469$=p01469$+"Ca_OJ/3Q?&6_'99\C^_$&FR\I*,A%I59Wa:7*W(NcEY%';66C73HC/E>_13EBD)O"
p01469$=p01469$+")OV/9(V4V,MQDQH%+&XPSbIR+2JaDHWH))EJX`<$6%^I.5-WRRJ4G]P%AT931]^_"
p01469$=p01469$+"AV0;^MXL8^->QG^1^'S?\@;F1<]5PRJG4@`W^GX.A$APa-%,B4/C`^[?9:EA7.)Q"
p01469$=p01469$+"2*R`\*>Ja)_M;=@[b7S%%TWJ\.2D@:SQ)]H>B*LEG&$Sc6G21GH.B2'R&/[`;/Z("
p01469$=p01469$+"BQ]P)1ARWKBG\NZU3VXY`Z/JJ=-`H?CDV24+*0:Y6;A(<;b-:2?,bbcU8?DL%SN["
p01469$=p01469$+"/R%(SJZF@-;EPcNDO)`+BJ3PDDa-(&_I2[\\A54;DN6T0b@,7HA^^?<F^IJ3M5R)"
p01469$=p01469$+",>a(%`%'(B-/MLI:=IB?9ZWB=a@Y3(C&OP70W`%5^$WX?SaJb0CSP`J7aL1K>EB["
p01469$=p01469$+"MS2(IGV%cVN$).Z1<@X/7-16Q?Q2.P)(@^+8S1UP*N9GD.,'/A66T6B]*AWZ3]1P"
p01469$=p01469$+"XU32EDX65$LK+[$7?4I&>$D.:O(EP,UW0B;4A?$=O'XVQ*J_WIG3*:3=M_b'Z*c7"
p01469$=p01469$+"1^)&bU8CLP/c;JH`UAF+=/OYb[$WSDD6Y3HK6@4FEI]*0F^O>BD>Y,]93a&6bNLU"
p01469$=p01469$+"'CU(AN_*C6H&M8&`Ka_LOTCJ$J7>%NN7Q5c%2B='$<X+YO9MAD6Y+`VS.`)M%CNM"
p01469$=p01469$+"8)C%^L)$W8c>:4,S/NXW9`P^[4697+<N(L<\<ZL_2[]WT[a2]GQ6/`N3E')JU5BC"
p01469$=p01469$+"\HX?-DH,7=_/'?<8V`VR5*.P$5=cHH/?1XXQ$;/TB?[8aSV4H&*6Xc6JJ3C?D%DE"
p01469$=p01469$+"/(IX?T?KY.XP3^4cHGTB-;-\1_D6ISP)J>3/c')O)P-QW9WX=BSEQM:3+J@B9)_W"
p01469$=p01469$+":UTaSRM9HP'C;M-b>F4,P9WA+X^GMAaKJ9cD.3UEP(\Pc+5JSPaFa(NN00%/^J*1"
p01469$=p01469$+"Y7?TIQ8FXHM0<PF6=C?4T`O?.JQLI>I_'E3>4>'M.XIbQ,UC@a11D)'PaP=5V;&K"
p01469$=p01469$+")9YP&_HH;9HC5*YBGJJSbbc$R@0\Ac&>OOUXR&[Y`I^985/^>>B_aS/TK>U$W0)E"
p01469$=p01469$+"^O^<CMS'\ZNGR5_;1-`TQRPPCD`EZ)DC]<@KU(WZ4OH>a=.$Z1^?<EIbQb0:7>32"
p01469$=p01469$+"-E[[793%Q`VY6CLCS,/6S-A[C;Q/:)H2`HS*VMKB,a4JB015;M;c'-05T4+6,Y5E"
p01469$=p01469$+"<(;Eb*>(.V)X7OP^Zab?`;LQ7B](6E.ERX+YT(T-aN(S_0Aa\E^Y?D(Z8\.RM.FH"
p01469$=p01469$+":30[ZNO8c4[_&R)YZ7]1B)bGC)^4F7*;*4.OEI/cHO*Y*_WBB-EK5aA<C?.G]&@V"
p01469$=p01469$+"&$L9VHG,\\$=G&P$G=7?HJcU7J@47BM)_*P\`(Gc8?<@8;C@3[OG4C9*HV-,B/ab"
p01469$=p01469$+"$MU.0\LLaFB>.,%K\>.C-B)IC&5DEQcK93@S4,I`6=(K$`TM'_?/6?b7bb9EF.A:"
p01469$=p01469$+"QA4C7K(QP-0U]3.-)@-[C<a(8\.OYYB/(2\Q)EN<,+]Aa?R0`0CEY-)^IDWE]0`R"
p01469$=p01469$+"XAc-CcG>:T>6R.^.>%D85)Z[Q%E%P9PNb)%^2H@cP]I([-B;B1TFLCV:L'0?OE_H"
p01469$=p01469$+"0OT%/%H3*4RC1)?R0[1++^(2GFMV(=0Q9`CL7\(UXN]>cFKbaB/0:274,/?004C`"
p01469$=p01469$+"I4VX3b&<(KRLCJ,L)I262GY9OK<MWVN/W-I9F;X\&HUG])Hb]2@?O@IMac.)0W3-"
p01469$=p01469$+"W1Z([KRBRD+)*9Da2`6NO_I73E_RaU@;C^[3E+&ZB>&7MZZ_D;SZ;O+)6(@[P:6&"
p01469$=p01469$+"J^R->';DX$\H6.bF$AT)*5.&'.@7&G^.&TT63Q%0%>_1/b.9W(a9c)HK%BTA@-O."
p01469$=p01469$+"<B0-9(OTFXH<N7=U8aV2J-3%)=IU)(R@U^($O9T=X6\-aT-'`+2Z`R,QNJ1Z?DUI"
p01469$=p01469$+"341%NF<_DH4LCR6^V4)EcE8HKE=c.(2IO2$XcQ34DLMa$5c+:Y);76'4\0.YD2BH"
p01469$=p01469$+"5V4DHJAV7S9M\8PX/G)?T&+?<Lb%S$=ZM(:bc^*@Y'7(AMc8@&/*38$[>_M<P5+."
p01469$=p01469$+"'8X`c47ZOXA+Q/4D8AV?3J9+0FB'`5'6,L.5TVL7X,a_`?VM/O1:KSU]479O%3R="
p01469$=p01469$+"a$@DI]X9G>:?J?]M+28SJ>+U5H=Y]?6A*C6[S;G,WLNO='_CJR>P9FGTH9CF2bKU"
p01469$=p01469$+"@FcTE)N`:C%&,Z>@Kb8F+RX;;W>=X=Z]K1D*M-2SVW3-4PF4EV?Mc;$T3M/:\T74"
p01469$=p01469$+"H*UA@4'TS/2I]aI>6@GA;1)7NMSG=OZ4SYN5>%*c,`I0.1&Y(RDC/Sa_9.K&RICV"
p01469$=p01469$+")cDL>R**45.(Z6C<LGK(;6]0I;**@9*%I1FP103/bH9GTC.`/NX3bUE0a-C)>&'V"
p01469$=p01469$+";-4/[>GTNF-_`642F.GG[_D>^L_<G%\$F$(R\=H.TR]XZ$Z7>$VIUNE7B:O;MK%D"
p01469$=p01469$+"03$cQPa+Jc)=HGbbI8JP%%bTD'V``6/$7c>XPC`<Y((<G5bbMG<4C3GL%4.J?,'a"
p01469$=p01469$+"R[DR)3a8DCA*0X'@=MU5O)-S\?M`ZLbNOW)K@5Q7I].?<^L)<6TI$/LHV+K1(W9c"
p01469$=p01469$+"O4+G/BA=)<J=)*WX9`D@'FI/[:L7R8+FS%U)2Vc0LL7a5-Q>U3.6'V%1C-;0Y&b:"
p01469$=p01469$+".`65^Z2`MS&8Q@]N46N4U%aKK4LIZRPVY^R@G=H.R/*D/H])_G6Wa_ZTXBF(:C'-"
p01469$=p01469$+"C*X.8ZP/OZNP[D5S8a7)R](O1)N\5*TQc*+,5L\aJ8)CU03:I-c'_EV8+^5-N[R*"
p01469$=p01469$+"F<^XH(acTOcDOCFDcYaP@Z]P`XSY7X\PFRKS$+\ZR:E>_D76I^7K72DQO.,OQ'4L"
p01469$=p01469$+"%7%\6O7a02B5Pc+BE@)`%2^/3IA6=D36K/Y_F*S/6(EC,A^<N-FO@IC9'^&A^?L_"
p01469$=p01469$+"%RW'3<=D2;YG8D??=L)ZJ2.)Y8B>aENX\.Z(%I5BHO>::M$6KEbF;FRQ>=P$K?DZ"
p01469$=p01469$+"JH_DV9)R43)b4]X6accV_O(@]A%M/.5Ma)3G\?V<FF:B3c^Q*JTDU^&$1Q68:^UR"
p01469$=p01469$+"7+BYZU/GJWc18=E;VGJ4_:%L,>MaO1(U$VZRW+WN[0,?)%;4^AIPJA_@9M)8AC)T"
p01469$=p01469$+"cII>=VA[`1b%,XYNN5>M]P_7-G)>5Fc(_B7$YPb(QS?6X-F`5$R&_0*P%TB[NaIL"
p01469$=p01469$+"_(Q>S@QYI+7@_BQTK3I&E/T0'0U1J**KP\0E32`,85(K+:9U7ZH6aQK%Xb.QDQTR"
p01469$=p01469$+"'9$;?C[%5V%QPZbF@3Fc8XIZK2LY\-.E`X-%Q$)L;>I*S`M*_VLGV$;D>)+CW-]Z"
p01469$=p01469$+"<GO*IK;EZS,SSN=&6CU@SWZ.C$W`86.F(.-RA0?CCX3F)%^Y3]J:P]DC&RP48&H`"
p01469$=p01469$+"(,4R@G,&.,?>BV;F<\c.F$.cWa&/A,,.BCa:32$8TF;2SaV3\`BYQ[$RA:/:N.\K"
p01469$=p01469$+"4$=@(8QJ25-?B?,3HVX%S=VX1?0V0]CCY)0AM>\I_[H8E(@]R,OaA5K6KX+H0c?V"
p01469$=p01469$+"*G;2869.YZBXcE<;8COJ]ESLD-A@c`)cF]M.YTPA,.29'JO)8&_&0PDM51>?bX09"
p01469$=p01469$+"[+$4(',&`M)7+L.c<PW\DK)*8(]V1TaK@/?W_b(]b7J8aGJH5$Z2Lb1M1P:A[=06"
p01469$=p01469$+"(/a\18H<<DNT9<b+B[<RD%bLIV@&(RL*=%ED49WRbHL,LK]a;<J.F2PY8W1<+/5\"
p01469$=p01469$+"/U?M0&@G<*Ya_(NB5O[NUP5ID5<+^;QD0?Z2HJVV]Y>F27;;,;K65a\?I6O\B)8$"
p01469$=p01469$+">%Q5SHb@\EY69.'Y&EQcJ9N[8S(IIC%BNbAEC2?671BNEI/-8K>KJ6=+<)_(L\16"
p01469$=p01469$+"L<O*T]OHMGb>DW$6(/HBBG[J[TBFW[F`9.\[?>.R^302-cDZb<CNOOY0$_\N3b7G"
p01469$=p01469$+"^W;BF;FX9>b1S;C13WX[%N6NG<(D7*Qc7IYX41P^F&X_V3-I^Q$F12L0'YQ^EaTX"
p01469$=p01469$+"WLS\X'2V*G1%\F(<%%D2X%W3U$:GF$$54Db^0;VQ)Ga-ZTGI568A@NObXK%LGQ.J"
p01469$=p01469$+";=61*T^[[%P%8KBSUO%O,H:;O90,3<LH-/%I%a6B25?GH@%a=)9A)N:`2;$>M$.R"
p01469$=p01469$+"E(1=2,0T2b3SaL5I-HVD]%3.OIY]:9>Z5?3A:$'T'`HL[Fa[>W74:Q$B'Q4/;3+_"
p01469$=p01469$+"+%8[4$SW8b>%L*-V3X`YC.L8JY?OI&6UcEOIKPN<*CY[_21?8=%(L@1A(`R(%N0`"
p01469$=p01469$+"8cZ$V%W0)b$8[7L4Q`B<c[SK\bH-?c1@&6:4+L5:b@S@V`+ZF&AGW>WK1@a)/VP&"
p01469$=p01469$+"%V`C5<c9?OE.)G=0A631=/.L0?];=:NEJ-.BOQ<;D5,W'b.9(B=Y;BYP<54&%aWN"
p01469$=p01469$+"V7BEF7XXP.ZDB-IED='7PV>OCA^HJ/ZJ+\SADaB_97_)%IL_WIb]aC:22:N2%]@("
p01469$=p01469$+"5Da+5A_8Z+bQbBT$9Q:())6W'K/O0.'G@V3\.MPFW;2O[/0;0K5&b&+DL_T3HZC\"
p01469$=p01469$+"LL[XW*MVU6DA7_@2CY'<LWIWI+.)L7KJWW.b3?9IDW[$\O.E%Va11]7M38A&O%FM"
p01469$=p01469$+"%U3%?CK?N^3<K8W*HV?]+AWE8-15=ZQOIAU7OF/V?313G+AS7b2Q5?_UF:-Bb`?S"
p01469$=p01469$+"&ZW$%9_G4aK9EE)F`]9''J2DR4aX`\X2,(5M4XLPT^<^F?.O=KS+4IVb%>*^SMc^"
p01469$=p01469$+"$_M8^UDDP6&/.EKMbKLSY,&`BY3DFS_N;?,9NV'91T'-TI+?T(L2-8\(G>@.;]ED"
p01469$=p01469$+"&YFHBAGWB00M;NW.<&5c%+.1YLU<Wa^KV_G[>a2*P.Kb&\:%KY<Y0^CL3W+(G=R?"
p01469$=p01469$+"UB8%=);aA.Y(CD*Q/Z9TP$0'4FY?+[<HZ;2ID8[/0%-[*[D5`72bQ%:0[>V`6V]N"
p01469$=p01469$+"=85LIHLD4O79)E@ZXabcR(CR(L<D3S0/<^+2[U)X>;A`,VP327*SB(?6:VP7[/cU"
p01469$=p01469$+"\6(>^]10N+M7;C]\:CG31,*9J(WQ$Q3U1CUaa<QWIB^^'VD-,`Q-^28V8L`X$@**"
p01469$=p01469$+",$)*:4E;C*@2BG(3*F*[[JN7,=7/WX@$@B'L&ZAJ$@X*F1]+bM:%.aJ6OHJ3B0,T"
p01469$=p01469$+"XC:;%GTYTGXFTKa1RU=NS@;%^,67G,UH1-@44AaY.=S,U(`=%Yb^[b_@$J=2cV^O"
p01469$=p01469$+"(&RPHB`A0;GZ.&5V]%=>(6;+\7%(?@BV9_0C5Q+:Z(`2(7UX.P.NbYESVc_,[>/?"
p01469$=p01469$+"aR.$`A/_AQ9ZA-+*&]5GKC2SC<-B`8NK&799RT*[F*J0NB^FS,CXGE?F+bVTBb>."
p01469$=p01469$+"(J]Rb`bTJ.C<%L=Ac;=\]A<0IPB\Z2$R$WPDDT(1DSc47CRUZ%GZT\<775c$I(8="
p01469$=p01469$+"*;?YC=RQ2%9O2&,MW$`1HYQ]JP6,E`J%--M@U7M[B'RZ6FGHH/XKP_K%ZR_$T3)H"
p01469$=p01469$+"ZY%N=5>[P:)ES[T49[X.L/2->*S/'XV\CPQ[VY/a&Q29QHbC$@J'.VXcD>2cC>M6"
p01469$=p01469$+"9/RG,HG;81ULL.(I[?8*I+aTDM]DN5[D[D]QB]DGJ3CbN)SS^0QX(^U[X8W6&18H"
p01469$=p01469$+":,=,R9Aa\DVX62QDQNBX%5W?$2<+?7D:R->;&5%@c]RKKK<&'3-,,%*5@-'`LWT-"
p01469$=p01469$+":/]_TV5D5P5QSYQVcI1,QW8OIC>E$@K33=/Q1([QKN+]U/8Q%'b:*N6F8IZ0$?3L"
p01469$=p01469$+"Z/R,'$I2MaCIAIH.a;UPS%F0_']R<%%E?YD$\L$&AWW*FR2\6ZVUP8C<J5U?HWbM"
p01469$=p01469$+".O)V@,ZFIQ^B(MbL*E,MFEW/%J*P7)-R?//JZC'%V6&V(7JI?D`*Cc1KZF[G-3.U"
p01469$=p01469$+"^;0$4IY.4X.&WZ=b/:.RP=7I-32Xb%Q.R[AZ_>7%/?4=K7<8/+%>[E2^,8`GGRKD"
p01469$=p01469$+"FX`?1SFB'aHQ;6J-*X)H$ZNK(IS7`U3ac+M*-.[Bc:3BCMcH/342`)*L;6L[9YR$"
p01469$=p01469$+"B9ZCANH&J>*&K@J==LX&b.KcY+$_,:KBLQ8%P4LN1TL;'N=c8`_58TNL);C`>DP<"
p01469$=p01469$+".(G0O=WN6;,bVIIBH6PM0[P94M)6*aU1YM4`M-Ea;=7KX9$0I(>3a6%<0:CZA3I7"
p01469$=p01469$+"G5KN`W.BZV=HX]CAQ`-=H:AP<<`XbR*>_>.YaJc\a@;V(HBGN?R_HKcc?2Z.b&9V"
p01469$=p01469$+"HVb<WQP$^YMXR38E*6:>/^<=]_;=J.M<%%%IKI6GV+^,@1Sa)X=%.J9,SDS@IUQc"
p01469$=p01469$+">J,V<H1]2&_,c0)Q4N_+I$7K&3K*[LRHS+U8CbTTEXHFS-+6'B9NAXQ.2L&:B/b/"
p01469$=p01469$+"J&I=VR`%1X/)_J;0ZcMC5TT8`b8GQI9`CIAN3@L9HE/H08aG[34AN6CNFK;ZGKZ<"
p01469$=p01469$+"Y30Wa$S5?0GP3J-`a1/O`^$:Yc@MBK3*I8H@R&cbOa@TP&5a85PP;5+4[]LNY\*^"
p01469$=p01469$+")]SXH9;HBEZLH4NA+%6K`,@P?.5^0+N(=NTD(E]O)bYc*CIC.KR9La7]<`*NGF2V"
p01469$=p01469$+"FW9+0.)K07HPN0]5aLEK'DbbMA;K/6\N@`GAJ+XM6W?=?5TY_=[[/**)`4G>QW@_"
p01469$=p01469$+"C^%C4bc`YEaEY^bMBG5K+=_;T*=CM_;P5*1:8IGGTa<G(<V-8QHU.W5-:/ME<_OW"
p01469$=p01469$+"MP1>c/S]8aI6;=>;H:X@:I@A0_Z$b*?.P7-\4;[@/QS'15@/844O\'EWcIH;W'1@"
p01469$=p01469$+"]WW%),UVLX$NJM<F^B5WRI&V5-b<,0?):(05_GJFbBWCOPK&*W7I_9NB39-a:37E"
p01469$=p01469$+">IAcb3VT4EcYH%7RSD'EcQ:*a<>RRF@[%F7UT*N-W-S;J>E=[;B&(7.*+__@0%`6"
p01469$=p01469$+"C4_/A&DC$NAaV,/N,Uc1.@.FNE\\:4\&7SHDQ*CY12=?L%=]KU`/,\;-U=W`'W3)"
p01469$=p01469$+"<T5JZL84aK]56/G049>D_$LW5RT`>5b4c(B6/X*W;^?[[''a_P;J_QYWKT+1-aL="
p01469$=p01469$+"]TT8RLCb(EQ$+AIa;&O<4-V.Q;FW\Q_12_8N5<c,7F9>FB_=-WH]BaTKI-a,HY4["
p01469$=p01469$+"VN`3AG-2S$EP+0^J;=.Lc[B(4@QC'RXSM\U*SJb7D_\,A6@:KZK[>(CZ(=[1*H;W"
p01469$=p01469$+"3b8Y=%^FF<FSCbBCM3DLY$+O>)NE_a@W;&<B=YD.@6>@U\&HQR1F3/W&8@86%=M5"
p01469$=p01469$+"\)AHCWG8A+P[VUJVPW[[6RHaYS7-DI*FPC%BFII81'(W<8/WUc9+@QP@E)7'4>H["
p01469$=p01469$+"Q?N<T,DIS(MT8-%^Kc\`:+'F>.$=8(R&0%-[W:IUD*'\?+H_(8Rc&>+Y@=L*B773"
p01469$=p01469$+"-CFOPcMaI7K?E@_FR%204*+';bICD'B(6P9-VS,a-@b0AcD-T*>cUEB&TX*R1^)L"
p01469$=p01469$+"(U;9E-DS9-@$aY4F3M*EV&E.VO,;GLI1$NDAWb:O/8],^1SZ*WV)+DQPTOKX4KS\"
p01469$=p01469$+"`=*Y+YQ/?,^7V+1Yb)L9CL0;^_^T8/\P9M@a++JD,?D31AXZFZRSKV.->.@3;/5;"
p01469$=p01469$+"\NYA@+)cRD3.GG`Wb0Z^*^JKSJCU`_97Y*GJH\7E<[]\3S8EK;QE0+FD7X<a5DB6"
p01469$=p01469$+"25CR\bX'\-?*M`J<;RX]_N5K$,14K:1':B04/Z;b@Y8M,)>92867VDB]&,cC5ZU&"
p01469$=p01469$+"(V]27>Ua6]E9QNPX<0;X&U\H]?IO4<0b4.JHIV/42HGRP?FZc-*@cQT'GIJG4TR="
p01469$=p01469$+"SJBQAc9>YS]4B@IS7KW%B:KDCR149%,/*c=E.$EbPP=^cBKO_9R?*[-VF8QDHHTC"
p01469$=p01469$+"UWY[+34JE;YF`RO:X1=S3bD[P7Z(X1Y*JTB*'*N;T\GMI&2>D+5N3XV,8OIOT>(["
p01469$=p01469$+"?KZX;VH]8\<H$QFQVD@ObUP9E`5+X`B\<ZRB*9WJ1-WS_aS,P=&O-5J(C8P]NIQc"
p01469$=p01469$+"Yb.I&4\T=QF0Q='GN?'<UDR697RF3AHKU_T/J<>1(:1A4=.47A==c'*%b(F=1/EC"
p01469$=p01469$+"]$L5O[P-:&0INR_DPO?VZH4YK6Z^b.O8C,C/*ULAAVX]4O$A>I,;DD%ENK0YHH'<"
p01469$=p01469$+"H]RK1VY6c@1D(Y92\UQGE0F>;T7]b5?/$+>WR[%E9A$*H$7)?SQ^H/ON2R3\91&*"
p01469$=p01469$+",2%F8.Y5?<-2\IE$`<@Pa23PC-8``/;_RZM;b)'D*_7]@Hc`S4\BQ*/XO>Nc,O3Q"
p01469$=p01469$+"a'79?$LSa0XJ]`RYH$>1M/UL?^OZ,:8)b7R;9cOF+D%V0)'/CE6c876:0%/G,ZA)"
p01469$=p01469$+"H0''ZJTFc=XEIKQ+c:.N4T\3-5\:X^4.ICYJ0_/L5(-43X6:>`&C$YM/I8U_;S0Z"
p01469$=p01469$+"C@RZ^^9-DG'6P$EM$2D.c)*c[J21a9_<:a@>'HY>Y_3[D$8P<F?O0@GNA;Q,$QDH"
p01469$=p01469$+"-,URT6EZ[=>+R.T07']0P>@K/<6)/'$Q6cH[KK')%**W6BcLQbK5U7J8ZMNBN5]V"
p01469$=p01469$+"_=5)58&,c`[W+C])N7RW8\CD+YPGGU`X^5L`ULV_R&TG'DAL8,\QYSa/C&DV3:3."
p01469$=p01469$+"SD9D-N/3.SF<Vc])Y>Y61`(P*8@W;M-@&Y>*W'05]5K85%bMSW'-=a6N:)+E'`.%"
p01469$=p01469$+"F;X)aE'G<FIE1U9[ER>NT<E,96W7XT6JC>@<]c2SOY<S;I>RR^6DPVI]5cULN1GN"
p01469$=p01469$+"0?:5MH7_YNE%:..UH9\]YN%T85>O<.9WFN?R:+?Ac<VB\`+WSLQA__$BC5:%57=R"
p01469$=p01469$+"[82&])Y<(ZM:=9F6J5*/U35'N>(>&a:93RF9D2T?T&?c*@O,?'I/,IAbEDY^,MS("
p01469$=p01469$+"KQ?4@L6OQ%8C*a5@.I_aWZ+-8(IaI*%':>)aTMZKS<A&FZ-J%@]GL=A;2a+E=<LK"
p01469$=p01469$+"7%V.?W3JQ\]AOT^FY6165]$1D0TB('IAc^'$;5(Sc.[I=F]Z-*/D)'S/A02$SK:?"
p01469$=p01469$+"JC4@0_3$C7_)<_*KZaM(*9UC%;-S?^HT$.O+^:K&N6[UJQV:V\(/6VW0_4[4=J2&"
p01469$=p01469$+"3cCC'_@]Q22VPC=%'`5`L+`/W17R;&\Q]a33X.-%'.=O>6U&ZTcCIaPRbVOa.0J/"
p01469$=p01469$+"53`S9WGDH3Q('CAJ;bYX7>.FC4\31NcWM[8_^4^[G\^'$;SQ+O[&D_ASX*5/X>MA"
p01469$=p01469$+"$>7<1XL,X*F2bJG(Oa=`?H\GO]J>4>C^U2\?(U]`8^$L[NI+(2URO_E-c&MbJ-[B"
p01469$=p01469$+"X/A[[G=WG84O%='C$/$D*/&DC\1`7S$5Wb=>NO_<3V9Cc&5=1VY$MFE&c:N3^&\G"
p01469$=p01469$+"KJ[?,Y%XK]P^,W$A/$/&[<&V_b>?J';\Z:BR*VQ$<T>VYT2B_^:;B+X%6`EG23;T"
p01469$=p01469$+"-MO1L+GD,^&UcQT6(FV66$TRSC3FcTN[B294J>A@A.AUFQC5YHGO8_,X.%05WUMH"
p01469$=p01469$+"\XP?V/\TNVAZ8[LC[(c&\M4\Y&P%2b>X'P=[0V?//(8M`Z<)GDI9Q//LD\2<)ELL"
p01469$=p01469$+"F[A4WA>$a4P<E5`26=FH]^Q(B8:,+2_R&O<LEKK<MbVL`8ZU)^VZ'`(P(c$G\R'-"
p01469$=p01469$+"32]$O5O5JK]Xc$LRHK^='O64]SWBEYI,`DF<7XDU)M8VK%$W00]);T:N`U6?`N(M"
p01469$=p01469$+";Oa<M$4XCb>?ZUEJZb6K3c42<[&NO(17\=CKJS/<M\'(S\\VVAS;Q$++3AVL1TIR"
p01469$=p01469$+"7`@5V-H8abGA+?;2$E5-3A+C2[_,$[56>a'X&L7L4`<CM^KZ*BbGY(U+&@YK5bN-"
p01469$=p01469$+"54ZK`ERBQVY:_O*$A9WbZ09.2$-D7VY60HQFW@-J<YWMUYS]X`>1Ha0`_F=)T$@<"
p01469$=p01469$+"[*8@-HY0`_]R=/\ZG9DJKb-]4'`I=N[E.'J[HB9,HFD17/U-+O>[)T&+-T'\W&D]"
p01469$=p01469$+"QOZ@Y(cU+b_>1(JXP7OW6b:(N$86;S^(aG'cV\J4@S.-FW5b*:]K-.<b%AQ00BMU"
p01469$=p01469$+"Na(BA_T8X_79*H%MMLJ.*^)B.Qa2b7;>8$V2&.XQ,\_SBS+0O&>Q3IO?AYU/%8^Z"
p01469$=p01469$+":68/QU4+_b)9P7<C]CMO%6cDR)YCPJP+M%WHDLG4*6=?MKK6DT\a)9W$[2.:.`T*"
p01469$=p01469$+";*$?I`6VM4ML7;'84JE[?$?_6G)P&Z>.-JV6^V]D$c1W%8,BO0@CD5SN?A$F`L/+"
p01469$=p01469$+"^3MFVE`O)C\<:Cb/<4=PVVbA`3;aZ';0a4&;?a*KPKW5BZ&/*'c9RB/(NT]VP2\Q"
p01469$=p01469$+"X9L;/)$TX.`N]ZA0b7;(XAD<NW'X`;aO,L+;M/<FXb8R@>.b$_b4ZDbJWI9)'N(<"
p01469$=p01469$+"Y6CGc:3CU7\<<BU],C+,c:*AD@1$PTVTR&R8-1E.(\9<?6cZaW::$X=Q^:JR6/G+"
p01469$=p01469$+"`X.N+@YH37\I^>Pc;aGM_'+()5[A^(U5LF0%882aR5QE6*UKT0VaX`UDQ`bQ(RQW"
p01469$=p01469$+"D&%J=63TI=DC.MP=DR=O6Q3W:G==VT7$.EY.OU(/N+=X^E39<b%Sb/cTUc*-[/G`"
p01469$=p01469$+"?,VS=]R\@;QR-VS3S=$/%XQ='YTUZA1.)0FR8>*c2UK<M\T\])ON(;PZ[)$M^@B7"
p01469$=p01469$+".R3,DQGR$([a?''%\)BTL7@*(1OW(*=S\5/4K:^.B)MD.ML-b=7?]MN^]^KC3'AF"
p01469$=p01469$+"ARJ__AA>7L^\LR+]c5TaWHXAI&Ea)/RVWG6N_[/@@I*W:Z+Q%-O-@VHFV]K4PA:("
p01469$=p01469$+"8PW+`bbRJ+::$$PZB3@5<N(=YAF9*E4A=H.),_-:0:`U8FS;2Q`WC/SA7E,I)U-Q"
p01469$=p01469$+"^5*V0)=XcHG[U&U+[X;?X9<OAcR9CD=*@X.:>0)YHO;8OLG_T@E+C6E*2\;.G/(b"
p01469$=p01469$+"PZ)T1=EF&DE??;MC=+&<8-<5`6X@b[*&.X`,U*D>&ZIY'W?a,A_LYNcD-@<.GI/H"
p01469$=p01469$+"\a9/3/K&FM\.>Z0@EU3b+J@@6$D&bS1)H'/,]P1-`$U`@`?PA1)8^&=2%`\%@LFP"
p01469$=p01469$+";GZZ6'H0D71b<,D4A>;7JYRA?9^.@'(GMLN1$G$<A^D%Ea+aVaA%/1KSY5TRYX?7"
p01469$=p01469$+"bQ/O:-]6H=b?C0$&/9EG)CPA.-0THU7?%]4;L:Zb(D%QY-O=4R?XLE&542=WQ3Fb"
p01469$=p01469$+"C;-(H`ITE*+1+,16?^_BC&M*_0SBNDKRYUa$Q*ZKNY)b9_)1WT1F%&6XP<K/W:CH"
p01469$=p01469$+"3$J&),c.DW5G+/D@T\FFUR'E;XK.0J@\3V/E\R;@W)O6HFV&M7H::%aF;I8V&`,9"
p01469$=p01469$+"X@4>4^Q6MOV(FQMEXB2+9>2%Y3=@KDM7BKS.b.VG^G5?QV^O++&WZ3W7)I%[3Z`&"
p01469$=p01469$+"UbE>@G$:)L&N:&+DR&97L`\J)@BT_$0BXN2c&S_Z-YU1(O<0M+4W`Y?8+4E'NA:Z"
p01469$=p01469$+"NO:K?>&`*1N`>%<@_-+6(0)*BKUT4:E0J'*\cN:CR%?W7`K0)ODN`TSUV_MX,MN2"
p01469$=p01469$+"4)^49TCH2)J$9$;aUb=-]+BS_]M+a>\-/K8%B,14.-K@=`(%Ya3.T:[c@ASNW>G:"
p01469$=p01469$+"2.8N6.,^<bW5UR)MVI:@`%^=F$51aU^^2C`9Z;<MF@,c74.8cK+3T;;>T>^/'EC`"
p01469$=p01469$+"ZHC%Y:1&58)%<]',%]H\E2%^\FbUL46JG>:-N9=)30^]JO\bM>K%YVF4`EPKDB.Q"
p01469$=p01469$+">4&R'U*QIBKbW&J'P/OB^<S9?2K%J\/]*1_Y04$+6c=c8^;1IV0UW]0V8K3W+SJ-"
p01469$=p01469$+"Y8&ZYT=FL>=5C@H7068*//JS>&@&'W%\(D[<SRIU5447AR&*UCIS`(X@8G6;2-XA"
p01469$=p01469$+"a=<NK2P.>84bcQ/>?X/U_,8?,BSbB8902)NVc`D8)UF/TF.]L*ZM=(c+R^5--V?7"
p01469$=p01469$+"c[R5_%PR(G%33,)4(']E@REQa$S9_M/]AQ-,UZ[ODT6*CP$84Pc2+0B]E$:)+X@F"
p01469$=p01469$+"B=+=.48M]N=.LQ8%H^J%U3^YY(`<1]%^POH\:;6H[WT6^_40*I:8R'<G4;R,\_S3"
p01469$=p01469$+"B7Xc+1U(DO+G*4%L==+Ia>@/+F'_L%BC,U-Y$b.XY>5R8*D+SI.6L=2%.?5aA5$Y"
p01469$=p01469$+"R)8K\b>b8%;,CTJbX0=W`U3MYTa857TIbK<-++$?^FP8?&-W'[GUX$aXI%M&)E'7"
p01469$=p01469$+"(cW-W56=:HQM?-9U0^N.Z[:G=NYF5?6\OXQ,*a*.]10?4TEI`]+bV@J&&&&UJ:-V"
p01469$=p01469$+"L@c3DMIab'A1+)$/YQVaC5\WLQ(PW9\=b*7J>=GM%aHIXHS/IA2+7B%8GFWIWDAJ"
p01469$=p01469$+"-NPO@=KS=.5CR_V,I-CcVA7$22R%03/8$,ZG4_V9-`\6L``%XS0FaXWD39)M@K,@"
p01469$=p01469$+"c-KT;NH\-@39F.NS(MV^\=4G[1(E\`B@AbS_a2=+4Z^&%1$0*IMZ`@.@2+*B8*P+"
p01469$=p01469$+"J3IH%08aF-*S8^M=['`V$-,N$aFKJOaF/`NTVF2MBAXNRN[5]@b)-BA]V/KU_]?0"
p01469$=p01469$+"C%D]RA%&_H3<DU$cMHN)JXW>]@bO5:&A/<Qb:X?'RI^%?<`]1a=Aa2'&[:16Y*N%"
p01469$=p01469$+"Y0HZSK.XYB+PFF2UR\>aM)^(FA+3.MX&?7b;CZ_HM?[HU$&+^aNb.1^]K/IOF$&)"
p01469$=p01469$+"TLQbP.:^5@%*L/46(L,/V/R:52]Q9+&Mb,=L8V,2FHT9OWM1>FRP5&:N[%.WZb5G"
p01469$=p01469$+"=<N+@L-)Wc$=O/LFLJHFLIT[01Z[L\&9KH]&+\U?NEI9O,Y8JUZXCOFMV&2Z<_Y$"
p01469$=p01469$+"R4,Y>K8K%IFB,1KMTL;.<*?$8$]H*93%4(L$R0*E//./9=0U<LU-XZ[WBY>a,U.Z"
p01469$=p01469$+"^aX8*:(CaZK$X).%)A8I=LS/([+<D?/DE%XUQQ8JW;*?R]%\JH7I3:`D.=K.,H]-"
p01469$=p01469$+"V;^2_>'`:DaRYK0a-bR2,&0$ZCQcX(H813Q-IZaJFF&)2?Zb'TW<]OMOEH.29$5$"
p01469$=p01469$+"6GI5]I58*/ZEb',03T/&?\<I*TL-](O&BV$9O>0%82S.%(5+.QG199;P>,<]`MN5"
p01469$=p01469$+"c=<:Q$RWYH*;@&K1&-K.Ic@`R)_KOR4%HW_8U$3OaRU'T_5PF6@']^RB;>D/6P^R"
p01469$=p01469$+"PYR-cQD^SB3IJHM9TF:WTYBOTKcFZIZJ?6;'CK0?^86>LB2CP?AH@[,E9=9DDW@B"
p01469$=p01469$+">B*FMY7O]:]G@UWIJM\F0^T'`D5V*ZUYS'9AD.-\+G;$68RQP6]W'2BM)U(.=2]&"
p01469$=p01469$+"GGWB@:Q-0R+U+4cZ62?W%[aVUUW(TD)S5ONPBQMZ[*)^0cHRP]`5D`=W<?&K1<)5"
p01469$=p01469$+"<*;@]A^*Z@RB^U27MTWN2>D^bTWX097Y-,:%\^*(\`SH%'aBT?*N_`-:\8^T1H7/"
p01469$=p01469$+"XC&3b+T@>4X&FEFJDX;C/UP^]23E=&25$((<*c-`;cZB]Q`_cRAYWKXXCDLN;F9]"
p01469$=p01469$+"*7)a0\5YO34OF>&c%[?F@C5+GSI&?:1D=;&_;2Z=U(`^>MY/+<Z-&:HW`@DG)G.@"
p01469$=p01469$+"=+H&L1:TX@/_Rb;/Z'P;,8J$8aF994$%"
p01469_bmp$=UNCOMPRESS$(INLINE$(p01469$))
' output of inline.bas for X11-Basic 04.04.2011
' /home/hoffmann/ttt/bas/p01705.bmp 16506 Bytes. (compressed: 5220 Bytes, 31%)
p01705$=""
p01705$=p01705$+"'>/LH\COcc7^(<Z]ZNG+DV3R4D5cO@[)caP(A<L(a8c^$M,XE=\PW,)A.R>L2I:b"
p01705$=p01705$+",A:??4M*Q4U+M*S3Z>TYa./\X(LC>PP%`E/2@5PU^Q9N_3]-QFVb`E',M&RMT@+\"
p01705$=p01705$+"T0/6X7-OA@<Z%OB0T,N0OH63acEA3'`9CJF\QDB?&4Db7.',*E^GRE51BFI*%0'N"
p01705$=p01705$+"G2S/VK`E$$4:E^`TZH'@3<05P)8]@0++C[/\-Y&Rc,PY<_'7'WRL)<F0b'JIMA,("
p01705$=p01705$+"TF];+UGSOQ<B0QES6)J5C%[a(N1D21+=-MA(,5J`OU956Y.a2X)WW@4cBD5IC3Q*"
p01705$=p01705$+"G:/$&76GL*'%`,6DW]='.%3ROX@]\O>O'L^IB&>SGL)E/G')2@b>T)I0EG]V_9_^"
p01705$=p01705$+"^0@FSO>X?[NLUXR8]++&;AT[@J';)GQ/6])KSJ&Wa$<5CAJTF:RIH8:9a:cQWc0V"
p01705$=p01705$+"[V2+A)a,^G%<1-TOA0`%0.'5W,@+;=O8V8K\H'cWa+5NQ;aZbTK$@=X)`T/270^c"
p01705$=p01705$+"G6-R[4;FZW<b<.GOUWH,5a4+7@<7O7SQ(P1%/H[_&^TKAab%>*T>/*(BM4P_]FH&"
p01705$=p01705$+"ELA,%1Z(KOSN.13@NM;;$?b;PQ1\Gb,<MU24)>EZ(BZU,(26\C[_L:b&KH_YPG7K"
p01705$=p01705$+"@)^'bG?c%Y&;?J6'NCU>_/<-BO;<JAC)L@.7+,]1V;L0MI]VSX4VB$'4Z-VI>)FB"
p01705$=p01705$+"Z($%c7ZLFcU?:2F@5DD$9/_&A%KFH=&FH,ZF`OED<`&2%8`<98RM+B9O.b>8LOAF"
p01705$=p01705$+".PWU-W_\:-O0$2DA5%SZ]A8ZR*8_N.]9*/[X*R-FI^W39SSZ[=GTJRJX;K++HX?J"
p01705$=p01705$+",N3*'.H,)V(T-Q3+b[J><AP^2WW+bPO?\`)SN19J3UZNJQ]+13:JJFD/-W%30$_W"
p01705$=p01705$+"BZMY79.4PP2%2`44XOF3A=4\9/*^$HA'^*%-'4@\[&RL?7(O1T\CI2TGANX.PN>7"
p01705$=p01705$+"W300D\:@UW?2Uc62-J7QL3N]L<5I.6''A^F/D3[Z:2P+=VVU8WT_L*Z+$/Sa&&)V"
p01705$=p01705$+"Q\X+Bc?N&,/Q]?)VbFVGM<?5GH:J3O2c:VQ<3DK-B8+5`/'^TY7;=F+,PQZB@_QF"
p01705$=p01705$+"YRP/%OSUO)1+B[[C_MAKR_^)X1&>Z1X->./FRS3YP%318O=3S$;<TWY9^?9HX.U="
p01705$=p01705$+",53=.P:Z<:I&Ta:=']B4=;c:?H$+HZ=PHb<9+Ua]3,X8DNKLTUL:b\X@M$</9=3C"
p01705$=p01705$+"2-_8(X)J74G[cE8>'CPC3bGAH]'@J5KGCPH/<J`ZH8a,$'18*^a`,-GRF<\'QB>2"
p01705$=p01705$+"Vc:U,@L9a[aa6^PH(0>;&*;VIHDa6>K:`<H>IUP`+^ANX*2HTXH/YG%O?QX[;TN/"
p01705$=p01705$+"5c.W:K^NM@F]5V-I&LbH5\%cV6_`/$Rb[$T=>OG&ZZJb2YO/7Y=0CJ%[)I?c>-D1"
p01705$=p01705$+".LWZ5=E;YbMZ:5N>'PKCA$XYb&(&/OFB_3TX/G@Z1.IDQN@$UV,[5%.%O4T2L7^+"
p01705$=p01705$+"S+<N-TaV6bH8,^/,_Ua1N)3)K]aX]F1cBZS[3M)IHP&F>/;N5(P%FC,(@aPKWDI."
p01705$=p01705$+"3b3^=F['YU),A7.YD$%FW57'K<=I3WE8_^cN3\?PCO5(Q+6.J+O*=_<LIRHH)UD*"
p01705$=p01705$+"Xa6$O+OB(RN7*')_+;3L>FA][O?[;,b68TK-=Ya-`;9cc5WE-K2[K//7QOO.Q'OM"
p01705$=p01705$+"W@?BBRC(7_)WL-a`=5S*3cQ&C]1/?Y*$HYPO7&8SGK9G/Z=TAKH`$Y)=@Z&ZD?/("
p01705$=p01705$+"F$&HNNH(A>W.^N4+*Mb6YT6`)?2c9?R3-;GR:U5/KH><+HB@A.C)NSH$<Q:Y^T26"
p01705$=p01705$+"OMK>bJJX2IN7^;H;*=+2C]0MU7&9F0>4G*<%[2bA+9/95D'5GRE'-VR6'&(1O23\"
p01705$=p01705$+"b$(]_%>N+RF?^[K(V4BLZY70;&bMP2FE,:M)Z)*(-V*[9OJ$'JS?)?N.=WTDI$VZ"
p01705$=p01705$+"[E@)$&B'Y/Y@VbXMT>4[[?BN?>/`V4/[c39:W+KWT16>9NPIT\/AYG``G?KR64()"
p01705$=p01705$+"I)^=;@M.&B37KQ5Q_QD5%3JQ)TRQ45QOITQ)('@*+^-GN<>ZWCR8]Ea<Rc*7<>?A"
p01705$=p01705$+"42ME.'Z+c;>SUK92F;^(;%?0CG4WaB+41'?WVB%4PDYK$>a@UFR;9@cSK/GU>]a/"
p01705$=p01705$+"7:N0B\TK/?$2Q0[2T&.%EH_>'X>(:Y%a848+0/)X0B<:O?R`a3;UW6EXX@;6b<LY"
p01705$=p01705$+"XD`.2@^b7\PN'Y[WWWTFK4X\aCSH(PBB$Y].>`*E@P\`];JOJ+8H@*1['P`c<:=T"
p01705$=p01705$+"-L(&$>a^C+>8<_=4R^2c_*\PY*OJC&%;40QZ?\=;&)PT)aL($[b[?/+KB<cD\Y_a"
p01705$=p01705$+"YR+X$+XM.BS:N76<_McH*+a-$6S]W(10R7@1U:+`BX..]<Z[T9ZTaJ]SBD7MX4<I"
p01705$=p01705$+"8_)0%G96:NFF+3Z3=8SQ?].1,*RYLbTK742_EO3WTc^%Y7,T^/`<HK$R1M-ZO.Q3"
p01705$=p01705$+"$%LA-/QW=5+8c?_cO0VI8]-KX4;*U_.9$E0K)?RIaI3^3,D@K6HO`$DJJ0[LA']`"
p01705$=p01705$+"\7DSIWQF<&?4SQ*X?KaU>;;%cVcMKG1G+2/\C`M[F@<NNE^\2<UXK8$c;\/24M'T"
p01705$=p01705$+"+1>c::?6(]P+PR_^FYU$3',V@5X7BCM(YQVG<bT,_I01N5*/[8I$4)N>1.<PZLNF"
p01705$=p01705$+"SO9VRZ]/NN[/20K^X*)[LX^-6T.'5,b\>'.6`N>A>G'RO:N5)T+-`)JUC/0LY-Q0"
p01705$=p01705$+"_B0U[)@PTbX1CCc;[+*._.JcU>JW/O&QOc5?4LD&=F^IU]a[1%2GI?\P+F?/OFE."
p01705$=p01705$+"<8bG0C:-'^%04^D4,]^GO7A1]S%AF1=M&+:80Y?(.%M&$<753Z.5=.OL)1JN`F7>"
p01705$=p01705$+"<4VB?0=:cG-bJ*@/J^a)J(+%ZT4QJ0AGTP%`03&VE;?9b-_6'5Ja%/0+3AVNW3@Q"
p01705$=p01705$+"L?$*\PbHADR`%GN&Z;])X2^`$5W0`1,TbJ.;]35UQ-&5A4B$+8$@Y[7V=M;-_J'+"
p01705$=p01705$+"1J?[ZQ/ZBS@_]^_276O32$J(;55M<WXOT_Y_Rc_IU6a_G&GYSM2E*WD]bBUV?^91"
p01705$=p01705$+"Rb(.%)?]8==,+_3I1.aYH6A7N<4MAB?b]@(8]`]$$]%`_G0VHY>.*c=HG;>/I9cL"
p01705$=p01705$+"SFW1Z,?&ECS3T&P+GXN?6U;4[Y9RF[(+3'/`LY.(W3\ZC4O^<X2)L6b$a:>P%D3Y"
p01705$=p01705$+"AU:Y)-8S=X9\PDDcYT:L0\M_Xa_`K8>@/&I_4HHC$)SM:M$FCC.2CB\`%=<;&P_2"
p01705$=p01705$+";*BA&<W_\HO&D<GEMG`CF.&6M&`9WWM$Vb`c_6J0AF^`].@RF)D40$]RMP$5,'*T"
p01705$=p01705$+"DB<SDRcHOD%&9@<_;%(`;aZ?;RAHa3%]&<GJc%TUO)Z;H)0Z03Y+G+D%)N\O/b*P"
p01705$=p01705$+"=_:N<)<^G]OKXD.@OA_TGJ](NY4A77GL6Q;]H/1FQY7b_D::^W?bJJcCVI:CS^O*"
p01705$=p01705$+"&Q2C>)5I_^8,@M;N8'6$`\N>97--P)&C.^\D@;)_F`/(c`WD&,2$>KQWZ\a\8%N="
p01705$=p01705$+"aY.?`9NZ9LRP$H?0/V/aWB`:-X`P7)0-%:aBb4?L\7]M2`Hb5>90]U&$:Y%?SR[$"
p01705$=p01705$+"P^KFONS\+NCK[XRO[0)6BZ&QcKN:32SP.$D`A%M2-$ScY5Y9.338VC7;Q2+8'3RO"
p01705$=p01705$+"96LOAHSYZ\2:c]IB9M/7R\+:1R;P@E$\V?G(5aD]F7$=YN>X,FA&3*37X8IA^JL4"
p01705$=p01705$+"BXE21S0RLFXba3M\WX86NbR/,O0BI/C.1K3RDGVESO$.X3,@T7?'OQX8-$0-8_(J"
p01705$=p01705$+"6PN99J6\[Qc'P?47Fc`2$5)K^M\H5&@UJ:QBB9T?-Q4\3]HV\H)SG;&PS^)4RV]R"
p01705$=p01705$+":6I7V%^ENE_c`)F.'7@;HD,XNa+CRD+60F<ACWJ]aJ>'c/=6Y*+RJJX)@A`*,].4"
p01705$=p01705$+".$bL-bV.U7'5`b(^CQ&LB\\%G;@QX)N<IU/F=Y>7H*YaOaA?&,M<XH=(_(W%O9'K"
p01705$=p01705$+"E`E)>7\<963.4Q5^aF0V@.X];IIE5&@(8b36-1>4:1%@?4K@$_QJT7M1D'95JN=S"
p01705$=p01705$+"O=aKFP&9^>SU>F\TY^IA[F>:=JbX]9)Z3`9@Z+/%8T(JJMX4KC`U@SbcUO$c%5C="
p01705$=p01705$+"[O3DG__=;_S_c/\J=H`4+I2,*CLPJISaA_B`VV_YOFZca:Y+[[=CTL]:[RQ$=2>N"
p01705$=p01705$+"(UV&T`H^18ST_O4H';;c7KF<5D'CC)V6[8R$[D83ECG,YJ:F8:b76CLJ>C,TJR+;"
p01705$=p01705$+"[)TRU9Ca9*\<&$5;J.X<G-5DYG'(HV3-YcFQ&;U>LR(cU(a5C)Hc3*a2^2QG^bSO"
p01705$=p01705$+",TAF@Y4FS/-_E]_F9IA*8b>&U8&B(<A-M6JGSabT+*=O981D-0;K-7+>J6W)Xc9?"
p01705$=p01705$+"BT@)OVD/`[EOD\(TMYF?SK4).>(Ob%]9<.HN\C6RVHA_ZVXQJ.\F?;ILPE@>N$M_"
p01705$=p01705$+"$VX$M.OSbHU41&%Fc*@O.5%N(MaX1^]4$>\>]Wb-@L@BbIRJ`W?KZBA2;H1*N[=^"
p01705$=p01705$+",?O*2=>63S%U$**/Z>@`a*T%FUTV=cVbU6:3I@Nc1>0*;C`;).Y.Gc`XSC=EQDBH"
p01705$=p01705$+"OcC7O([3SZ6L]>8C'+PCTI=X25W<Z@/IR3(0(-VMD349[[1;D+2L>bL@QP0V<41F"
p01705$=p01705$+"SbN2c,R(OH<U3PFG-T7A>*WC6b9MI\b[P_/):OO75G>O:aFTV()$*0/J?)EN[<LV"
p01705$=p01705$+"L;Q,SSXQT55D2^cVQP`2L[Xa@D;_bW]U,;EUWROHF3[P\bQY;L0b<:\9`0$YWU<a"
p01705$=p01705$+"U*YA\<3FN[6).0)(NU(E2Y\->L)L^HIRL]*N2McN%2PLa8:.I>N[Q427@74?'918"
p01705$=p01705$+"NU\$:.<.B?>*T6P)EQN]ONUE1b1D7DW&=DA//6_/7J>>X74_L:(/-@cI@'6=NYCJ"
p01705$=p01705$+"OWI]ZI%6F0=I_M9?:0G.N&BC[@D_+0=.&NPUCS?XD`S'KHJNU?6:U6$9SO)^FL=9"
p01705$=p01705$+"53KOR;Y,E*8J\?\T&@7Jc6>Y5Cc$)E,RT<>)L;<8/'=B2Q;=`=)X$C:QE]9S:5;G"
p01705$=p01705$+":U9UD^b,Q(UQZUD]LR<E4SW'L\/(:@1ZJ2P<,Z-63[>a-'$EaT@U:W2OY0>;/$/Y"
p01705$=p01705$+"?X*,%_U2T.K0c]&R9'WMU(3cD&O92ZGQ](_US@=,X-ZC-b5+(Ub[3`@Q8>.%I&4$"
p01705$=p01705$+"6-6%W<cH%LK<XI&+>'<YV$KAP\UIULFP)F-[+,9]V5C+DK[0YR6I$F$-cN4:--&`"
p01705$=p01705$+"@@]B\YIKP8H'8F1C4A\8aPUP$N+ISC8ETY79EJ5(>A;B2IA8G9JBT0.)0^W]O]A?"
p01705$=p01705$+"^I_WZSI))2c'8H+8GAPZF&-D)+.c;6Q=)876&?<?O/5W$T2&PVN-9N'8[X.OC^$L"
p01705$=p01705$+"FbOX+5a*HTO3G7SY,I*>S`.-HPTRO(a,2L:.@8,^[>.CMZDY/(DR0T4+%6VW9=c8"
p01705$=p01705$+"^aZ.3c@CK6'/G]=/M2=N`KU*D&(M38D>B5.ERcT;*'L?3*B1VZ/TKKO\0>^PI(O5"
p01705$=p01705$+"\[EE_354aOK^K[/8b0&5=GN>2VB^KL+-Y&W-,B]QHBSPRHRMH=]G9H?H/[4B\QLU"
p01705$=p01705$+"8RP,LEQ'N=>ZD^R]DX++O_WbK>C7cF--b@8EK-\<c/;XI-<Z,5^0%L-41*.aE^)."
p01705$=p01705$+"K^OTL@67W*RA],*`Vc%KN:Ic\`KY`K_c,=>AX&YHBKXbQ,U5>%F*Z=Y/.F%bS_$K"
p01705$=p01705$+"<-$\&\@0NQ.^?C4G5GLME3;OB2<EN7<%5LAP0:>J40U$^A[5O7`+MC'($NC0c$_H"
p01705$=p01705$+"a&3;H.$HHTYO2%\06;c$?BI33a5@cYC`HXS`a>cN.),6_Y]O8Cb>/E''=1A8N7L["
p01705$=p01705$+"3S?($<EMUY=[1,+B2VBW=b\:Lb)O4$&J`GaKE3<RJZS&`2>6R`Z+4*RS^2[+OW'6"
p01705$=p01705$+"?$cO:b^PA?SB9)]XV3<,'H>H)_+_BU8[T\'\b:[X?U?<:$%B,$b@NE?RP@bG<]&@"
p01705$=p01705$+"WCVJ-bX-N4BCNPWc_H)>;T^CX.4cUJ6[]S%RPPMV;.GN=E>A(MI+`M-QNRT8ZK;/"
p01705$=p01705$+"9TOK/,@8[VIE-LD`5&PG\W$MBC/cD(*<Oc1Ta:b7)T;&\(cV$;/<:[9<+$LbGVT6"
p01705$=p01705$+"$bH6HBVIE6OG`2X=AQ?,7,PJY9cZ=@8b6IA=STS981>HD-\6&aCbHP7b.&NP8=)%"
p01705$=p01705$+"1K3F--84C>A.CE,G0*aB=2N9`^5',*?Q*bBA@/MX0aJ[(QY[2/aLGb37<03M*D8?"
p01705$=p01705$+"I:Q.5$IWK*M=YV_S;M2D+`GJ$R=A=DZE9HD'IOHU`4Y(K*9:B[R?7.E]EJQ\\N<S"
p01705$=p01705$+"4.%ZLZ-E`9,T50$C6+Y?X7=3\T^G9N=\K;$YR9DN/3^,2T+D',WD2./M%9>32KV0"
p01705$=p01705$+"KI`OX&_(Ub*@%]OZAALS8FTG2K&G=UTIRB>2,Y$,?b'^cD3)_DMaC%C6\_4'+UbU"
p01705$=p01705$+"00Y5W6&^CT$T>)c=1OW?>cH2N6Q1K<3>8+:;MR[[_WGOA&%ZaV[/HW`BD.\$8GZ3"
p01705$=p01705$+"=D5TT=.?G/:;>8)O_;6/.F>DLc%`[PYY*$B@K>]aZ:UY77=WBWR]AbACQCYUcME]"
p01705$=p01705$+"a,7NN]a:YVC,GcBX+bA+WVcV>1BE:V(09PM5)Da;S5.T9ND5U0A(cBAUUUa)8/RJ"
p01705$=p01705$+"VZ3<JS=7_2<P'E?$J)(:aH2-]-WOHB:B?U6(0c&O7CW<A?Q2Jac+)5S0MNJB<,b6"
p01705$=p01705$+"a6R^2VX4.U-U7WS?X6;46OE%S:Z.O<\X-BFXbRGEK.86VJ7)]*5b/0Ec8(M/16Z`"
p01705$=p01705$+"^%G:I21\%PA\*_YI28ZO)OKK-;/aF+(<.8V(Y,PFA8RXQ;R9+X&@K6J&92%O-GUb"
p01705$=p01705$+">YU?68W9SCGPOS_UBb\JGN/DW=/0M7R0WEBTU/)WHRE)P)S9\?GG$$LEO6E2)(U:"
p01705$=p01705$+"G2^LI`*<3CW<FXQDIQ7^c1\Y4GN*A=A_(c[DZ:E4H*IVLUP=4I2')D9?+25[[U66"
p01705$=p01705$+"ZNNBA0U6<LP(Q^bE10`1Ua0^9FI(]c=Y]:LV$'<BZ*+8+J[31`V*&QMU^^V<4J*^"
p01705$=p01705$+"TAT?`Ma6PTI0A,?UP>Eb0W+`7)_'M`bQA54%7WJ9HXW\?8`,LZW'AbL^GHS/U\E]"
p01705$=p01705$+"6`:;/?@5Z+.cC%L+NMPOSc/JN:(O+@AVHOcVcOJ&N<,`24]$"
p01705_bmp$=UNCOMPRESS$(INLINE$(p01705$))

color schwarz
pbox 0,0,640,400
for j=0 to 12
  for i=0 to 19
    put i*32,j*32,p00225_bmp$
  next i
next j
put 32,32,p01469_bmp$
color gelb
deftext 1
text 100,170,"GET und PUT mit XBASIC V.1.16   (c) Markus Hoffmann"
text 100,190,"the animation is done with bitmaps from files."
num=6
i=0
dim dx(num+1),dy(num+1),x(num+1),y(num+1),hinter$(num+1),kugel$(num+1),m(num+1)
arrayfill m(),1
m(2)=0.7
arrayfill dy(),1
for i=0 to num
  x(i)=random(600)
  y(i)=random(350)
  dx(i)=(random(400)-200)/50
next i
for j=0 to num
  color schwarz
  pbox 310,0,380,40

for i=0 to 10
  color get_color(i*6553+j*(65535-i*6553)/num,32000+i*6553/2,i*6553)
  pcircle 350+i/3,20-i/3,m(j)*(20-2*i)
next i
get 350-m(j)*20,20-m(j)*20,2*m(j)*20,2*m(j)*20,kugel$(j)
next j
do
  get 300,dropy,60,70,dh$
  for i=0 to num
    x(i)=max(0,min(x(i),599))
    y(i)=max(0,min(y(i),359))
  
    get x(i),y(i),40,40,hinter$(i)
  next i
  put 300,dropy,p01705_bmp$
  color schwarz
  for i=1 to num
    graphmode 1
    pcircle x(i)+20,y(i)+20,20
    graphmode 2
    put x(i),y(i),kugel$(i)
  next i
    put x(0),y(0),p01277_bmp$
  vsync
  graphmode 1
  put 300,dropy,dh$
  dropy=dropy+1
  if dropy>330
  dropy=0
  endif
  for i=0 to num
    put x(i),y(i),hinter$(i)
    add x(i),dx(i)
    add y(i),dy(i)
    if x(i)>=635-40 or x(i)<=5
      dx(i)=-dx(i)
    endif
    if y(i)>=395-40 or y(i)<=5
      dy(i)=-dy(i)
    endif
    ' Kraefte
    kraftx=0
    krafty=0
    for k=0 to num
      if k<>i
        l=sqrt((x(i)-x(k))^2+(y(i)-y(k))^2)
        r=@pot(l)
	add kraftx,r*(x(k)-x(i))/l
	add krafty,r*(y(k)-y(i))/l
      endif
    next k
    add dx(i),kraftx*m(i)
    add dy(i),krafty*m(i)
    dx(i)=dx(i)*0.999
    dy(i)=dy(i)*0.999
  ' Zusammenstoesse
 '   if i<num
 '     for k=i+1 to num
 '       vx=x(k)-x(i)
 '       vy=y(k)-y(i)
 '       if vx^2+vy^2<40^2
 '         s=vx*dx(k)+vy*dy(k)
 '         s=s/sqrt(vx^2+vy^2)/sqrt(dx(k)^2+dy(k)^2)
 '         w=2*acos(s)
 '     dx(k)=cos(w)*dx(k)+sin(w)*dy(k)
 '     dy(k)=-sin(w)*dx(k)+cos(w)*dy(k)
 '     dx(i)=cos(w)*dx(i)+sin(w)*dy(i)
 '     dy(i)=-sin(w)*dx(i)+cos(w)*dy(i)
 '     dx(i)=-dx(i)
 '     dy(i)=-dy(i)
 '     dx(k)=-dx(k)
 '     dy(k)=-dy(k)
 '   endif
 ' next k
 ' endif
  next i
  exit if inp?(-2)
loop
vsync
quit

function pot(d)
return -exp(-d^2/20/20)*100
endfunc
