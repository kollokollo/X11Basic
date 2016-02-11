' Demonstration of the INLINE() function in X11-Basic
' this is a good way to include any (binary) data in your program.
' The encoding is done using inline.bas, which comes with the X11-Basic package
' 
' output of inline.bas for X11-Basic 23.04.2002
' demo 104 Bytes.
demo$=""
demo$=demo$+"5*II@V%M@[4D=*9V,)5I@[4D=*9V,(IR?*IR=6Y*A:]OA*IS?F\.&IAI?J\D8ZII"
demo$=demo$+",*5M=;1I@V%P=;1I?F%OaJ]R=:\P,*5E?J\D>*)X,*9W,*AI>ZUE@+%X/F\R&JAV"
demo$=demo$+"A;1W&HXR&DL$"

PRINT "Original length of data:    ";LEN(demo$);" Bytes."
a$=INLINE$(demo$)
PRINT "Length of the decoded text: ";LEN(a$);" Bytes."
PRINT "The overhead is:            ";ROUND(LEN(demo$)*100/LEN(a$));"%"
PRINT a$
PRINT
'
' And here is the same with compression on:
'
' output of inline.bas for X11-Basic 18.04.2013
' demo.txt 561 Bytes. (compressed: 387 Bytes, 68%)
demo$=""
demo$=demo$+"b_FD4\\'UU-%RIPK\cV^-c%c'_4GZ+a['[]RD+X5b.<ZU@BAG]H1%S=U*='ISL<("
demo$=demo$+"L&8Vc?'KS3M@CI1Ub2a3_JMcB.b3`Jb>BAL_A>?<6VcGYA/HDF-<[YEcaO*B?)[-"
demo$=demo$+"OA0-&$O2Ec]0'A[1JC4QHR3[2>*_NV0b^L3AQ:N)&GY>`T'-KOV9[Y*;*+)SQN'*"
demo$=demo$+"VIIU\;^$A8P9MTVDM2H3KI%*RT:EBV1'^B0DZ^Y'Vc82=91\\>DPA`%?]V44WWR+"
demo$=demo$+"=*H-GP(586ME$W)U0F[/,]Y^K^-O*$]^N4%P;/Y>^<\Zc:XA?JO+OE@^W8R+UTYa"
demo$=demo$+"XH]?8][ROHQE-_9UGW$b-EH.<\`@C27;7.4JcP%@N):O@ELDK8^$bPGK</(:6]HV"
demo$=demo$+"L<-&3_NKW61'<9%bL&?FTSKWE;&OPDW,\+*`-<)GT_cNDN1:BF8;C:+NRV'=,V:."
demo$=demo$+"<WS<O`,T$DRXc(c)16,QQX31W,X>=+%5\.=^AKYJ27X_.Fc,_.YaI>B9A\:XKPJ0"
demo$=demo$+"_M4$"
demo_txt$=UNCOMPRESS$(INLINE$(demo$))
PRINT "Original length of compressed data: ";LEN(demo$);" Bytes."
PRINT "Length of the decoded text:         ";LEN(demo_txt$);" Bytes."
PRINT "The overhead is:                    ";ROUND(LEN(demo$)*100/LEN(demo_txt$));"%"
PRINT demo_txt$
PRINT
