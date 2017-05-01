' Utility to convert gui Files with Formulars or other Graphical user interface
' Objects to X11Basic executable programs
'
'* This file is part of X11BASIC, the basic interpreter for Unix/X
'* ======================================================================
'* X11BASIC is free software and comes with NO WARRANTY - read the file
'* COPYING for details
'*
' (c) Markus Hoffmann 2003 (letzte Bearbeitung: 10.08.2003)
'                                               27.01.2005
'                                               01.05.2017 Korrektur W. Schoenewolf
'
DIM iz$(1000)
DIM fstr$(1000)
DIM fstrnam$(1000)
CLR anziz,anzstring,anztree,anzfreestring,anzobj
CLR anztedinfo,anzdata,anzbitblk,anziconblk

inputfile$=PARAM$(2)
chw=8
chh=16
OPEN "I",#1,inputfile$
WHILE NOT EOF(#1)
  LINEINPUT #1,t$
  t$=TRIM$(t$)
  IF LEN(t$)
    iz$(anziz)=t$
    INC anziz
  ENDIF
WEND
CLOSE #1

PRINT "' gui2bas V.1.02   (c) Markus Hoffmann 2003-2005"
PRINT "' convertetd "+inputfile$+"  "+DATE$+" "+TIME$
PRINT "@init	     ! initialization"
PRINT "@doit	     ! execute forms"
PRINT "quit"

WHILE count<anziz
  t$=iz$(count)
  INC count
  EXIT IF t$="}"
  IF LEFT$(t$)="'" OR LEFT$(t$)="#"
    PRINT t$
  ELSE
    IF WORT_SEP(t$,":",1,a$,b$)=2
      name$=TRIM$(a$)
      t$=TRIM$(b$)
    ELSE
      name$=""
      t$=a$
    ENDIF
    SPLIT t$," ",1,typ$,t$
    IF UPPER$(typ$)="TREE"
      IF t$="{"
        @dotree(name$)
      ENDIF
    ELSE IF UPPER$(typ$)="FREESTR"
      @dofreestr(t$)
    ELSE IF UPPER$(typ$)="RSC"
      IF t$="{"
        @dorsc(name$)
      ENDIF
    ELSE
      DEC count
      @dotree("FREETREE_"+name$)
    ENDIF
  ENDIF
WEND

PRINT "PROCEDURE init"
IF anzfreestring>0
  PRINT "  DIM freestring$("+STR$(anzfreestring)+")"
  FOR i=0 TO anzfreestring-1
    IF LEN(fstrnam$(i))
      PRINT fstrnam$+"="+STR$(i)
    ENDIF
    t$=REPLACE$(fSTR$(i),CHR$(34),"<&&&gaense>")
    t$=REPLACE$(t$,"<&&&gaense>",ENCLOSE$("+chr$(34)+"))
    PRINT "  freestring$("+STR$(i)+")="+ENCLOSE$(fSTR$(i))
  NEXT i
ENDIF
PRINT "RETURN"
PRINT "PROCEDURE doit"
IF anztree>0
  FOR i=0 TO anztree-1
    PRINT "  @formular"+STR$(i)
  NEXT i
ENDIF
IF anzfreestring>0
  FOR i=0 TO anzfreestring-1
    PRINT "  ~FORM_ALERT(1,freestring$("+STR$(i)+"))"
  NEXT i
ENDIF
PRINT "RETURN"
QUIT

PROCEDURE dotree(n$)
  klammer=0
  spaces=1
  anzobj=0
  IF LEN(n$)
    PRINT "' TREE NAME: "+n$
  ENDIF
  PRINT "PROCEDURE formular"+STR$(anztree)
  PRINT "  LOCAL ret,x,y,w,h"
  ~@doit2(-1)
  ' while count<anziz
  '   t$=iz$(count)
  '   inc count
  '   exit if t$="}"
  '   ~@doit2(-1)
  '   print t$
  ' wend
  @addtree(aobj,anzobj-1)
  PRINT "RETURN"
  INC anztree
RETURN
PROCEDURE dorsc(n$)
  LOCAL t$
  PRINT "' RSC information "+n$
  WHILE count<anziz
    t$=iz$(count)
    INC count
    EXIT IF t$="}"
    t$=REPLACE$(t$,"#","!")
    PRINT "' "+t$
  WEND
RETURN
PROCEDURE dofreestr(b$)
  LOCAL text$
  text$=@getval$(b$,"STRING")
  IF LEFT$(text$)=CHR$(34)
    text$=DECLOSE$(text$)
  ENDIF
  fSTR$(anzfreestring)=text$
  INC anzfreestring
RETURN
FUNCTION doit2(parent)
  LOCAL t$,klammer,idx,typ$,label$,obnext,obtail,obhead,parameter$
  ' print "'# DOIT2: ",parent,count
  idx=-1
  WHILE count<anziz
    t$=iz$(count)
    INC count
    EXIT IF t$="}"
    IF LEFT$(t$)="'" OR LEFT$(t$)="#"
      PRINT t$
    ELSE
      idx=anzobj
      INC anzobj
      IF RIGHT$(t$)="{"
        klammer=1
        t$=TRIM$(LEFT$(t$,LEN(t$)-1))
      ELSE
        klammer=0
      ENDIF
      SPLIT t$,":",1,label$,t$
      IF LEN(t$)=0
        t$=label$
        label$=""
      ENDIF
      SPLIT t$,"(",1,typ$,t$
      typ$=TRIM$(typ$)
      IF RIGHT$(t$)=")"
        t$=LEFT$(t$,LEN(t$)-1)
      ENDIF
      IF klammer=1
        obhead=anzobj
        obtail=@doit2(idx)
        IF obtail=-1
          obhead=-1
        ENDIF
      ELSE
        obtail=-1
        obhead=-1
      ENDIF
      IF iz$(count)="}" OR count=anziz
        obnext=parent
      ELSE
        obnext=anzobj
      ENDIF
      IF LEN(label$)
        PRINT label$+"="+STR$(idx)
      ENDIF
      @doobj(idx,obnext,obhead,obtail,typ$,t$)
    ENDIF
  WEND
  ' print "'# END DOIT2: ",parent,count
  RETURN idx
ENDFUNC

FUNCTION doit(par1,par2,countee)
  LOCAL klammer,ppp,typ$,obnext,obhead,obtail,idx,label$,t2$,parameter$
  LOCAL label$,typ$,i,t$,ss
  t$=iz$(countee)
  PRINT "# DOIT ",par1,par2,t$
  IF RIGHT$(t$)="{"
    klammer=1
    t$=TRIM$(LEFT$(t$,LEN(t$)-1))
  ELSE
    klammer=0
  ENDIF
  SPLIT t$,":",1,label$,t$
  IF LEN(t$)
  ELSE
    t$=label$
    label$=""
  ENDIF
  SPLIT t$,"(",1,typ$,t$
  typ$=TRIM$(typ$)
  IF RIGHT$(t$)=")"
    t$=LEFT$(t$,LEN(t$)-1)
  ENDIF
  idx=anzobj
  INC anzobj

  obhead=anzobj
  parameter$=t$
  IF klammer=1
    ss=@suchende(countee+1,idx)
    obtail=anzobj-1
  ELSE
    obhead=-1
    obtail=-1
  ENDIF

  IF par1=1
    obnext=par2
  ELSE
    obnext=anzobj
  ENDIF
  IF LEN(label$)
    PRINT label$+"="+STR$(idx)
  ENDIF
  on=obnext
  oh=obhead
  ot=obtail
  i=idx
  @doobj(i,on,oh,ot,typ$,parameter$)
  IF klammer
    RETURN ss+1
  ENDIF
  RETURN countee+1
RETURN
FUNCTION suchende(start,idx)
  LOCAL i,t$,k
  k=0
  i=start
  WHILE i<anziz
    t$=iz$(i)
    IF RIGHT$(t$)="{"
      i=@doit(0,idx,i)
      INC k
    ELSE IF LEFT$(t$)="}"
      IF k=0
        RETURN i
      ENDIF
      DEC k
    ELSE
      i=@doit(0,idx,i)
    ENDIF
  WEND
  RETURN -1
ENDFUNC

PROCEDURE doobj(idx,obnext,obhead,obtail,a$,b$)
  IF UPPER$(a$)="FREESTR"
    text$=@getval$(b$,"STRING")
    IF LEFT$(text$)=CHR$(34)
      text$=DECLOSE$(text$)
    ENDIF
    fSTR$(anzfreestring)=text$
    INC anzfreestring
  ELSE
    obx=VAL(@getval$(b$,"X"))*chw
    oby=VAL(@getval$(b$,"Y"))*chh
    obw=VAL(@getval$(b$,"W"))*chw
    obh=VAL(@getval$(b$,"H"))*chh
    flags$=@getval$(b$,"FLAGS")
    state$=@getval$(b$,"STATE")
    obflags=@doflags(flags$)
    obstate=@dostate(state$)
    IF UPPER$(a$)="BOX" OR UPPER$(a$)="BOXCHAR"
      obtype=20*ABS(UPPER$(a$)="BOX")+27*ABS(UPPER$(a$)="BOXCHAR")
      char$=@getval$(b$,"CHAR")
      IF LEN(char$)=1
        char=ASC(char$)
      ELSE IF LEN(char$)=3
        char=PEEK(VARPTR(char$)+1)
      ELSE
        char=0
      ENDIF
      frame=VAL(@getval$(b$,"FRAME"))
      framecol=VAL(@getval$(b$,"FRAMECOL"))
      textcol=VAL(@getval$(b$,"TEXTCOL"))
      bgcol=VAL(@getval$(b$,"BGCOL"))
      pattern=VAL(@getval$(b$,"PATTERN"))
      textmode=VAL(@getval$(b$,"TEXTMODE"))
      obspec=cvl(CHR$(16*(bgcol AND 15)+2*(pattern AND 7)+(textmode AND 1))+CHR$((framecol AND 15)+16*(textcol AND 15))+CHR$(frame)+CHR$(char))
      PRINT "obj"+STR$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      PRINT "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(";obspec;")";
    ELSE IF UPPER$(a$)="TEXT" OR UPPER$(a$)="FTEXT" OR UPPER$(a$)="BOXTEXT"
      obtype=21*abs(UPPER$(a$)="TEXT")+29*abs(UPPER$(a$)="FTEXT")+22*abs(UPPER$(a$)="BOXTEXT")
      text$=@getval$(b$,"TEXT")
      IF left$(text$)=CHR$(34)
        text$=DECLOSE$(text$)
      ENDIF
      ptmp$=@getval$(b$,"PTMP")
      IF left$(ptmp$)=CHR$(34)
        ptmp$=DECLOSE$(ptmp$)
      ENDIF
      pvalid$=@getval$(b$,"PVALID")
      IF left$(pvalid$)=CHR$(34)
        pvalid$=DECLOSE$(pvalid$)
      ENDIF
      font=VAL(@getval$(b$,"FONT"))
      IF @getval$(b$,"FONT")=""
        font=3
      ENDIF
      just=VAL(@getval$(b$,"JUST"))
      color=VAL(@getval$(b$,"COLOR"))
      border=VAL(@getval$(b$,"BORDER"))
      @addtedinfo(text$,ptmp$,pvalid$,font,just,color,border)
      PRINT "obj"+STR$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      PRINT "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(varptr(tedinfo";anztedinfo-1;"$))";
    ELSE IF UPPER$(a$)="STRING" OR UPPER$(a$)="TITLE" OR UPPER$(a$)="BUTTON"
      obtype=28*abs(UPPER$(a$)="STRING")+32*abs(UPPER$(a$)="TITLE")+26*abs(UPPER$(a$)="BUTTON")
      text$=@getval$(b$,"TEXT")
      IF left$(text$)=CHR$(34)
        text$=DECLOSE$(text$)
      ENDIF
      @addstring(text$)
      PRINT "obj"+STR$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      PRINT "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(varptr(string";anzstring-1;"$))";
    ELSE IF UPPER$(a$)="IMAGE"
      data$=@getval$(b$,"DATA")
      iw=VAL(@getval$(b$,"IW"))
      ih=VAL(@getval$(b$,"IH"))
      ix=VAL(@getval$(b$,"IX"))
      iy=VAL(@getval$(b$,"IY"))
      ic=VAL(@getval$(b$,"IC"))
      @addbitblk(data$,iw,ih,ix,iy,ic)
      PRINT "obj"+STR$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      PRINT "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(varptr(bitblk";anzbitblk-1;"$))";
    ELSE IF UPPER$(a$)="ICON"
      data$=@getval$(b$,"DATA")
      mask$=@getval$(b$,"MASK")
      text$=@getval$(b$,"TEXT")
      char=VAL(@getval$(b$,"CHAR"))
      xchar=VAL(@getval$(b$,"XCHAR"))
      ychar=VAL(@getval$(b$,"YCHAR"))
      xicon=VAL(@getval$(b$,"XICON"))
      yicon=VAL(@getval$(b$,"YICON"))
      wicon=VAL(@getval$(b$,"WICON"))
      hicon=VAL(@getval$(b$,"HICON"))
      xtext=VAL(@getval$(b$,"XTEXT"))
      ytext=VAL(@getval$(b$,"YTEXT"))
      wtext=VAL(@getval$(b$,"WTEXT"))
      htext=VAL(@getval$(b$,"HTEXT"))
      IF LEFT$(text$)=CHR$(34)
        text$=DECLOSE$(text$)
      ENDIF

      @addiconblk(data$,mask$,text$,char,xchar,ychar,xicon,yicon,wicon,hicon,xtext,ytext,wtext,htext)
      PRINT "obj"+STR$(idx)+"$=mki$(";obnext;")+mki$(";obhead;")+mki$(";obtail;")";
      PRINT "+mki$(";obtype;")+mki$(";obflags;")+mki$(";obstate;")+mkl$(varptr(iconblk";anziconblk-1;"$))";
    ELSE IF UPPER$(a$)="UNKNOWN"
      PRINT "' UNKNOWN: ";b$
    ELSE
      PRINT "' unsupported: "+a$,parent
      RETURN
    ENDIF
    PRINT "+mki$(";obx;")+mki$(";oby;")+mki$(";obw;")+mki$(";obh;")"
  ENDIF
RETURN
PROCEDURE addtree(aob,oobj)
  LOCAL t$,i
  t$="tree"+STR$(anztree)+"$="
  FOR i=aobj TO oobj
    t$=t$+"obj"+STR$(i)+"$"
    IF i<>oobj
      IF LEN(t$)>70
        PRINT SPACE$(spaces*2)+t$
        t$="tree"+STR$(anztree)+"$=tree"+STR$(anztree)+"$+"
      ELSE
        t$=t$+"+"
      ENDIF
    ENDIF
  NEXT i
  PRINT SPACE$(spaces*2)+t$
  PRINT "  ~form_center(varptr(tree"+STR$(anztree)+"$),x,y,w,h)"
  PRINT "  ~form_dial(0,0,0,0,0,x,y,w,h)"
  PRINT "  ~form_dial(1,0,0,0,0,x,y,w,h)"
  PRINT "  ~objc_draw(varptr(tree"+STR$(anztree)+"$),0,-1,0,0,,)"
  PRINT "  ret=FORM_DO(VARPTR(tree"+STR$(anztree)+"$))"
  PRINT "  ~form_dial(2,0,0,0,0,x,y,w,h)"
  PRINT "  ~form_dial(3,0,0,0,0,x,y,w,h)"
  PRINT "  showpage"
RETURN
PROCEDURE addstring(r$)
  PRINT "string"+STR$(anzstring)+"$="+ENCLOSE$(r$)+"+chr$(0)"
  INC anzstring
RETURN
PROCEDURE adddata(r$)
  PRINT "data"+STR$(anzdata)+"$=INLINE$("+r$+")"
  INC anzdata
RETURN
PROCEDURE addtedinfo(a$,b$,c$,d,e,f,g)
  PRINT "string"+STR$(anzstring)+"$="+ENCLOSE$(a$)+"+chr$(0)+space$("+STR$(LEN(c$))+")"
  INC anzstring
  @addstring(b$)
  @addstring(c$)
  PRINT "tedinfo"+STR$(anztedinfo)+"$=mkl$(varptr(string"+STR$(anzstring-3)+"$))";
  PRINT "+mkl$(varptr(string"+STR$(anzstring-2)+"$))";
  PRINT "+mkl$(varptr(string"+STR$(anzstring-1)+"$))";
  PRINT "+mki$(";d;")+mki$(0)+mki$(";e;")+mki$(";f;")+mki$(0)+mki$(";g;")+mki$(";LEN(b$);")+mki$(";LEN(c$);")"
  INC anztedinfo
RETURN
PROCEDURE addbitblk(a$,b,c,d,e,f)
  @adddata(a$)
  PRINT "bitblk"+STR$(anzbitblk)+"$=mkl$(varptr(data"+STR$(anzdata-1)+"$))";
  PRINT "+mki$(";b;")+mki$(";c;")+mki$(";d;")+mki$(";e;")+mki$(";f;")"
  INC anzbitblk
RETURN
PROCEDURE addiconblk(a$,b$,c$,b,c,d,e,f,g,h,i,j,k,l)
  @adddata(a$)
  @adddata(b$)
  @addstring(c$)
  PRINT "iconblk"+STR$(anziconblk)+"$=mkl$(varptr(data"+STR$(anzdata-2)+"$))";
  PRINT "+mkl$(varptr(data"+STR$(anzdata-1)+"$))";
  PRINT "+mkl$(varptr(string"+STR$(anzstring-1)+"$))";
  PRINT "+mki$(";b;")+mki$(";c;")+mki$(";d;")+mki$(";e;")+mki$(";f;")";
  PRINT "+mki$(";g;")+mki$(";h;")+mki$(";i;")+mki$(";j;")+mki$(";k;")+mki$(";l;")"
  INC anziconblk
RETURN
FUNCTION getval$(t$,f$)
  LOCAL a$,val$
  val$=""
  SPLIT t$,",",1,a$,t$
  WHILE LEN(a$)
    a$=TRIM$(a$)
    SPLIT a$,"=",1,name$,val$
    EXIT IF UPPER$(name$)=UPPER$(f$)
    val$=""
    SPLIT t$,",",1,a$,t$
  WEND
  RETURN val$
ENDFUNCTION
FUNCTION doflags(t$)
  LOCAL ret,a$
  ret=0
  IF LEFT$(t$)="("
    t$=RIGHT$(t$,LEN(t$)-1)
  ENDIF
  IF RIGHT$(t$)=")"
    t$=LEFT$(t$,LEN(t$)-1)
  ENDIF
  SPLIT t$,"+",1,a$,t$
  WHILE LEN(a$)
    IF a$="NONE"
    ELSE IF a$="SELECTABLE"
      ret=ret OR 1
    ELSE IF a$="DEFAULT"
      ret=ret OR 2
    ELSE IF a$="EXIT"
      ret=ret OR 4
    ELSE IF a$="EDITABLE"
      ret=ret OR 8
    ELSE IF a$="RADIOBUTTON"
      ret=ret OR 16
    ELSE IF a$="LASTOB"
      ret=ret OR 32
    ELSE IF a$="TOUCHEXIT"
      ret=ret OR 64
    ELSE IF a$="HIDETREE"
      ret=ret OR 128
    ELSE IF a$="INDIRECT"
      ret=ret OR 256
    ELSE
      PRINT "Unknown flag:",a$
    ENDIF
    SPLIT t$,"+",1,a$,t$
  WEND
  RETURN ret
ENDFUNC
FUNCTION dostate(t$)
  LOCAL ret,a$
  ret=0
  IF LEFT$(t$)="("
    t$=RIGHT$(t$,LEN(t$)-1)
  ENDIF
  IF RIGHT$(t$)=")"
    t$=LEFT$(t$,LEN(t$)-1)
  ENDIF
  SPLIT t$,"+",1,a$,t$
  WHILE LEN(a$)
    IF a$="NORMAL"
      ' do nothing
    ELSE IF a$="SELECTED"
      ret=ret OR 1
    ELSE IF a$="CROSSED"
      ret=ret OR 2
    ELSE IF a$="CHECKED"
      ret=ret OR 4
    ELSE IF a$="DISABLED"
      ret=ret OR 8
    ELSE IF a$="OUTLINED"
      ret=ret OR 16
    ELSE IF a$="SHADOWED"
      ret=ret OR 32
    ELSE IF a$="WHITEBAK"
      ret=ret OR 64
    ELSE IF a$="WHITEBACK"
      ret=ret OR 64
    ELSE IF a$="DRAW3D"
      ret=ret OR 128
    ELSE
      PRINT "Unknown state:",a$
    ENDIF
    SPLIT t$,"+",1,a$,t$
  WEND
  RETURN ret
ENDFUNC
